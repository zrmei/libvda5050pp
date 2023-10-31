//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/core/generic_event_manager.h"

#include <catch2/catch.hpp>

using namespace std::chrono_literals;

enum class MyEventType {
  k_my_event_1,
  k_my_event_2,
  k_my_event_3,
};

struct MyEvent : vda5050pp::events::Event<MyEventType> {
  std::string name;
};

struct MyEvent1 : vda5050pp::events::EventId<MyEvent, MyEventType::k_my_event_1> {
  int value;
};

struct MyEvent2 : vda5050pp::events::EventId<MyEvent, MyEventType::k_my_event_2> {
  bool value;
};

struct MyEvent3 : vda5050pp::events::EventId<MyEvent, MyEventType::k_my_event_3> {
  std::string value;
};

TEST_CASE("core::GenericEventManager - dispatch/subscribe", "[core][events]") {
  std::promise<std::shared_ptr<MyEvent1>> promise1;
  std::promise<std::shared_ptr<MyEvent2>> promise2;
  std::promise<std::shared_ptr<MyEvent3>> promise3;

  auto future1 = promise1.get_future();
  auto future2 = promise2.get_future();
  auto future3 = promise3.get_future();

  auto callback1 = [&promise1](std::shared_ptr<MyEvent1> evt) { promise1.set_value(evt); };
  auto callback2 = [&promise2](std::shared_ptr<MyEvent2> evt) { promise2.set_value(evt); };
  auto callback3 = [&promise3](std::shared_ptr<MyEvent3> evt) { promise3.set_value(evt); };

  vda5050pp::config::EventManagerOptions opts;
  vda5050pp::core::GenericEventManager<MyEvent> mgr(opts);

  {
    auto sub = mgr.getScopedSubscriber();
    sub.subscribe<MyEvent1>(callback1);
    sub.subscribe<MyEvent2>(callback2);
    sub.subscribe<MyEvent3>(callback3);

    WHEN("MyEvent1 is dispatched") {
      auto evt = std::make_shared<MyEvent1>();
      evt->name = "name";
      evt->value = 100;
      mgr.dispatch(evt);

      THEN("Only callback1 was called") {
        REQUIRE(future1.wait_for(100ms) == std::future_status::ready);
        auto res = future1.get();
        REQUIRE(res->name == evt->name);
        REQUIRE(res->value == evt->value);
        REQUIRE(future2.wait_for(100ms) == std::future_status::timeout);
        REQUIRE(future3.wait_for(100ms) == std::future_status::timeout);
      }
    }

    WHEN("MyEvent2 is dispatched") {
      auto evt = std::make_shared<MyEvent2>();
      evt->name = "name";
      evt->value = true;
      mgr.dispatch(evt);

      THEN("Only callback2 was called") {
        REQUIRE(future2.wait_for(100ms) == std::future_status::ready);
        auto res = future2.get();
        REQUIRE(res->name == evt->name);
        REQUIRE(res->value == evt->value);
        REQUIRE(future1.wait_for(100ms) == std::future_status::timeout);
        REQUIRE(future3.wait_for(100ms) == std::future_status::timeout);
      }
    }

    WHEN("MyEvent3 is dispatched") {
      auto evt = std::make_shared<MyEvent3>();
      evt->name = "name";
      evt->value = "value";
      mgr.dispatch(evt);

      THEN("Only callback3 was called") {
        REQUIRE(future3.wait_for(100ms) == std::future_status::ready);
        auto res = future3.get();
        REQUIRE(res->name == evt->name);
        REQUIRE(res->value == evt->value);
        REQUIRE(future1.wait_for(100ms) == std::future_status::timeout);
        REQUIRE(future2.wait_for(100ms) == std::future_status::timeout);
      }
    }
  }

  WHEN("The subscriber is out of scope and events are dispatched") {
    auto evt1 = std::make_shared<MyEvent1>();
    auto evt2 = std::make_shared<MyEvent2>();
    auto evt3 = std::make_shared<MyEvent3>();

    mgr.dispatch(evt1);
    mgr.dispatch(evt2);
    mgr.dispatch(evt3);

    THEN("No callback will be called") {
      REQUIRE(future1.wait_for(100ms) == std::future_status::timeout);
      REQUIRE(future2.wait_for(100ms) == std::future_status::timeout);
      REQUIRE(future3.wait_for(100ms) == std::future_status::timeout);
    }
  }
}