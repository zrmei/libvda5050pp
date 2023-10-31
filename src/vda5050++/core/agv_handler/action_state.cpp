//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/agv_handler/action_state.h"

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/instance.h"

using namespace vda5050pp::core::agv_handler;

ActionState::ActionState(std::shared_ptr<const vda5050::Action> action) noexcept(true)
    : vda5050pp::handler::ActionState(std::move(action)) {}

void ActionState::setRunning() noexcept(false) {
  auto &manager = Instance::ref().getActionStatusManager();

  auto event = std::make_shared<vda5050pp::events::ActionStatusRunning>();
  event->action_id = this->getAction().actionId;

  manager.dispatch(event);
}

void ActionState::setPaused() noexcept(false) {
  auto &manager = Instance::ref().getActionStatusManager();

  auto event = std::make_shared<vda5050pp::events::ActionStatusPaused>();
  event->action_id = this->getAction().actionId;

  manager.dispatch(event);
}

void ActionState::setFinished() noexcept(false) {
  auto &manager = Instance::ref().getActionStatusManager();

  auto event = std::make_shared<vda5050pp::events::ActionStatusFinished>();
  event->action_id = this->getAction().actionId;
  event->action_result = std::nullopt;

  manager.dispatch(event);
}

void ActionState::setFinished(std::string_view result_code) noexcept(false) {
  auto &manager = Instance::ref().getActionStatusManager();

  auto event = std::make_shared<vda5050pp::events::ActionStatusFinished>();
  event->action_id = this->getAction().actionId;
  event->action_result = result_code;

  manager.dispatch(event);
}

void ActionState::setFailed() noexcept(false) {
  auto &manager = Instance::ref().getActionStatusManager();

  auto event = std::make_shared<vda5050pp::events::ActionStatusFailed>();
  event->action_id = this->getAction().actionId;
  event->action_errors = {};

  manager.dispatch(event);
}

void ActionState::setFailed(const std::list<vda5050::Error> &errors) noexcept(false) {
  auto &manager = Instance::ref().getActionStatusManager();

  auto event = std::make_shared<vda5050pp::events::ActionStatusFailed>();
  event->action_id = this->getAction().actionId;
  event->action_errors = errors;

  manager.dispatch(event);
}