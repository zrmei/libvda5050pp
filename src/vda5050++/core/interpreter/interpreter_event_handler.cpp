//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/interpreter/interpreter_event_handler.h"

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/events/interpreter_event.h"
#include "vda5050++/core/instance.h"
#include "vda5050++/core/interpreter/control_instant_actions.h"
#include "vda5050++/core/interpreter/functional.h"
#include "vda5050++/core/logger.h"

using namespace vda5050pp::core::interpreter;

void InterpreterEventHandler::handleValidInstantActions(
    std::shared_ptr<vda5050pp::core::events::ValidInstantActionMessageEvent> data) noexcept(false) {
  if (data == nullptr || data->valid_instant_actions == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("ValidInstantActionMessageEvent"));
  }

  auto instant_action_evt = std::make_shared<vda5050pp::core::events::YieldInstantActionGroup>();

  // Publish each actions as NewAction and then as InstantAction
  for (const auto &action : data->valid_instant_actions->actions) {
    getInterpreterLogger()->debug("Propagating new action(id={})", action.actionId);

    auto new_action_evt = std::make_shared<vda5050pp::core::events::YieldNewAction>();
    new_action_evt->action = std::make_shared<vda5050::Action>(action);
    vda5050pp::core::Instance::ref().getInterpreterEventManager().synchronousDispatch(
        new_action_evt);

    if (isControlInstantAction(action)) {
      // Handle control instant actions separately
      auto ctrl = makeControlInstantActionControlBlock(new_action_evt->action);
      ctrl->setTeardown([this, a = new_action_evt->action] {
        this->active_control_blocks_.extract(a->actionId);
      });
      this->active_control_blocks_.insert_or_assign(action.actionId, ctrl);
      ctrl->enable();

    } else {
      // Add to scheduler group
      instant_action_evt->instant_actions.push_back(new_action_evt->action);
      if (new_action_evt->action->blockingType == vda5050::BlockingType::HARD) {
        instant_action_evt->blocking_type_ceiling = vda5050::BlockingType::HARD;
      } else if (new_action_evt->action->blockingType == vda5050::BlockingType::SOFT &&
                 instant_action_evt->blocking_type_ceiling != vda5050::BlockingType::HARD) {
        instant_action_evt->blocking_type_ceiling = vda5050::BlockingType::SOFT;
      }
    }
  }

  if (!instant_action_evt->instant_actions.empty()) {
    vda5050pp::core::Instance::ref().getInterpreterEventManager().dispatch(instant_action_evt);
  }
}

void InterpreterEventHandler::handleValidOrder(
    std::shared_ptr<vda5050pp::core::events::ValidOrderMessageEvent> data) const noexcept(false) {
  if (data == nullptr || data->valid_order == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("ValidOrderMessageEvent"));
  }

  getInterpreterLogger()->info("Interpreting Order(id={}, update_id={})",
                               data->valid_order->orderId, data->valid_order->orderUpdateId);

  auto event_iter_ptr = EventIter::fromOrder(*data->valid_order);

  // Iterate over all interpreter events and dispatch them in order
  for (;;) {
    auto [event, new_event_iter_ptr] = nextEvent(std::move(event_iter_ptr));
    event_iter_ptr = std::move(new_event_iter_ptr);
    if (event == nullptr) {
      break;
    }
    getInterpreterLogger()->debug("Yielding new event, type_id={}", int(event->getId()));
    Instance::ref().getInterpreterEventManager().dispatch(event);
  }

  // Done interpreting
  Instance::ref().getInterpreterEventManager().dispatch(
      std::make_shared<vda5050pp::core::events::InterpreterDone>());
  getInterpreterLogger()->debug("Done Interpreting");

  // Request state update after order update
  auto update = std::make_shared<vda5050pp::core::events::RequestStateUpdateEvent>();
  update->urgency = state::StateUpdateUrgency::high();
  Instance::ref().getStateEventManager().dispatch(update);
}

void InterpreterEventHandler::handleActionValidateEvent(
    std::shared_ptr<vda5050pp::events::ActionValidate> data) const noexcept(false) {
  getInterpreterLogger()->debug("handleActionValidateEvent()");
  if (data == nullptr || data->action == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("ActionValidateEvent"));
  }

  auto result_token = data->acquireResultToken();
  if (!result_token.isAcquired()) {
    getInterpreterLogger()->debug("Could not acquire result token -> skipping action(id={})",
                                  data->action->actionId);
    return;
  }

  if (!isControlInstantAction(*data->action)) {
    getInterpreterLogger()->debug("Action (id={}, type={}) is not a control action",
                                  data->action->actionId, data->action->actionType);
    result_token.release();
    return;
  }

  vda5050pp::core::events::ValidationResult result =
      validateControlInstantAction(*data->action, data->context);
  result_token.setValue(std::move(result));
}

void InterpreterEventHandler::initialize(vda5050pp::core::Instance &instance) {
  this->message_subscriber_ = instance.getMessageEventManager().getScopedSubscriber();
  this->action_subscriber_ = instance.getActionEventManager().getScopedActionEventSubscriber();
  this->factsheet_subscriber_ = instance.getFactsheetEventManager().getScopedSubscriber();

  this->message_subscriber_->subscribe<vda5050pp::core::events::ValidInstantActionMessageEvent>(
      [this](auto arg) { this->handleValidInstantActions(arg); });
  this->message_subscriber_->subscribe<vda5050pp::core::events::ValidOrderMessageEvent>(
      [this](auto arg) { this->handleValidOrder(arg); });
  this->action_subscriber_->subscribe(
      std::bind(std::mem_fn(&InterpreterEventHandler::handleActionValidateEvent), this,
                std::placeholders::_1));
  this->factsheet_subscriber_->subscribe<vda5050pp::core::events::FactsheetControlActionListEvent>(
      [this](auto arg) { this->handleFactsheetControlActionListEvent(arg); });
}

void InterpreterEventHandler::handleFactsheetControlActionListEvent(
    std::shared_ptr<vda5050pp::core::events::FactsheetControlActionListEvent> data) const
    noexcept(false) {
  getInterpreterLogger()->debug("handleFactsheetControlActionListEvent()");
  if (data == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("FactsheetControlActionListEvent"));
  }

  auto result = data->acquireResultToken();
  if (!result.isAcquired()) {
    throw vda5050pp::VDA5050PPSynchronizedEventNotAcquired(
        MK_EX_CONTEXT("Could not acquire FactsheetControlActionListEvent"));
  }

  result.setValue(interpreter::listControlActions());
}

void InterpreterEventHandler::deinitialize(vda5050pp::core::Instance &) {
  this->active_control_blocks_.clear();
  this->message_subscriber_.reset();
  this->action_subscriber_.reset();
  this->factsheet_subscriber_.reset();
}

std::string_view InterpreterEventHandler::describe() const { return "InterpreterEventHandler"; }