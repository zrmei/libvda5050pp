//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/core/agv_handler/action_event_handler.h"

#include <fmt/format.h>

#include <functional>

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/instance.h"
#include "vda5050++/core/logger.h"

using namespace vda5050pp::core::agv_handler;

void ActionEventHandler::initialize(vda5050pp::core::Instance &instance) noexcept(false) {
  this->subscriber_ = instance.getActionEventManager().getScopedActionEventSubscriber();

  this->subscriber_->subscribe(std::bind(std::mem_fn(&ActionEventHandler::handleValidateEvent),
                                         this, std::placeholders::_1));
  this->subscriber_->subscribe(
      std::bind(std::mem_fn(&ActionEventHandler::handlePrepareEvent), this, std::placeholders::_1));
  this->subscriber_->subscribe(
      std::bind(std::mem_fn(&ActionEventHandler::handleCancelEvent), this, std::placeholders::_1));
  this->subscriber_->subscribe(
      std::bind(std::mem_fn(&ActionEventHandler::handlePauseEvent), this, std::placeholders::_1));
  this->subscriber_->subscribe(
      std::bind(std::mem_fn(&ActionEventHandler::handleResumeEvent), this, std::placeholders::_1));
  this->subscriber_->subscribe(
      std::bind(std::mem_fn(&ActionEventHandler::handleStartEvent), this, std::placeholders::_1));
  this->subscriber_->subscribe(std::bind(std::mem_fn(&ActionEventHandler::handleActionListEvent),
                                         this, std::placeholders::_1));
  this->subscriber_->subscribe(
      std::bind(std::mem_fn(&ActionEventHandler::handleForgetEvent), this, std::placeholders::_1));
}

void ActionEventHandler::deinitialize(vda5050pp::core::Instance &) noexcept(false) {
  this->subscriber_.reset();
}

std::string_view ActionEventHandler::describe() const { return "ActionEventHandler"; }

void ActionEventHandler::handleValidateEvent(
    std::shared_ptr<vda5050pp::events::ActionValidate> data) noexcept(false) {
  // Check fatal conditions
  if (data == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("ActionValidate contains no data"));
  }
  if (data->action == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("ActionValidate contains no action"));
  }

  getAGVHandlerLogger()->debug("handleValidateEvent(action_type={}, action_id={})",
                               data->action->actionType, data->action->actionId);

  auto result_token = data->acquireResultToken();
  if (!result_token.isAcquired()) {
    getAGVHandlerLogger()->debug("Could not acquire resultToken for action(id={})",
                                 data->action->actionId);
    return;
  }

  // Search appropiate handler
  for (auto action_handler : Instance::ref().getActionHandler()) {
    if (action_handler->match(*data->action)) {
      // Get the validation result
      vda5050pp::handler::ValidationResult validation_result;
      try {
        validation_result = action_handler->validate(*data->action, data->context);
      } catch (vda5050pp::VDA5050PPError &e) {
        e.addAdditionalContext("action_handler.action_id", data->action->actionId);
        getAGVHandlerLogger()->error(
            "ActionHandler(@{}).validate threw an exception, cannot validate.",
            data->action->actionId);
        result_token.setException(std::current_exception());
        throw std::move(e);
      }

      // Register a new action store if there was no error and the action is meant to be kept
      if (validation_result.errors.empty() && data->keep) {
        ActionStore store;
        store.action = data->action;
        store.action_parameters = std::make_shared<
            std::map<std::string, vda5050pp::handler::ParameterValue, std::less<>>>(
            std::move(validation_result.parameters));
        store.action_handler = action_handler;

        this->handled_actions_.insert_or_assign(data->action->actionId, std::move(store));
      }

      result_token.setValue(std::move(validation_result.errors));

      return;
    }
  }

  // Not matched
  getAGVHandlerLogger()->debug("Action(type={}, id={}) is unhandled.", data->action->actionType,
                               data->action->actionId);
  result_token.release();
}

void ActionEventHandler::handleActionListEvent(
    std::shared_ptr<vda5050pp::events::ActionList> data) const noexcept(false) {
  // Check fatal conditions
  if (data == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("ActionList contains no data"));
  }

  getAGVHandlerLogger()->debug("handleActionListEvent()");

  auto result = data->acquireResultToken();
  if (!result.isAcquired()) {
    getAGVHandlerLogger()->debug("handleActionListEvent(): could not acquire result token.");
    return;
  }

  // Use the descriptions of all handled actions
  auto agv_actions = std::make_shared<std::list<vda5050::AgvAction>>();
  for (const auto &handler : Instance::ref().getActionHandler()) {
    agv_actions->splice(agv_actions->end(), handler->getActionDescription());
  }
  result.setValue(std::move(agv_actions));
}

void ActionEventHandler::handlePrepareEvent(
    std::shared_ptr<vda5050pp::events::ActionPrepare> data) noexcept(false) {
  // Check fatal conditions
  if (data == nullptr || data->action == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("ActionPrepare contains no data"));
  }

  // Check if this action is handled
  auto maybe_store = this->tryFindHandledAction(data->action->actionId);
  if (!maybe_store.has_value()) {
    getAGVHandlerLogger()->debug("handlePrepareEvent(id={}, type={}) skip", data->action->actionId,
                                 data->action->actionType);
    return;
  }
  auto &store = maybe_store->get();

  if (store.action_handler == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(MK_EX_CONTEXT("store.action_handler nullptr"));
  }

  // Create new action_state
  store.action_state = std::make_shared<vda5050pp::core::agv_handler::ActionState>(data->action);

  // Prepare and store the returned callbacks
  try {
    store.action_callbacks = std::make_shared<vda5050pp::handler::ActionCallbacks>(
        store.action_handler->prepare(store.action_state, store.action_parameters));
  } catch (vda5050pp::VDA5050PPError &e) {
    e.addAdditionalContext("action_handler.action_id", store.action->actionId);
    getAGVHandlerLogger()->error("ActionHandler(@{}).prepare threw an exception, cannot prepare.",
                                 store.action->actionId);
    throw std::move(e);
  }
}

