//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/validation/validation_event_handler.h"

#include "vda5050++/core/checks/action.h"
#include "vda5050++/core/checks/header.h"
#include "vda5050++/core/checks/order.h"
#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/logger.h"

using namespace vda5050pp::core::validation;
using namespace std::chrono_literals;

inline std::list<vda5050::Error> queryAcceptZoneSet(std::string_view zone_set_id) {
  auto evt = std::make_shared<vda5050pp::events::QueryAcceptZoneSet>();
  evt->zone_set_id = zone_set_id;
  auto future = evt->getFuture();

  vda5050pp::core::Instance::ref().getQueryEventManager().dispatch(evt, false);

  if (future.wait_for(1s) != std::future_status::ready) {
    throw vda5050pp::VDA5050PPSynchronizedEventTimedOut(MK_FN_EX_CONTEXT(
        fmt::format("QueryAcceptZoneSet(zone_set_id={}) timed out.", zone_set_id)));
  }

  return future.get();
}

void ValidationEventHandler::handleValidateOrder(
    std::shared_ptr<vda5050pp::core::events::ValidateOrderEvent> evt) const {
  if (evt == nullptr || evt->order == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("ValidateOrderEvent is empty"));
  }

  auto result_token = evt->acquireResultToken();
  if (!result_token.isAcquired()) {
    getValidationLogger()->warn("{} could not acquire result token for Order(headerId={}).",
                                this->describe(), evt->order->header.headerId);
    return;
  }

  // Run all checks
  std::list<vda5050::Error> errors;
  errors.splice(errors.end(), checks::checkHeader(evt->order->header));
  errors.splice(errors.end(), checks::checkOrderId(*evt->order));
  errors.splice(errors.end(), checks::checkOrderGraphConsistency(*evt->order));
  errors.splice(errors.end(), checks::checkOrderAppend(*evt->order));
  errors.splice(errors.end(), checks::checkOrderActionIds(*evt->order));
  if (evt->order->zoneSetId.has_value()) {
    errors.splice(errors.end(), queryAcceptZoneSet(*evt->order->zoneSetId));
  }

  std::vector<std::tuple<std::shared_ptr<const vda5050pp::events::ActionValidate>,
                         std::shared_ptr<const vda5050::Action>,
                         std::future<vda5050pp::core::events::ValidationResult>>>
      results;

  // Synchronously send validate for each action to the user interface
  for (const auto &node : evt->order->nodes) {
    for (const auto &action : node.actions) {
      getValidationLogger()->debug("Sending ActionValidate(action={}, node) to AGV interface",
                                   action.actionId);
      auto v_evt = std::make_shared<vda5050pp::events::ActionValidate>();
      v_evt->action = std::make_shared<vda5050::Action>(action);
      v_evt->context = vda5050pp::misc::ActionContext::k_node;
      v_evt->keep = node.released;
      results.emplace_back(v_evt, v_evt->action, v_evt->getFuture());
      vda5050pp::core::Instance::ref().getActionEventManager().dispatch(v_evt, true);
    }
  }
  for (const auto &edge : evt->order->edges) {
    for (const auto &action : edge.actions) {
      getValidationLogger()->debug("Sending ActionValidate(action={}, edge) to AGV interface",
                                   action.actionId);
      auto v_evt = std::make_shared<vda5050pp::events::ActionValidate>();
      v_evt->action = std::make_shared<vda5050::Action>(action);
      v_evt->context = vda5050pp::misc::ActionContext::k_edge;
      v_evt->keep = edge.released;
      results.emplace_back(v_evt, v_evt->action, v_evt->getFuture());
      vda5050pp::core::Instance::ref().getActionEventManager().dispatch(v_evt, true);
    }
  }

  // Gather results (if no result is available, the action is considered unknown)
  for (auto &[_, action, future] : results) {
    if (future.wait_for(0s) != std::future_status::ready) {
      getValidationLogger()->debug("Future for action {} unavailable -> unknown action",
                                   action->actionType);
      vda5050::Error unknown_action;
      unknown_action.errorType = "unknownActionType";
      unknown_action.errorLevel = vda5050::ErrorLevel::WARNING;
      unknown_action.errorDescription =
          fmt::format("Unknown action Type {} encountered", action->actionType);
      unknown_action.errorReferences = {
          {{"action.actionType", action->actionType}, {"action.actionId", action->actionId}}};
      errors.emplace_back(std::move(unknown_action));
    } else {
      getValidationLogger()->debug("Got validation result for Action {}", action->actionId);
      errors.splice(errors.end(), future.get());
    }
  }

  getValidationLogger()->debug("Validation yield {} error for order(headerId={})", errors.size(),
                               evt->order->header.headerId);
  result_token.setValue(std::move(errors));
}

