// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include <vda5050++/core/instance.h>
#include <vda5050++/observer/order_observer.h>

#include <catch2/catch_all.hpp>

TEST_CASE("observer::OrderObserver - event handling", "[observer]") {
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().useWhiteList();
  cfg.refGlobalConfig().refEventManagerOptions().synchronous_event_dispatch = true;
  vda5050pp::core::Instance::reset();
  auto instance = vda5050pp::core::Instance::init(cfg).lock();

  vda5050pp::observer::OrderObserver observer;

  WHEN("No events were received yet") {
    THEN("The last node ID is (\"\", 0)") { REQUIRE_FALSE(observer.getLastNode().has_value()); }

    THEN("The order status is k_order_idle") {
      REQUIRE_FALSE(observer.getOrderStatus().has_value());
    }

    THEN("The actions are not known") {
      REQUIRE_FALSE(observer.getActionStatus("action1").has_value());
      REQUIRE_FALSE(observer.getActionStatus("action2").has_value());
      REQUIRE_FALSE(observer.getActionStatus("").has_value());
    }
  }

  WHEN("Receiving a NewLastNode Event") {
    auto evt = std::make_shared<vda5050pp::core::events::OrderNewLastNodeId>();
    evt->last_node_id = "node1";
    evt->seq_id = 1;

    std::string cb_id;
    decltype(vda5050::Node::sequenceId) cb_seq = 0;

    observer.onLastNodeIdChanged([&cb_id, &cb_seq](auto pair) {
      const auto &[id, seq] = pair;
      cb_id = id;
      cb_seq = seq;
    });

    instance->getOrderEventManager().dispatch(evt);

    THEN("The last node ID is (\"node1\", 1)") {
      auto ret = observer.getLastNode();
      REQUIRE(ret.has_value());
      const auto &[id, seq] = ret.value();
      REQUIRE(id == "node1");
      REQUIRE(seq == 1);
    }

    THEN("The callback was called") {
      REQUIRE(cb_id == "node1");
      REQUIRE(cb_seq == 1);
    }
  }

  WHEN("Receiving an OrderStatus Event") {
    auto evt = std::make_shared<vda5050pp::core::events::OrderStatus>();
    evt->status = vda5050pp::misc::OrderStatus::k_order_active;

    vda5050pp::misc::OrderStatus cb_status = vda5050pp::misc::OrderStatus::k_order_idle;

    observer.onOrderStatusChanged([&cb_status](auto status) { cb_status = status; });

    instance->getOrderEventManager().dispatch(evt);

    THEN("The order status is correct") {
      REQUIRE(observer.getOrderStatus() == vda5050pp::misc::OrderStatus::k_order_active);
    }

    THEN("The callback was called") {
      REQUIRE(cb_status == vda5050pp::misc::OrderStatus::k_order_active);
    }
  }

  WHEN("Receiving an ActionStatus Event") {
    auto evt = std::make_shared<vda5050pp::core::events::OrderActionStatusChanged>();
    evt->action_id = "action1";
    evt->action_status = vda5050::ActionStatus::RUNNING;

    vda5050::ActionStatus cb_status = vda5050::ActionStatus::WAITING;
    bool cb2_called = false;

    observer.onActionStatusChanged("action1", [&cb_status](auto status) { cb_status = status; });
    observer.onActionStatusChanged("action2", [&cb2_called](auto) { cb2_called = true; });

    instance->getOrderEventManager().dispatch(evt);

    THEN("The action status is correct") {
      REQUIRE(observer.getActionStatus("action1") == vda5050::ActionStatus::RUNNING);
    }

    THEN("The callback was called") { REQUIRE(cb_status == vda5050::ActionStatus::RUNNING); }

    THEN("Then other action callbacks were not called") { REQUIRE_FALSE(cb2_called); }
  }
}