void ActionEventHandler::handleCancelEvent(
    std::shared_ptr<vda5050pp::events::ActionCancel> data) noexcept(false) {
  // Check fatal conditions
  if (data == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("ActionCancel contains no data"));
  }

  // Check if this action is handled
  auto maybe_store = this->tryFindHandledAction(data->action_id);
  if (!maybe_store.has_value()) {
    getAGVHandlerLogger()->debug("handleCancelEvent(id={}) skip", data->action_id);
    return;
  }
  const auto &store = maybe_store->get();

  if (store.action_callbacks == nullptr || store.action_callbacks->on_cancel == nullptr) {
    throw VDA5050PPCallbackNotSet(MK_EX_CONTEXT("on_cancel is null"));
  }

  if (store.action_state == nullptr) {
    throw VDA5050PPNullPointer(MK_EX_CONTEXT("store.action_state is null"));
  }

  store.action_callbacks->on_cancel(*store.action_state);
}

void ActionEventHandler::handlePauseEvent(
    std::shared_ptr<vda5050pp::events::ActionPause> data) noexcept(false) {
  // Check fatal conditions
  if (data == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("ActionPause contains no data"));
  }

  // Check if this action is handled
  auto maybe_store = this->tryFindHandledAction(data->action_id);
  if (!maybe_store.has_value()) {
    getAGVHandlerLogger()->debug("handlePauseEvent(id={}) skip", data->action_id);
    return;
  }
  const auto &store = maybe_store->get();

  if (store.action_callbacks == nullptr || store.action_callbacks->on_pause == nullptr) {
    throw VDA5050PPCallbackNotSet(MK_EX_CONTEXT("on_pause is null"));
  }

  if (store.action_state == nullptr) {
    throw VDA5050PPNullPointer(MK_EX_CONTEXT("store.action_state is null"));
  }

  store.action_callbacks->on_pause(*store.action_state);
}

void ActionEventHandler::handleResumeEvent(
    std::shared_ptr<vda5050pp::events::ActionResume> data) noexcept(false) {
  // Check fatal conditions
  if (data == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("ActionResume contains no data"));
  }

  // Check if this action is handled
  auto maybe_store = this->tryFindHandledAction(data->action_id);
  if (!maybe_store.has_value()) {
    getAGVHandlerLogger()->debug("handleResumeEvent(id={}) skip", data->action_id);
    return;
  }
  const auto &store = maybe_store->get();

  if (store.action_callbacks == nullptr || store.action_callbacks->on_resume == nullptr) {
    throw VDA5050PPCallbackNotSet(MK_EX_CONTEXT("on_resume is null"));
  }

  if (store.action_state == nullptr) {
    throw VDA5050PPNullPointer(MK_EX_CONTEXT("store.action_state is null"));
  }

  store.action_callbacks->on_resume(*store.action_state);
}

void ActionEventHandler::handleStartEvent(
    std::shared_ptr<vda5050pp::events::ActionStart> data) noexcept(false) {
  // Check fatal conditions
  if (data == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("ActionStart contains no data"));
  }

  // Check if this action is handled
  auto maybe_store = this->tryFindHandledAction(data->action_id);
  if (!maybe_store.has_value()) {
    getAGVHandlerLogger()->debug("handleStartEvent(id={}) skip", data->action_id);
    return;
  }
  const auto &store = maybe_store->get();

  if (store.action_callbacks == nullptr || store.action_callbacks->on_start == nullptr) {
    throw VDA5050PPCallbackNotSet(MK_EX_CONTEXT("on_start is null"));
  }

  if (store.action_state == nullptr) {
    throw VDA5050PPNullPointer(MK_EX_CONTEXT("store.action_state is null"));
  }

  store.action_callbacks->on_start(*store.action_state);
}

void ActionEventHandler::handleForgetEvent(
    std::shared_ptr<vda5050pp::events::ActionForget> data) noexcept(false) {
  // Check fatal conditions
  if (data == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("ActionForget contains no data"));
  }

  auto success = this->tryRemoveActionStore(data->action_id);

  getAGVHandlerLogger()->debug("handleForgetEvent(action_id={}): success={}", data->action_id,
                               success);
}

std::optional<std::reference_wrapper<ActionEventHandler::ActionStore>>
ActionEventHandler::tryFindHandledAction(std::string_view action_id) {
  auto it = this->handled_actions_.find(action_id);

  if (it == this->handled_actions_.end()) {
    return std::nullopt;
  }

  return it->second;
}

bool ActionEventHandler::tryRemoveActionStore(std::string_view action_id) {
  auto it = this->handled_actions_.find(action_id);

  if (it == this->handled_actions_.end()) {
    return false;
  }

  this->handled_actions_.erase(it);
  return true;
}