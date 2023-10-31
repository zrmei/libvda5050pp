//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/navigation_event_manager.h"

#include <eventpp/utilities/argumentadapter.h>

#include <chrono>

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/common/formatters.h"
#include "vda5050++/core/logger.h"

using namespace vda5050pp::core;
using namespace std::chrono_literals;

ScopedNavigationEventSubscriber::ScopedNavigationEventSubscriber(NavigationEventQueue &queue)
    : remover_(queue) {}

void ScopedNavigationEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::NavigationHorizonUpdate>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::NavigationEventType::k_horizon_update,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::NavigationHorizonUpdate>)>(
          std::move(callback)));
}

void ScopedNavigationEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::NavigationBaseIncreased>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::NavigationEventType::k_base_increased,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::NavigationBaseIncreased>)>(
          std::move(callback)));
}

void ScopedNavigationEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::NavigationNextNode>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::NavigationEventType::k_next_node,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::NavigationNextNode>)>(
          std::move(callback)));
}

void ScopedNavigationEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::NavigationUpcomingSegment>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::NavigationEventType::k_upcoming_segment,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::NavigationUpcomingSegment>)>(
          std::move(callback)));
}

void ScopedNavigationEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::NavigationControl>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::NavigationEventType::k_control,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::NavigationControl>)>(
          std::move(callback)));
}

NavigationEventManager::NavigationEventManager(const vda5050pp::config::EventManagerOptions &opts)
    : opts_(opts),
      thread_(std::bind(std::mem_fn(&NavigationEventManager::threadTask), this,
                        std::placeholders::_1)) {}

void NavigationEventManager::threadTask(vda5050pp::core::common::StopToken tkn) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    // This event loop is not needed
    return;
  }

  try {
    while (!tkn.stopRequested()) {
      if (this->navigation_event_queue_.processUntil([&tkn] { return tkn.stopRequested(); })) {
        // no pause
      } else {
        std::this_thread::sleep_for(10ms);
      }
    }
  } catch (const vda5050pp::VDA5050PPError &err) {
    getEventsLogger()->error(
        "NavigationEventManager caught an exception, while processing events:\n {}", err);
    // TODO: Handle critical exceptions
  }
}

void NavigationEventManager::dispatch(
    std::shared_ptr<vda5050pp::events::NavigationHorizonUpdate> data) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    this->navigation_event_queue_.dispatch(vda5050pp::events::NavigationEventType::k_horizon_update,
                                           data);
  } else {
    this->navigation_event_queue_.enqueue(vda5050pp::events::NavigationEventType::k_horizon_update,
                                          data);
  }
}

void NavigationEventManager::dispatch(
    std::shared_ptr<vda5050pp::events::NavigationBaseIncreased> data) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    this->navigation_event_queue_.dispatch(vda5050pp::events::NavigationEventType::k_base_increased,
                                           data);
  } else {
    this->navigation_event_queue_.enqueue(vda5050pp::events::NavigationEventType::k_base_increased,
                                          data);
  }
}

void NavigationEventManager::dispatch(
    std::shared_ptr<vda5050pp::events::NavigationNextNode> data) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    this->navigation_event_queue_.dispatch(vda5050pp::events::NavigationEventType::k_next_node,
                                           data);
  } else {
    this->navigation_event_queue_.enqueue(vda5050pp::events::NavigationEventType::k_next_node,
                                          data);
  }
}

void NavigationEventManager::dispatch(
    std::shared_ptr<vda5050pp::events::NavigationUpcomingSegment> data) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    this->navigation_event_queue_.dispatch(
        vda5050pp::events::NavigationEventType::k_upcoming_segment, data);
  } else {
    this->navigation_event_queue_.enqueue(
        vda5050pp::events::NavigationEventType::k_upcoming_segment, data);
  }
}

void NavigationEventManager::dispatch(
    std::shared_ptr<vda5050pp::events::NavigationControl> data) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    this->navigation_event_queue_.dispatch(vda5050pp::events::NavigationEventType::k_control, data);
  } else {
    this->navigation_event_queue_.enqueue(vda5050pp::events::NavigationEventType::k_control, data);
  }
}

ScopedNavigationEventSubscriber
NavigationEventManager::getScopedNavigationEventSubscriber() noexcept(true) {
  return ScopedNavigationEventSubscriber(this->navigation_event_queue_);
}