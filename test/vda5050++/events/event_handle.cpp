//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/events/event_handle.h"

#include <fmt/format.h>

#include <catch2/catch.hpp>

#include "vda5050++/core/instance.h"
#include "vda5050++/exception.h"

TEST_CASE("EventHandle - instance access", "[events][interface]") {
  vda5050pp::core::Instance::reset();
  WHEN("The instance was not initialized yet") {
    vda5050pp::events::EventHandle event_handle;

    THEN("An EventHandle is not valid") { REQUIRE_FALSE(event_handle.isValid()); }

    THEN("EventHandle get subscriber throws an exception") {
      REQUIRE_THROWS_AS(event_handle.getScopedActionEventSubscriber(),
                        vda5050pp::VDA5050PPNotInitialized);
      REQUIRE_THROWS_AS(event_handle.getScopedNavigationEventSubscriber(),
                        vda5050pp::VDA5050PPNotInitialized);
      REQUIRE_THROWS_AS(event_handle.getScopedQueryEventSubscriber(),
                        vda5050pp::VDA5050PPNotInitialized);
    }
  }

  WHEN("The instance was initialized") {
    vda5050pp::core::Instance::reset();
    vda5050pp::Config cfg;
    cfg.refGlobalConfig().useWhiteList();
    vda5050pp::core::Instance::init(cfg);
    vda5050pp::events::EventHandle event_handle;

    THEN("An EventHandle is valid") { REQUIRE(event_handle.isValid()); }

    THEN("EventHandle get subscriber throws no exception") {
      REQUIRE_NOTHROW(event_handle.getScopedActionEventSubscriber());
      REQUIRE_NOTHROW(event_handle.getScopedNavigationEventSubscriber());
      REQUIRE_NOTHROW(event_handle.getScopedQueryEventSubscriber());
    }
  }
}

template <typename EventT> inline void testActionEventForwarding() {
  vda5050pp::events::EventHandle handle;

  auto event = std::make_shared<EventT>();

  WHEN(fmt::format("A {} event is dispatched", typeid(EventT).name())) {
    auto sub =
        vda5050pp::core::Instance::ref().getActionStatusManager().getScopedActionStatusSubscriber();
    std::shared_ptr<EventT> received;
    sub.subscribe([&received](std::shared_ptr<EventT> evt) { received = evt; });

    handle.dispatch(event);

    THEN("It was forwarded") { REQUIRE(received == event); }
  }
}

template <typename EventT> inline void testNavigationEventForwarding() {
  vda5050pp::events::EventHandle handle;

  auto event = std::make_shared<EventT>();

  WHEN(fmt::format("A {} event is dispatched", typeid(EventT).name())) {
    auto sub = vda5050pp::core::Instance::ref()
                   .getNavigationStatusManager()
                   .getScopedNavigationStatusSubscriber();
    std::shared_ptr<EventT> received;
    sub.subscribe([&received](std::shared_ptr<EventT> evt) { received = evt; });

    handle.dispatch(event);

    THEN("It was forwarded") { REQUIRE(received == event); }
  }
}

template <typename EventT> inline void testStatusEventForwarding() {
  vda5050pp::events::EventHandle handle;

  auto event = std::make_shared<EventT>();

  WHEN(fmt::format("A {} event is dispatched", typeid(EventT).name())) {
    auto sub =
        vda5050pp::core::Instance::ref().getStatusEventManager().getScopedStatusEventSubscriber();
    std::shared_ptr<EventT> received;
    sub.subscribe([&received](std::shared_ptr<EventT> evt) { received = evt; });

    handle.dispatch(event);

    THEN("It was forwarded") { REQUIRE(received == event); }
  }
}

TEST_CASE("EventHandle - dispatching", "[events][interface]") {
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().refEventManagerOptions().synchronous_event_dispatch = true;
  cfg.refGlobalConfig().useWhiteList();
  vda5050pp::core::Instance::reset();
  auto instance = vda5050pp::core::Instance::init(cfg).lock();

  testActionEventForwarding<vda5050pp::events::ActionStatusFailed>();
  testActionEventForwarding<vda5050pp::events::ActionStatusFinished>();
  testActionEventForwarding<vda5050pp::events::ActionStatusInitializing>();
  testActionEventForwarding<vda5050pp::events::ActionStatusPaused>();
  testActionEventForwarding<vda5050pp::events::ActionStatusRunning>();
  testActionEventForwarding<vda5050pp::events::ActionStatusWaiting>();
  testNavigationEventForwarding<vda5050pp::events::NavigationStatusControl>();
  testNavigationEventForwarding<vda5050pp::events::NavigationStatusDistanceSinceLastNode>();
  testNavigationEventForwarding<vda5050pp::events::NavigationStatusDriving>();
  testNavigationEventForwarding<vda5050pp::events::NavigationStatusNodeReached>();
  testNavigationEventForwarding<vda5050pp::events::NavigationStatusPosition>();
  testNavigationEventForwarding<vda5050pp::events::NavigationStatusVelocity>();
  testStatusEventForwarding<vda5050pp::events::LoadAdd>();
  testStatusEventForwarding<vda5050pp::events::LoadRemove>();
  testStatusEventForwarding<vda5050pp::events::LoadsAlter>();
  testStatusEventForwarding<vda5050pp::events::LoadsGet>();
  testStatusEventForwarding<vda5050pp::events::OperatingModeSet>();
  testStatusEventForwarding<vda5050pp::events::OperatingModeGet>();
  testStatusEventForwarding<vda5050pp::events::OperatingModeAlter>();
  testStatusEventForwarding<vda5050pp::events::BatteryStateGet>();
  testStatusEventForwarding<vda5050pp::events::BatteryStateSet>();
  testStatusEventForwarding<vda5050pp::events::BatteryStateAlter>();
  testStatusEventForwarding<vda5050pp::events::ErrorAdd>();
  testStatusEventForwarding<vda5050pp::events::ErrorsAlter>();
  testStatusEventForwarding<vda5050pp::events::InfoAdd>();
  testStatusEventForwarding<vda5050pp::events::InfosAlter>();
}