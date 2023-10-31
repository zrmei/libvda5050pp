//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/query_event_manager.h"

#include <eventpp/utilities/argumentadapter.h>

#include <chrono>

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/common/formatters.h"
#include "vda5050++/core/logger.h"

using namespace vda5050pp::core;
using namespace std::chrono_literals;

ScopedQueryEventSubscriber::ScopedQueryEventSubscriber(QueryEventQueue &queue) : remover_(queue) {}

void ScopedQueryEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::QueryPauseable>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::QueryEventType::k_pauseable,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::QueryPauseable>)>(
          std::move(callback)));
}

void ScopedQueryEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::QueryResumable>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::QueryEventType::k_resumable,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::QueryResumable>)>(
          std::move(callback)));
}

void ScopedQueryEventSubscriber::subscribe(
    std::function<void(std::shared_ptr<vda5050pp::events::QueryAcceptZoneSet>)>
        &&callback) noexcept(true) {
  this->remover_.appendListener(
      vda5050pp::events::QueryEventType::k_accept_zone_set,
      eventpp::argumentAdapter<void(std::shared_ptr<vda5050pp::events::QueryAcceptZoneSet>)>(
          std::move(callback)));
}

QueryEventManager::QueryEventManager(const vda5050pp::config::EventManagerOptions &opts)
    : opts_(opts),
      thread_(std::bind(std::mem_fn(&QueryEventManager::threadTask), this, std::placeholders::_1)) {
}

void QueryEventManager::threadTask(vda5050pp::core::common::StopToken tkn) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch) {
    // This event loop is not needed
    return;
  }

  try {
    while (!tkn.stopRequested()) {
      if (this->query_event_queue_.processUntil([&tkn] { return tkn.stopRequested(); })) {
        // no pause
      } else {
        std::this_thread::sleep_for(10ms);
      }
    }
  } catch (const vda5050pp::VDA5050PPError &err) {
    getEventsLogger()->error("QueryEventManager caught an exception, while processing events:\n {}",
                             err);
    // TODO: Handle critical exceptions
  }
}

void QueryEventManager::dispatch(std::shared_ptr<vda5050pp::events::QueryPauseable> data,
                                 bool synchronous) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch || synchronous) {
    this->query_event_queue_.dispatch(vda5050pp::events::QueryEventType::k_pauseable, data);
  } else {
    this->query_event_queue_.enqueue(vda5050pp::events::QueryEventType::k_pauseable, data);
  }
}

void QueryEventManager::dispatch(std::shared_ptr<vda5050pp::events::QueryResumable> data,
                                 bool synchronous) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch || synchronous) {
    this->query_event_queue_.dispatch(vda5050pp::events::QueryEventType::k_resumable, data);
  } else {
    this->query_event_queue_.enqueue(vda5050pp::events::QueryEventType::k_resumable, data);
  }
}

void QueryEventManager::dispatch(std::shared_ptr<vda5050pp::events::QueryAcceptZoneSet> data,
                                 bool synchronous) noexcept(true) {
  if (this->opts_.synchronous_event_dispatch || synchronous) {
    this->query_event_queue_.dispatch(vda5050pp::events::QueryEventType::k_accept_zone_set, data);
  } else {
    this->query_event_queue_.enqueue(vda5050pp::events::QueryEventType::k_accept_zone_set, data);
  }
}

ScopedQueryEventSubscriber QueryEventManager::getScopedQueryEventSubscriber() noexcept(true) {
  return ScopedQueryEventSubscriber(this->query_event_queue_);
}