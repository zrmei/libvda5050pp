// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include <spdlog/fmt/fmt.h>

#include <catch2/catch.hpp>

#include "test/data.h"
#include "vda5050++/core/common/type_traits.h"
#include "vda5050++/core/instance.h"
#include "vda5050++/sinks/status_sink.h"

using namespace std::chrono_literals;

template <typename EventT, typename CheckT>
inline void quickStateEventTest(EventT &&evt, CheckT &&check_fn,
                                bool requires_state_update = false) {
  static_assert(vda5050pp::core::common::is_signature_v<CheckT, void(const vda5050::State &)>,
                "CheckT must be a function with signature void(const vda5050::State &)");
  WHEN(fmt::format("Dispatching a {}", typeid(EventT).name())) {
    auto &instance = vda5050pp::core::Instance::ref();

    bool called = false;
    auto sub = instance.getStateEventManager().getScopedSubscriber();
    sub.subscribe<vda5050pp::core::events::RequestStateUpdateEvent>(
        [&called](auto) { called = true; });

    instance.getStatusEventManager().dispatch(std::forward<EventT>(evt));

    THEN("The status manager contains the data") {
      vda5050::State state;
      instance.getStatusManager().dumpTo(state);
      check_fn(state);
    }

    if (requires_state_update) {
      THEN("A RequestStateUpdate event was dispatched") { REQUIRE(called); }
    }
  }
}