void ValidationEventHandler::handleValidateInstantActions(
    std::shared_ptr<vda5050pp::core::events::ValidateInstantActionsEvent> evt) const {
  if (evt == nullptr || evt->instant_actions == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(
        MK_EX_CONTEXT("ValidateInstantActionsEvent is empty"));
  }

  auto result_token = evt->acquireResultToken();
  if (!result_token.isAcquired()) {
    getValidationLogger()->warn("{} could not acquire result token for InstantAction(headerId={}).",
                                this->describe(), evt->instant_actions->header.headerId);
    return;
  }

  std::list<vda5050::Error> errors;
  std::set<std::string_view, std::less<>> seen_ids;
  std::vector<std::tuple<std::shared_ptr<const vda5050pp::events::ActionValidate>,
                         std::shared_ptr<const vda5050::Action>,
                         std::future<vda5050pp::core::events::ValidationResult>>>
      results;

  // Synchronously send validate for each action to the user interface
  for (const auto &action : evt->instant_actions->actions) {
    // Also check uniqueness before
    errors.splice(errors.end(), checks::uniqueActionId(action, seen_ids));

    getValidationLogger()->debug("Sending ActionValidate(action={}, instant) to AGV interface",
                                 action.actionId);
    auto v_evt = std::make_shared<vda5050pp::events::ActionValidate>();
    v_evt->action = std::make_shared<vda5050::Action>(action);
    v_evt->context = vda5050pp::misc::ActionContext::k_instant;
    v_evt->keep = false;
    results.emplace_back(v_evt, v_evt->action, v_evt->getFuture());
    vda5050pp::core::Instance::ref().getActionEventManager().dispatch(v_evt, true);
  }

  // Gather results (if no result is available, the action is considered unknown)
  for (auto &[_, action, future] : results) {
    if (future.wait_for(0s) != std::future_status::ready) {
      getValidationLogger()->debug("Future for action {} unavailable -> unknown action",
                                   action->actionType);
      vda5050::Error unknown_action;
      unknown_action.errorType = "unknownActionType";
      unknown_action.errorLevel = vda5050::ErrorLevel::WARNING;
      unknown_action.errorDescription =
          fmt::format("Unknown action Type {} encountered", action->actionType);
      unknown_action.errorReferences = {
          {{"action.actionType", action->actionType}, {"action.actionId", action->actionId}}};
      errors.emplace_back(std::move(unknown_action));
    } else {
      getValidationLogger()->debug("Got validation result for Action {}", action->actionId);
      errors.splice(errors.end(), future.get());
    }
  }

  getValidationLogger()->debug("handleValidateInstantActions(): gathered results -> {} errors",
                               errors.size());
  result_token.setValue(std::move(errors));
}

void ValidationEventHandler::initialize(vda5050pp::core::Instance &instance) {
  this->subscriber_ = instance.getValidationEventManager().getScopedSubscriber();

  this->subscriber_->subscribe<vda5050pp::core::events::ValidateOrderEvent>(
      [this](auto arg) { this->handleValidateOrder(arg); });
  this->subscriber_->subscribe<vda5050pp::core::events::ValidateInstantActionsEvent>(
      [this](auto arg) { this->handleValidateInstantActions(arg); });
}

void ValidationEventHandler::deinitialize(vda5050pp::core::Instance &) {
  this->subscriber_.reset();
}

std::string_view ValidationEventHandler::describe() const { return "ValidationEventHandler"; }