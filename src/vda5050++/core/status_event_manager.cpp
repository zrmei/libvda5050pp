//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/status_event_manager.h"

#include <eventpp/utilities/argumentadapter.h>

#include <chrono>
#include <functional>

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/common/formatters.h"
#include "vda5050++/core/logger.h"

using namespace vda5050pp::core;
using namespace std::chrono_literals;

ScopedStatusEventSubscriber::ScopedStatusEventSubscriber(StatusEventQueue &queue)
    : remover_(queue) {}

void ScopedStatusEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::LoadAdd>)> &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::StatusEventType::k_load_add,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::LoadAdd>)>(
          std::move(callback)));
}
void ScopedStatusEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::LoadRemove>)> &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::StatusEventType::k_load_remove,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::LoadRemove>)>(
          std::move(callback)));
}
void ScopedStatusEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::LoadsGet>)> &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::StatusEventType::k_loads_get,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::LoadsGet>)>(
          std::move(callback)));
}
void ScopedStatusEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::LoadsAlter>)> &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::StatusEventType::k_loads_alter,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::LoadsAlter>)>(
          std::move(callback)));
}
void ScopedStatusEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::OperatingModeSet>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::StatusEventType::k_operating_mode_set,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::OperatingModeSet>)>(
          std::move(callback)));
}
void ScopedStatusEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::OperatingModeGet>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::StatusEventType::k_operating_mode_get,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::OperatingModeGet>)>(
          std::move(callback)));
}
void ScopedStatusEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::OperatingModeAlter>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::StatusEventType::k_operating_mode_alter,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::OperatingModeAlter>)>(
          std::move(callback)));
}
void ScopedStatusEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::BatteryStateSet>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::StatusEventType::k_battery_state_set,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::BatteryStateSet>)>(
          std::move(callback)));
}
void ScopedStatusEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::BatteryStateGet>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::StatusEventType::k_battery_state_get,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::BatteryStateGet>)>(
          std::move(callback)));
}
void ScopedStatusEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::BatteryStateAlter>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::StatusEventType::k_battery_state_alter,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::BatteryStateAlter>)>(
          std::move(callback)));
}
void ScopedStatusEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::RequestNewBase>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::StatusEventType::k_request_new_base,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::RequestNewBase>)>(
          std::move(callback)));
}
void ScopedStatusEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::ErrorAdd>)> &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::StatusEventType::k_error_add,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::ErrorAdd>)>(
          std::move(callback)));
}
void ScopedStatusEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::ErrorsAlter>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::StatusEventType::k_errors_alter,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::ErrorsAlter>)>(
          std::move(callback)));
}
void ScopedStatusEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::InfoAdd>)> &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::StatusEventType::k_info_add,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::InfoAdd>)>(
          std::move(callback)));
}
void ScopedStatusEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::InfosAlter>)> &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::StatusEventType::k_infos_alter,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::InfosAlter>)>(
          std::move(callback)));
}

void StatusEventManager::threadTask(vda5050pp::core::common::StopToken tkn) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    // This processing loop is not needed
    return;
  }

  try {
    while (!tkn.stopRequested()) {
      if (this->status_event_queue_.processUntil([&tkn] { return tkn.stopRequested(); })) {
        // no pause
      } else {
        std::this_thread::sleep_for(10ms);
      }
    }
  } catch (const vda5050pp::VDA5050PPError &err) {
    getEventsLogger()->error(
        "StatusEventManager caught an exception, while processing events:\n {}", err);
    // TODO: Handle critical exceptions
  }
}

StatusEventManager::StatusEventManager(const vda5050pp::config::EventManagerOptions &opts)
    : opts_(opts),
      thread_(
          std::bind(std::mem_fn(&StatusEventManager::threadTask), this, std::placeholders::_1)) {}

void StatusEventManager::dispatch(std::shared_ptr<vda5050pp::events::StatusEvent> data,
                                  bool synchronous) noexcept(false) {
  if (data == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("event is nullptr"));
  }
  if (data->type == vda5050pp::events::StatusEventType::k_none) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("event has no specified type"));
  }

  if (this->opts_.synchronous_event_dispatch || synchronous) {
    this->status_event_queue_.dispatch(data->type, data);
  } else {
    this->status_event_queue_.enqueue(data->type, data);
  }
}

ScopedStatusEventSubscriber StatusEventManager::getScopedStatusEventSubscriber() noexcept(true) {
  return ScopedStatusEventSubscriber(this->status_event_queue_);
}