TEST_CASE("core::state::StateEventHandler", "[core][state]") {
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().useWhiteList();
  cfg.refGlobalConfig().bwListModule(vda5050pp::core::module_keys::k_state_event_handler_key);
  cfg.refGlobalConfig().refEventManagerOptions().synchronous_event_dispatch = true;
  vda5050pp::core::Instance::reset();
  auto instance = vda5050pp::core::Instance::init(cfg).lock();
  using namespace test::data;

  WHEN("The order manager contains an order") {
    instance->getOrderManager().replaceGraph(
        vda5050pp::core::state::Graph({
            vda5050pp::core::state::GraphElement(wrap_shared(mkNode("n0", 0, true, {}))),
            vda5050pp::core::state::GraphElement(wrap_shared(mkEdge("e1", 1, false, {}))),
            vda5050pp::core::state::GraphElement(wrap_shared(mkNode("n2", 2, false, {}))),
        }),
        "order");

    WHEN("A YieldGraphExtension event is dispatched") {
      auto sub = instance->getNavigationEventManager().getScopedNavigationEventSubscriber();
      std::promise<std::pair<std::list<std::shared_ptr<const vda5050::Node>>,
                             std::list<std::shared_ptr<const vda5050::Edge>>>>
          promise;
      auto future = promise.get_future();
      sub.subscribe([&promise](std::shared_ptr<vda5050pp::events::NavigationBaseIncreased> evt) {
        promise.set_value(std::make_pair(evt->base_expand_nodes, evt->base_expand_edges));
      });

      auto evt = std::make_shared<vda5050pp::core::events::YieldGraphExtension>();
      evt->graph = wrap_shared(vda5050pp::core::state::Graph({
          vda5050pp::core::state::GraphElement(wrap_shared(mkNode("n0", 0, true, {}))),
          vda5050pp::core::state::GraphElement(wrap_shared(mkEdge("e1", 1, true, {}))),
          vda5050pp::core::state::GraphElement(wrap_shared(mkNode("n2", 2, true, {}))),
      }));
      evt->order_update_id = 1;

      instance->getInterpreterEventManager().dispatch(evt);

      THEN("A corresponding base increased event was dispatched") {
        REQUIRE(future.wait_for(0s) == std::future_status::ready);
        auto [nodes, edges] = future.get();
        REQUIRE(nodes.size() == 1);
        REQUIRE(edges.size() == 1);
      }
    }

    WHEN("A YieldGraphReplacement event is dispatched") {
      auto sub = instance->getNavigationEventManager().getScopedNavigationEventSubscriber();
      std::promise<std::pair<std::list<std::shared_ptr<const vda5050::Node>>,
                             std::list<std::shared_ptr<const vda5050::Edge>>>>
          promise;
      auto future = promise.get_future();
      sub.subscribe([&promise](std::shared_ptr<vda5050pp::events::NavigationBaseIncreased> evt) {
        promise.set_value(std::make_pair(evt->base_expand_nodes, evt->base_expand_edges));
      });

      auto evt = std::make_shared<vda5050pp::core::events::YieldGraphReplacement>();
      evt->graph = wrap_shared(vda5050pp::core::state::Graph({
          vda5050pp::core::state::GraphElement(wrap_shared(mkNode("n0", 0, true, {}))),
          vda5050pp::core::state::GraphElement(wrap_shared(mkEdge("e1", 1, true, {}))),
          vda5050pp::core::state::GraphElement(wrap_shared(mkNode("n2", 2, true, {}))),
      }));
      evt->order_id = "order2";

      instance->getInterpreterEventManager().dispatch(evt);

      THEN("A corresponding base increased event was dispatched") {
        REQUIRE(future.wait_for(0s) == std::future_status::ready);
        auto [nodes, edges] = future.get();
        REQUIRE(nodes.size() == 2);
        REQUIRE(edges.size() == 1);
      }

      WHEN("A NewLastNodeIdEvent is dispatched") {
        auto evt_nln = std::make_shared<vda5050pp::core::events::OrderNewLastNodeId>();
        evt_nln->last_node_id = "n2";
        evt_nln->seq_id = 2;
        instance->getOrderEventManager().dispatch(evt_nln);
        THEN("The OrderManager has the last_node_id") {
          vda5050::State state;
          instance->getOrderManager().dumpTo(state);
          REQUIRE(state.lastNodeId == "n2");
        }
      }
    }
  }

  WHEN("A YieldNewActionEvent is dispatched") {
    auto evt = std::make_shared<vda5050pp::core::events::YieldNewAction>();
    evt->action = std::make_shared<vda5050::Action>();
    evt->action->actionId = "action1";

    instance->getInterpreterEventManager().dispatch(evt);

    THEN("The action is added to the order manager") {
      REQUIRE_NOTHROW(instance->getOrderManager().getAction("action1"));
      REQUIRE_NOTHROW(instance->getOrderManager().getActionState("action1"));
    }
  }

  WHEN("A ClearActionEvent is dispatched") {
    auto evt_clear = std::make_shared<vda5050pp::core::events::YieldClearActions>();
    instance->getInterpreterEventManager().dispatch(evt_clear);
    THEN("The OrderManage has no actions") {
      REQUIRE_THROWS_AS(instance->getOrderManager().getAction("action1"),
                        vda5050pp::VDA5050PPInvalidArgument);
    }
  }

  WHEN("A OrderActionStatusChanged event is dispatched") {
    auto &om = instance->getOrderManager();
    om.addNewAction(wrap_shared(mkAction("a1", "t1", vda5050::BlockingType::NONE)));

    auto sub = instance->getStateEventManager().getScopedSubscriber();
    bool called = false;
    sub.subscribe<vda5050pp::core::events::RequestStateUpdateEvent>(
        [&called](auto) { called = true; });

    auto evt = std::make_shared<vda5050pp::core::events::OrderActionStatusChanged>();
    evt->action_id = "a1";
    evt->action_status = vda5050::ActionStatus::FINISHED;

    instance->getOrderEventManager().dispatch(evt);

    THEN("The action status is updated") {
      REQUIRE(om.getActionState("a1")->actionStatus == vda5050::ActionStatus::FINISHED);
    }
    THEN("A RequestStateUpdate event was dispatched") { REQUIRE(called); }
  }

  WHEN("A OrderStatus event is dispatched") {
    auto evt = std::make_shared<vda5050pp::core::events::OrderStatus>();
    evt->status = vda5050pp::misc::OrderStatus::k_order_paused;

    auto sub = instance->getStateEventManager().getScopedSubscriber();
    bool called = false;
    sub.subscribe<vda5050pp::core::events::RequestStateUpdateEvent>(
        [&called](auto) { called = true; });

    instance->getOrderEventManager().synchronousDispatch(evt);

    THEN("The order status is set accordingly") {
      REQUIRE(instance->getOrderManager().getOrderStatus() ==
              vda5050pp::misc::OrderStatus::k_order_paused);
    }

    THEN("A RequestStateUpdate event was dispatched") { REQUIRE(called); }
  }

  WHEN("There is an active graph and some non-/waiting actions") {
    auto &om = instance->getOrderManager();
    auto a1 = mkAction("action1", "type1", vda5050::BlockingType::NONE);
    auto a2 = mkAction("action2", "type2", vda5050::BlockingType::NONE);
    om.addNewAction(wrap_shared(a1));
    om.addNewAction(wrap_shared(a2));
    om.replaceGraph(
        vda5050pp::core::state::Graph({
            vda5050pp::core::state::GraphElement(wrap_shared(mkNode("n0", 0, true, {}))),
            vda5050pp::core::state::GraphElement(wrap_shared(mkEdge("e1", 1, true, {}))),
            vda5050pp::core::state::GraphElement(wrap_shared(mkNode("n2", 2, true, {a1, a2}))),
        }),
        "order");
    om.getActionState("action1")->actionStatus = vda5050::ActionStatus::FINISHED;

    WHEN("An OrderClearAfterCancel event is dispatched") {
      auto evt = std::make_shared<vda5050pp::core::events::OrderClearAfterCancel>();
      evt->cancel_action = std::make_shared<vda5050::Action>();

      instance->getOrderEventManager().synchronousDispatch(evt);

      THEN("The graph is clear") {
        REQUIRE_THROWS_AS(om.getCurrentGraph(), vda5050pp::VDA5050PPInvalidArgument);
      }
      THEN("The finished action stays finished") {
        REQUIRE(om.getActionState("action1")->actionStatus == vda5050::ActionStatus::FINISHED);
      }
      THEN("The waiting action was set to failed") {
        REQUIRE(om.getActionState("action2")->actionStatus == vda5050::ActionStatus::FAILED);
      }
    }
  }

  WHEN("A NavigationStatusPosition event is dispatched") {
    vda5050::AGVPosition pos;
    pos.x = 1;
    pos.y = 2;

    auto evt = std::make_shared<vda5050pp::events::NavigationStatusPosition>();
    evt->position = pos;

    instance->getNavigationStatusManager().dispatch(evt);

    THEN("The status manager has the position") {
      vda5050::State state;
      instance->getStatusManager().dumpTo(state);
      REQUIRE(state.agvPosition.has_value());
      REQUIRE(state.agvPosition.value() == pos);
    }
  }

  WHEN("A NavigationStatusVelocity event is dispatched") {
    vda5050::Velocity vel;
    vel.vx = 1;
    vel.vy = 2;

    auto evt = std::make_shared<vda5050pp::events::NavigationStatusVelocity>();
    evt->velocity = vel;

    instance->getNavigationStatusManager().dispatch(evt);

    THEN("The status manager has the velocity") {
      vda5050::State state;
      instance->getStatusManager().dumpTo(state);
      REQUIRE(state.velocity.has_value());
      REQUIRE(state.velocity.value() == vel);
    }
  }

  WHEN("A NavigationStatusDistanceSinceLastNode event is dispatched") {
    double dist = 1.5;

    auto evt = std::make_shared<vda5050pp::events::NavigationStatusDistanceSinceLastNode>();
    evt->distance_since_last_node = dist;

    instance->getNavigationStatusManager().dispatch(evt);

    THEN("The status manager has the distance") {
      vda5050::State state;
      instance->getStatusManager().dumpTo(state);
      REQUIRE(state.distanceSinceLastNode.has_value());
      REQUIRE(state.distanceSinceLastNode.value() == dist);
    }
  }

  WHEN("A NavigationStatusDriving event is dispatched") {
    auto evt = std::make_shared<vda5050pp::events::NavigationStatusDriving>();
    evt->is_driving = true;

    bool called = false;
    auto sub = instance->getStateEventManager().getScopedSubscriber();
    sub.subscribe<vda5050pp::core::events::RequestStateUpdateEvent>(
        [&called](auto) { called = true; });

    instance->getNavigationStatusManager().dispatch(evt);

    THEN("The status manager has the driving state") {
      vda5050::State state;
      instance->getStatusManager().dumpTo(state);
      REQUIRE(state.driving);
    }

    THEN("A RequestStateUpdate event was dispatched") { REQUIRE(called); }
  }

  WHEN("A NavigationStatusNodeReached with a last_node_id is dispatched") {
    auto evt = std::make_shared<vda5050pp::events::NavigationStatusNodeReached>();
    evt->last_node_id = "node1";

    instance->getNavigationStatusManager().dispatch(evt);

    THEN("The last_node_id is set in the state") {
      vda5050::State state;
      instance->getOrderManager().dumpTo(state);
      REQUIRE(state.lastNodeId == evt->last_node_id);
    }
  }

  WHEN("A NavigationStatusNodeReached without a last_node_id is dispatched") {
    auto evt = std::make_shared<vda5050pp::events::NavigationStatusNodeReached>();
    evt->last_node_id = std::nullopt;

    instance->getNavigationStatusManager().dispatch(evt);

    THEN("The last_node_id is not set in the state") {
      vda5050::State state;
      instance->getOrderManager().dumpTo(state);
      REQUIRE(state.lastNodeId == "");
    }
  }

  auto add_load = std::make_shared<vda5050pp::events::LoadAdd>();
  add_load->load.loadId = "load_id";

  quickStateEventTest(
      add_load,
      [](const vda5050::State &state) {
        REQUIRE(state.loads.has_value());
        REQUIRE(state.loads.value().size() == 1);

        auto remove_load = std::make_shared<vda5050pp::events::LoadRemove>();
        remove_load->load_id = "load_id";
        quickStateEventTest(
            remove_load,
            [](const vda5050::State &s) {
              REQUIRE(s.loads.has_value());
              REQUIRE(s.loads.value().size() == 0);
            },
            true);

        auto alter_load = std::make_shared<vda5050pp::events::LoadsAlter>();
        alter_load->alter_function = [](auto &loads) { loads.push_back(vda5050::Load{}); };
        quickStateEventTest(
            alter_load,
            [](const vda5050::State &s) {
              REQUIRE(s.loads.has_value());
              REQUIRE(s.loads.value().size() == 2);
            },
            true);
      },
      true);

  auto operating_mode_set = std::make_shared<vda5050pp::events::OperatingModeSet>();
  operating_mode_set->operating_mode = vda5050::OperatingMode::SERVICE;

  quickStateEventTest(
      operating_mode_set,
      [](const vda5050::State &s) {
        REQUIRE(s.operatingMode == vda5050::OperatingMode::SERVICE);

        auto operating_mode_alter = std::make_shared<vda5050pp::events::OperatingModeAlter>();
        operating_mode_alter->alter_function = [](auto) {
          return vda5050::OperatingMode::SEMIAUTOMATIC;
        };
        quickStateEventTest(
            operating_mode_alter,
            [](const vda5050::State &s2) {
              REQUIRE(s2.operatingMode == vda5050::OperatingMode::SEMIAUTOMATIC);
            },
            true);
      },
      true);

  auto battery_state_set = std::make_shared<vda5050pp::events::BatteryStateSet>();
  battery_state_set->battery_state.batteryHealth = 100;

  quickStateEventTest(
      battery_state_set,
      [](const vda5050::State &s) {
        REQUIRE(s.batteryState.batteryHealth.value() == 100);

        auto battery_state_alter = std::make_shared<vda5050pp::events::BatteryStateAlter>();
        battery_state_alter->alter_function = [](auto &bs) { bs.batteryHealth = 50; };
        quickStateEventTest(
            battery_state_alter,
            [](const vda5050::State &s2) { REQUIRE(s2.batteryState.batteryHealth.value() == 50); },
            false);
      },
      false);

  auto request_new_base = std::make_shared<vda5050pp::events::RequestNewBase>();
  quickStateEventTest(
      request_new_base, [](const vda5050::State &state) { REQUIRE(state.newBaseRequest.value()); },
      true);

  auto error_add = std::make_shared<vda5050pp::events::ErrorAdd>();
  error_add->error.errorType = "t1";
  quickStateEventTest(
      error_add,
      [](const vda5050::State &state) {
        REQUIRE(state.errors.size() == 1);

        auto error_alter = std::make_shared<vda5050pp::events::ErrorsAlter>();
        error_alter->alter_function = [](auto &errors) { errors.push_back(vda5050::Error{}); };
        quickStateEventTest(
            error_alter, [](const vda5050::State &s) { REQUIRE(s.errors.size() == 2); }, true);
      },
      true);

  auto info_add = std::make_shared<vda5050pp::events::InfoAdd>();
  info_add->info.infoType = "t1";
  quickStateEventTest(
      info_add,
      [](const vda5050::State &state) {
        REQUIRE(state.informations.size() == 1);

        auto info_alter = std::make_shared<vda5050pp::events::InfosAlter>();
        info_alter->alter_function = [](auto &infos) { infos.push_back(vda5050::Info{}); };
        quickStateEventTest(
            info_alter, [](const vda5050::State &s) { REQUIRE(s.informations.size() == 2); }, true);
      },
      true);
}