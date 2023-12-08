// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/sinks/navigation_sink.h"

#include <spdlog/fmt/fmt.h>

#include <catch2/catch.hpp>

#include "vda5050++/core/instance.h"

template <typename Event> inline void testEvent(const std::function<void()> &call_sink) {
  SECTION(fmt::format("Sink dispatch {}", typeid(Event).name())) {
    auto sub = vda5050pp::core::Instance::ref()
                   .getNavigationStatusManager()
                   .getScopedNavigationStatusSubscriber();
    auto event = std::make_shared<Event>();
    bool called = false;
    sub.subscribe([event, &called](std::shared_ptr<Event>) { called = true; });
    call_sink();
    REQUIRE(called);
  }
}

template <typename Event> inline void testSyncEvent(const std::function<void()> &call_sink) {
  SECTION(fmt::format("Sink dispatch {}", typeid(Event).name())) {
    auto sub = vda5050pp::core::Instance::ref()
                   .getNavigationEventManager()
                   .getScopedNavigationEventSubscriber();
    auto event = std::make_shared<Event>();
    bool called = false;
    sub.subscribe([event, &called](std::shared_ptr<Event> evt) {
      auto tkn = evt->acquireResultToken();
      tkn.setValue((typename Event::result_type){});
      called = true;
    });
    call_sink();
    REQUIRE(called);
  }
}

TEST_CASE("sinks::NavigationSink", "[events][sinks]") {
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().useWhiteList();
  cfg.refGlobalConfig().refEventManagerOptions().synchronous_event_dispatch = true;
  vda5050pp::core::Instance::reset();
  auto instance = vda5050pp::core::Instance::init(cfg).lock();

  vda5050pp::sinks::NavigationSink sink;

  testEvent<vda5050pp::events::NavigationStatusControl>([sink] {
    sink.setNavigationStatus(vda5050pp::events::NavigationStatusControlType::k_failed);
  });
  testEvent<vda5050pp::events::NavigationStatusDistanceSinceLastNode>(
      [sink] { sink.setDistanceSinceLastNode(0); });
  testEvent<vda5050pp::events::NavigationStatusDriving>([sink] { sink.setDriving(true); });
  testEvent<vda5050pp::events::NavigationStatusNodeReached>([sink] { sink.setNodeReached(0); });
  testEvent<vda5050pp::events::NavigationStatusPosition>(
      [sink] { sink.setPosition(vda5050::AGVPosition{}); });
  testEvent<vda5050pp::events::NavigationStatusVelocity>(
      [sink] { sink.setVelocity(vda5050::Velocity{}); });
}