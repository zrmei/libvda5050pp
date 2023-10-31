//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/core/action_status_manager.h"

#include <eventpp/utilities/argumentadapter.h>

#include <chrono>

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/common/formatters.h"
#include "vda5050++/core/logger.h"

using namespace vda5050pp::core;
using namespace std::chrono_literals;

ScopedActionStatusSubscriber::ScopedActionStatusSubscriber(ActionStatusQueue &queue)
    : remover_(queue) {}

void ScopedActionStatusSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::ActionStatusWaiting>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::ActionStatusType::k_action_status_waiting,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::ActionStatusWaiting>)>(
          std::move(callback)));
}

void ScopedActionStatusSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::ActionStatusInitializing>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::ActionStatusType::k_action_status_initializing,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::ActionStatusInitializing>)>(
          std::move(callback)));
}

void ScopedActionStatusSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::ActionStatusRunning>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::ActionStatusType::k_action_status_running,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::ActionStatusRunning>)>(
          std::move(callback)));
}

void ScopedActionStatusSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::ActionStatusPaused>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::ActionStatusType::k_action_status_paused,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::ActionStatusPaused>)>(
          std::move(callback)));
}

void ScopedActionStatusSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::ActionStatusFinished>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::ActionStatusType::k_action_status_finished,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::ActionStatusFinished>)>(
          std::move(callback)));
}

void ScopedActionStatusSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::ActionStatusFailed>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::ActionStatusType::k_action_status_failed,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::ActionStatusFailed>)>(
          std::move(callback)));
}

ActionStatusManager::ActionStatusManager(const vda5050pp::config::EventManagerOptions &opts)
    : opts_(opts),
      thread_(
          std::bind(std::mem_fn(&ActionStatusManager::threadTask), this, std::placeholders::_1)) {}

void ActionStatusManager::threadTask(vda5050pp::core::common::StopToken tkn) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    // This event loop is not needed
    return;
  }

  try {
    while (!tkn.stopRequested()) {
      if (this->action_status_queue_.processUntil([&tkn] { return tkn.stopRequested(); })) {
        // no pause
      } else {
        std::this_thread::sleep_for(10ms);
      }
    }
  } catch (const vda5050pp::VDA5050PPError &err) {
    getEventsLogger()->error(
        "ActionStatusManager caught an exception, while processing events:\n {}", err);
    // TODO: Handle critical exceptions
  }
}

void ActionStatusManager::dispatch(
    std::shared_ptr<vda5050pp::events::ActionStatusInitializing> data) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    this->action_status_queue_.dispatch(
        vda5050pp::events::ActionStatusType::k_action_status_initializing, data);
  } else {
    this->action_status_queue_.enqueue(
        vda5050pp::events::ActionStatusType::k_action_status_initializing, data);
  }
}

void ActionStatusManager::dispatch(
    std::shared_ptr<vda5050pp::events::ActionStatusWaiting> data) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    this->action_status_queue_.dispatch(
        vda5050pp::events::ActionStatusType::k_action_status_waiting, data);
  } else {
    this->action_status_queue_.enqueue(vda5050pp::events::ActionStatusType::k_action_status_waiting,
                                       data);
  }
}

void ActionStatusManager::dispatch(
    std::shared_ptr<vda5050pp::events::ActionStatusRunning> data) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    this->action_status_queue_.dispatch(
        vda5050pp::events::ActionStatusType::k_action_status_running, data);
  } else {
    this->action_status_queue_.enqueue(vda5050pp::events::ActionStatusType::k_action_status_running,
                                       data);
  }
}

void ActionStatusManager::dispatch(
    std::shared_ptr<vda5050pp::events::ActionStatusPaused> data) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    this->action_status_queue_.dispatch(vda5050pp::events::ActionStatusType::k_action_status_paused,
                                        data);
  } else {
    this->action_status_queue_.enqueue(vda5050pp::events::ActionStatusType::k_action_status_paused,
                                       data);
  }
}

void ActionStatusManager::dispatch(
    std::shared_ptr<vda5050pp::events::ActionStatusFinished> data) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    this->action_status_queue_.dispatch(
        vda5050pp::events::ActionStatusType::k_action_status_finished, data);
  } else {
    this->action_status_queue_.enqueue(
        vda5050pp::events::ActionStatusType::k_action_status_finished, data);
  }
}

void ActionStatusManager::dispatch(
    std::shared_ptr<vda5050pp::events::ActionStatusFailed> data) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    this->action_status_queue_.dispatch(vda5050pp::events::ActionStatusType::k_action_status_failed,
                                        data);
  } else {
    this->action_status_queue_.enqueue(vda5050pp::events::ActionStatusType::k_action_status_failed,
                                       data);
  }
}

ScopedActionStatusSubscriber ActionStatusManager::getScopedActionStatusSubscriber() noexcept(true) {
  return ScopedActionStatusSubscriber(this->action_status_queue_);
}