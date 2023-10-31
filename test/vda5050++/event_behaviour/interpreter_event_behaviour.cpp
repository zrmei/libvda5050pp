//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include <catch2/catch.hpp>

#include "vda5050++/core/instance.h"

TEST_CASE("Behaviour of internal interpreter events", "[core][interpreter][event]") {
  vda5050pp::Config cfg;
  vda5050pp::config::EventManagerOptions evt_opts;
  evt_opts.synchronous_event_dispatch = true;
  cfg.refGlobalConfig().setLogLevel(vda5050pp::config::LogLevel::k_debug);
  cfg.refGlobalConfig().setEventManagerOptions(evt_opts);
  cfg.refGlobalConfig().useWhiteList();
  cfg.refGlobalConfig().bwListModule(vda5050pp::core::module_keys::k_state_event_handler_key);
  vda5050pp::core::Instance::reset();
  vda5050pp::core::Instance::init(cfg);

  auto node1 = std::make_shared<vda5050::Node>();
  node1->nodeId = "node1";
  node1->sequenceId = 0;
  node1->released = true;

  auto node2 = std::make_shared<vda5050::Node>();
  node2->nodeId = "node2";
  node2->sequenceId = 2;
  node2->released = true;

  auto node3 = std::make_shared<vda5050::Node>();
  node3->nodeId = "node3";
  node3->sequenceId = 4;
  node3->released = false;

  auto edge1 = std::make_shared<vda5050::Edge>();
  edge1->edgeId = "edge1";
  edge1->sequenceId = 1;
  edge1->released = true;

  auto edge2 = std::make_shared<vda5050::Edge>();
  edge2->edgeId = "edge2";
  edge2->sequenceId = 3;
  edge2->released = false;

  auto node12 = std::make_shared<vda5050::Node>();
  node12->nodeId = "node12";
  node12->sequenceId = 4;
  node12->released = true;

  auto node22 = std::make_shared<vda5050::Node>();
  node22->nodeId = "node22";
  node22->sequenceId = 6;
  node22->released = true;

  auto node32 = std::make_shared<vda5050::Node>();
  node32->nodeId = "node32";
  node32->sequenceId = 8;
  node32->released = false;

  auto edge12 = std::make_shared<vda5050::Edge>();
  edge12->edgeId = "edge12";
  edge12->sequenceId = 3;
  edge12->released = true;

  auto edge22 = std::make_shared<vda5050::Edge>();
  edge22->edgeId = "edge22";
  edge22->sequenceId = 5;
  edge22->released = true;

  auto edge32 = std::make_shared<vda5050::Edge>();
  edge32->edgeId = "edge32";
  edge32->sequenceId = 7;
  edge32->released = false;

  vda5050pp::core::state::GraphElement gn1(node1);
  vda5050pp::core::state::GraphElement gn2(node2);
  vda5050pp::core::state::GraphElement gn3(node3);
  vda5050pp::core::state::GraphElement ge1(edge1);
  vda5050pp::core::state::GraphElement ge2(edge2);
  vda5050pp::core::state::GraphElement gn12(node12);
  vda5050pp::core::state::GraphElement gn22(node22);
  vda5050pp::core::state::GraphElement gn32(node32);
  vda5050pp::core::state::GraphElement ge12(edge12);
  vda5050pp::core::state::GraphElement ge22(edge22);
  vda5050pp::core::state::GraphElement ge32(edge32);

  vda5050pp::core::state::Graph g1({gn1, gn2, gn3, ge1, ge2});
  vda5050pp::core::state::Graph g2({gn12, gn22, gn32, ge12, ge22, ge32});
  vda5050pp::core::state::Graph g3({gn32, ge22, gn22});

  WHEN("A YieldNewAction event is dispatched") {
    auto evt_a1 = std::make_shared<vda5050pp::core::events::YieldNewAction>();
    evt_a1->action = std::make_shared<vda5050::Action>();
    evt_a1->action->actionId = "Action1";
    evt_a1->action->actionType = "Type1";
    evt_a1->action->actionDescription = "Desc1";

    vda5050pp::core::Instance::ref().getInterpreterEventManager().dispatch(evt_a1);

    THEN("The action is known in the state") {
      auto action =
          vda5050pp::core::Instance::ref().getOrderManager().getAction(evt_a1->action->actionId);
      REQUIRE(*action == *evt_a1->action);
    }

    THEN("The action state is as expected") {
      auto as = vda5050pp::core::Instance::ref().getOrderManager().getActionState(
          evt_a1->action->actionId);
      REQUIRE(as->actionId == evt_a1->action->actionId);
      REQUIRE(as->actionStatus == vda5050::ActionStatus::WAITING);
      REQUIRE(as->actionType == evt_a1->action->actionType);
      REQUIRE(as->actionDescription == evt_a1->action->actionDescription);
    }
  }

  WHEN("A YieldGraphExtension event is dispatched (with no existing graph)") {
    auto evt_ex = std::make_shared<vda5050pp::core::events::YieldGraphExtension>();
    evt_ex->graph = std::make_shared<vda5050pp::core::state::Graph>(g1);
    evt_ex->order_update_id = 0;

    REQUIRE_THROWS_AS(
        vda5050pp::core::Instance::ref().getInterpreterEventManager().dispatch(evt_ex),
        vda5050pp::VDA5050PPInvalidArgument);

    THEN("The extension is not used") {
      REQUIRE_FALSE(vda5050pp::core::Instance::ref().getOrderManager().hasGraph());
    }
  }

  WHEN("A YieldGraphReplacement event is dispatched (with no existing graph)") {
    auto evt_rp = std::make_shared<vda5050pp::core::events::YieldGraphReplacement>();
    evt_rp->graph = std::make_shared<vda5050pp::core::state::Graph>(g1);
    evt_rp->order_id = "Order1";

    vda5050pp::core::Instance::ref().getInterpreterEventManager().dispatch(evt_rp);

    THEN("The state is set accordingly") {
      vda5050::State state;
      // node1 is not dumped, because the AGV is on it
      vda5050pp::core::Instance::ref().getOrderManager().dumpTo(state);

      REQUIRE(state.orderId == evt_rp->order_id);
      REQUIRE(state.orderUpdateId == 0);
      REQUIRE(state.nodeStates[0].nodeId == node2->nodeId);
      REQUIRE(state.nodeStates[1].nodeId == node3->nodeId);
      REQUIRE(state.edgeStates[0].edgeId == edge1->edgeId);
      REQUIRE(state.edgeStates[1].edgeId == edge2->edgeId);
    }

    WHEN("A YieldGraphExtension event is dispatched (with an existing graph)") {
      auto evt_ex = std::make_shared<vda5050pp::core::events::YieldGraphExtension>();
      evt_ex->graph = std::make_shared<vda5050pp::core::state::Graph>(g2);
      evt_ex->order_update_id = 1;

      vda5050pp::core::Instance::ref().getInterpreterEventManager().dispatch(evt_ex);

      THEN("The state is set accordingly") {
        vda5050::State state;
        // node1 is not dumped, because the AGV is on it
        vda5050pp::core::Instance::ref().getOrderManager().dumpTo(state);

        REQUIRE(state.orderId == evt_rp->order_id);
        REQUIRE(state.orderUpdateId == evt_ex->order_update_id);
        REQUIRE(state.nodeStates[0].nodeId == node2->nodeId);
        REQUIRE(state.nodeStates[1].nodeId == node12->nodeId);
        REQUIRE(state.nodeStates[2].nodeId == node22->nodeId);
        REQUIRE(state.nodeStates[3].nodeId == node32->nodeId);
        REQUIRE(state.edgeStates[0].edgeId == edge1->edgeId);
        REQUIRE(state.edgeStates[1].edgeId == edge12->edgeId);
        REQUIRE(state.edgeStates[2].edgeId == edge22->edgeId);
        REQUIRE(state.edgeStates[3].edgeId == edge32->edgeId);
      }
    }
  }
}