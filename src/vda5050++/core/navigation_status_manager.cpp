//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/navigation_status_manager.h"

#include <eventpp/utilities/argumentadapter.h>

#include <chrono>

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/common/formatters.h"
#include "vda5050++/core/logger.h"

using namespace vda5050pp::core;
using namespace std::chrono_literals;

ScopedNavigationStatusSubscriber::ScopedNavigationStatusSubscriber(NavigationStatusQueue &queue)
    : remover_(queue) {}

void ScopedNavigationStatusSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::NavigationStatusPosition>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::NavigationStatusType::k_position,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::NavigationStatusPosition>)>(
          std::move(callback)));
}

void ScopedNavigationStatusSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::NavigationStatusVelocity>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::NavigationStatusType::k_velocity,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::NavigationStatusVelocity>)>(
          std::move(callback)));
}

void ScopedNavigationStatusSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::NavigationStatusNodeReached>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::NavigationStatusType::k_node_reached,
      eventpp::argumentAdapter<void(
          std::shared_ptr<vda5050pp::events::NavigationStatusNodeReached>)>(std::move(callback)));
}

void ScopedNavigationStatusSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::NavigationStatusDistanceSinceLastNode>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::NavigationStatusType::k_distance_since_last_node,
      eventpp::argumentAdapter<void(
          std::shared_ptr<vda5050pp::events::NavigationStatusDistanceSinceLastNode>)>(
          std::move(callback)));
}

void ScopedNavigationStatusSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::NavigationStatusDriving>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::NavigationStatusType::k_driving,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::NavigationStatusDriving>)>(
          std::move(callback)));
}

void ScopedNavigationStatusSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::NavigationStatusControl>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::NavigationStatusType::k_navigation_control_status,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::NavigationStatusControl>)>(
          std::move(callback)));
}

NavigationStatusManager::NavigationStatusManager(const vda5050pp::config::EventManagerOptions &opts)
    : opts_(opts),
      thread_(std::bind(std::mem_fn(&NavigationStatusManager::threadTask), this,
                        std::placeholders::_1)) {}

void NavigationStatusManager::threadTask(vda5050pp::core::common::StopToken tkn) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    // This event loop is not needed
    return;
  }

  try {
    while (!tkn.stopRequested()) {
      if (this->navigation_status_queue_.processUntil([&tkn] { return tkn.stopRequested(); })) {
        // no pause
      } else {
        std::this_thread::sleep_for(10ms);
      }
    }
  } catch (const vda5050pp::VDA5050PPError &err) {
    getEventsLogger()->error(
        "NavigationStatusManager caught an exception, while processing events:\n {}", err);
    // TODO: Handle critical exceptions
  }
}

void NavigationStatusManager::dispatch(
    std::shared_ptr<vda5050pp::events::NavigationStatusPosition> data) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    this->navigation_status_queue_.dispatch(vda5050pp::events::NavigationStatusType::k_position,
                                            data);
  } else {
    this->navigation_status_queue_.enqueue(vda5050pp::events::NavigationStatusType::k_position,
                                           data);
  }
}

void NavigationStatusManager::dispatch(
    std::shared_ptr<vda5050pp::events::NavigationStatusVelocity> data) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    this->navigation_status_queue_.dispatch(vda5050pp::events::NavigationStatusType::k_velocity,
                                            data);
  } else {
    this->navigation_status_queue_.enqueue(vda5050pp::events::NavigationStatusType::k_velocity,
                                           data);
  }
}

void NavigationStatusManager::dispatch(
    std::shared_ptr<vda5050pp::events::NavigationStatusNodeReached> data) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    this->navigation_status_queue_.dispatch(vda5050pp::events::NavigationStatusType::k_node_reached,
                                            data);
  } else {
    this->navigation_status_queue_.enqueue(vda5050pp::events::NavigationStatusType::k_node_reached,
                                           data);
  }
}

void NavigationStatusManager::dispatch(
    std::shared_ptr<vda5050pp::events::NavigationStatusDistanceSinceLastNode> data) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    this->navigation_status_queue_.dispatch(
        vda5050pp::events::NavigationStatusType::k_distance_since_last_node, data);
  } else {
    this->navigation_status_queue_.enqueue(
        vda5050pp::events::NavigationStatusType::k_distance_since_last_node, data);
  }
}

void NavigationStatusManager::dispatch(
    std::shared_ptr<vda5050pp::events::NavigationStatusDriving> data) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    this->navigation_status_queue_.dispatch(vda5050pp::events::NavigationStatusType::k_driving,
                                            data);
  } else {
    this->navigation_status_queue_.enqueue(vda5050pp::events::NavigationStatusType::k_driving,
                                           data);
  }
}

void NavigationStatusManager::dispatch(
    std::shared_ptr<vda5050pp::events::NavigationStatusControl> data) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    this->navigation_status_queue_.dispatch(
        vda5050pp::events::NavigationStatusType::k_navigation_control_status, data);
  } else {
    this->navigation_status_queue_.enqueue(
        vda5050pp::events::NavigationStatusType::k_navigation_control_status, data);
  }
}

ScopedNavigationStatusSubscriber
NavigationStatusManager::getScopedNavigationStatusSubscriber() noexcept(true) {
  return ScopedNavigationStatusSubscriber(this->navigation_status_queue_);
}
