//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/core/action_event_manager.h"

#include <eventpp/utilities/argumentadapter.h>

#include <chrono>

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/common/formatters.h"
#include "vda5050++/core/logger.h"

using namespace vda5050pp::core;
using namespace std::chrono_literals;

ScopedActionEventSubscriber::ScopedActionEventSubscriber(ActionEventQueue &queue)
    : remover_(queue) {}

void ScopedActionEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::ActionList>)> &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::ActionEventType::k_action_list,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::ActionList>)>(
          std::move(callback)));
}

void ScopedActionEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::ActionValidate>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::ActionEventType::k_action_validate,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::ActionValidate>)>(
          std::move(callback)));
}

void ScopedActionEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::ActionPrepare>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::ActionEventType::k_action_prepare,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::ActionPrepare>)>(
          std::move(callback)));
}

void ScopedActionEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::ActionStart>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::ActionEventType::k_action_start,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::ActionStart>)>(
          std::move(callback)));
}

void ScopedActionEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::ActionPause>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::ActionEventType::k_action_pause,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::ActionPause>)>(
          std::move(callback)));
}

void ScopedActionEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::ActionResume>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::ActionEventType::k_action_resume,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::ActionResume>)>(
          std::move(callback)));
}

void ScopedActionEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::ActionCancel>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::ActionEventType::k_action_cancel,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::ActionCancel>)>(
          std::move(callback)));
}

void ScopedActionEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::ActionForget>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::ActionEventType::k_action_forget,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::ActionForget>)>(
          std::move(callback)));
}

ActionEventManager::ActionEventManager(const vda5050pp::config::EventManagerOptions &opts)
    : opts_(opts),
      thread_(
          std::bind(std::mem_fn(&ActionEventManager::threadTask), this, std::placeholders::_1)) {}

void ActionEventManager::threadTask(vda5050pp::core::common::StopToken tkn) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    // This thread loop is not needed
    return;
  }

  try {
    while (!tkn.stopRequested()) {
      if (this->action_event_queue_.processUntil([&tkn] { return tkn.stopRequested(); })) {
        // no pause
      } else {
        std::this_thread::sleep_for(10ms);
      }
    }
  } catch (const vda5050pp::VDA5050PPError &err) {
    getEventsLogger()->error(
        "ActionEventManager caught an exception, while processing events:\n {}", err);
    // TODO: Handle critical exceptions
  }
}

void ActionEventManager::dispatch(std::shared_ptr<vda5050pp::events::ActionList> data,
                                  bool synchronous) noexcept(false) {
  if (this->opts_.synchronous_event_dispatch || synchronous) {
    this->action_event_queue_.dispatch(vda5050pp::events::ActionEventType::k_action_list, data);
  } else {
    this->action_event_queue_.enqueue(vda5050pp::events::ActionEventType::k_action_list, data);
  }
}

void ActionEventManager::dispatch(std::shared_ptr<vda5050pp::events::ActionValidate> data,
                                  bool synchronous) noexcept(false) {
  if (this->opts_.synchronous_event_dispatch || synchronous) {
    this->action_event_queue_.dispatch(vda5050pp::events::ActionEventType::k_action_validate, data);
  } else {
    this->action_event_queue_.enqueue(vda5050pp::events::ActionEventType::k_action_validate, data);
  }
}

void ActionEventManager::dispatch(std::shared_ptr<vda5050pp::events::ActionPrepare> data,
                                  bool synchronous) noexcept(false) {
  if (this->opts_.synchronous_event_dispatch || synchronous) {
    this->action_event_queue_.dispatch(vda5050pp::events::ActionEventType::k_action_prepare, data);
  } else {
    this->action_event_queue_.enqueue(vda5050pp::events::ActionEventType::k_action_prepare, data);
  }
}

void ActionEventManager::dispatch(std::shared_ptr<vda5050pp::events::ActionStart> data,
                                  bool synchronous) noexcept(false) {
  if (this->opts_.synchronous_event_dispatch || synchronous) {
    this->action_event_queue_.dispatch(vda5050pp::events::ActionEventType::k_action_start, data);
  } else {
    this->action_event_queue_.enqueue(vda5050pp::events::ActionEventType::k_action_start, data);
  }
}

void ActionEventManager::dispatch(std::shared_ptr<vda5050pp::events::ActionPause> data,
                                  bool synchronous) noexcept(false) {
  if (this->opts_.synchronous_event_dispatch || synchronous) {
    this->action_event_queue_.dispatch(vda5050pp::events::ActionEventType::k_action_pause, data);
  } else {
    this->action_event_queue_.enqueue(vda5050pp::events::ActionEventType::k_action_pause, data);
  }
}

void ActionEventManager::dispatch(std::shared_ptr<vda5050pp::events::ActionResume> data,
                                  bool synchronous) noexcept(false) {
  if (this->opts_.synchronous_event_dispatch || synchronous) {
    this->action_event_queue_.dispatch(vda5050pp::events::ActionEventType::k_action_resume, data);
  } else {
    this->action_event_queue_.enqueue(vda5050pp::events::ActionEventType::k_action_resume, data);
  }
}

void ActionEventManager::dispatch(std::shared_ptr<vda5050pp::events::ActionCancel> data,
                                  bool synchronous) noexcept(false) {
  if (this->opts_.synchronous_event_dispatch || synchronous) {
    this->action_event_queue_.dispatch(vda5050pp::events::ActionEventType::k_action_cancel, data);
  } else {
    this->action_event_queue_.enqueue(vda5050pp::events::ActionEventType::k_action_cancel, data);
  }
}

void ActionEventManager::dispatch(std::shared_ptr<vda5050pp::events::ActionForget> data,
                                  bool synchronous) noexcept(false) {
  if (this->opts_.synchronous_event_dispatch || synchronous) {
    this->action_event_queue_.dispatch(vda5050pp::events::ActionEventType::k_action_forget, data);
  } else {
    this->action_event_queue_.enqueue(vda5050pp::events::ActionEventType::k_action_forget, data);
  }
}

ScopedActionEventSubscriber ActionEventManager::getScopedActionEventSubscriber() noexcept(true) {
  return ScopedActionEventSubscriber(this->action_event_queue_);
}