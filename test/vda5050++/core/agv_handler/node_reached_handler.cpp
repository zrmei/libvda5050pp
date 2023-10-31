// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include <catch2/catch.hpp>

#include "vda5050++/core/events/event_control_blocks.h"
#include "vda5050++/core/instance.h"

using namespace std::chrono_literals;

TEST_CASE("core::agv_handler::NodeReachedHandler", "[core][agv_handler]") {
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().refEventManagerOptions().synchronous_event_dispatch = true;
  cfg.refGlobalConfig().useWhiteList();
  cfg.refGlobalConfig().bwListModule(vda5050pp::core::module_keys::k_node_reached_handler_key);
  vda5050pp::core::Instance::reset();
  auto instance = vda5050pp::core::Instance::init(cfg).lock();

  auto pos_reached_check = std::make_shared<vda5050pp::events::NavigationStatusPosition>();
  pos_reached_check->auto_check_node_reached = true;
  pos_reached_check->position.x = 0;
  pos_reached_check->position.y = 1;
  pos_reached_check->position.theta = 0;
  pos_reached_check->position.mapId = "map";

  auto pos_reached_no_check = std::make_shared<vda5050pp::events::NavigationStatusPosition>();
  pos_reached_no_check->auto_check_node_reached = false;
  pos_reached_no_check->position.x = 0;
  pos_reached_no_check->position.y = 1;
  pos_reached_no_check->position.theta = 0;
  pos_reached_no_check->position.mapId = "map";

  auto pos_not_reached_check = std::make_shared<vda5050pp::events::NavigationStatusPosition>();
  pos_not_reached_check->auto_check_node_reached = true;
  pos_not_reached_check->position.x = 2;
  pos_not_reached_check->position.y = 2;
  pos_not_reached_check->position.theta = 0;
  pos_not_reached_check->position.mapId = "map";

  auto pos_other_map_check = std::make_shared<vda5050pp::events::NavigationStatusPosition>();
  pos_other_map_check->auto_check_node_reached = true;
  pos_other_map_check->position.x = 0;
  pos_other_map_check->position.y = 1;
  pos_other_map_check->position.theta = 0;
  pos_other_map_check->position.mapId = "map2";

  auto goal = std::make_shared<vda5050::Node>();
  goal->nodePosition = vda5050::NodePosition{};
  goal->nodePosition->x = 0;
  goal->nodePosition->y = 1;
  goal->nodePosition->theta = 0;
  goal->nodePosition->mapId = "map";
  goal->nodePosition->allowedDeviationXY = 1;
  goal->nodePosition->allowedDeviationTheta = 0.1;
  goal->sequenceId = 2;
  goal->released = true;

  auto edge = std::make_shared<vda5050::Edge>();
  edge->sequenceId = 1;
  edge->released = true;

  auto start = std::make_shared<vda5050::Node>();
  *start = *goal;
  start->sequenceId = 0;

  WHEN("The a check event is dispatched") {
    instance->getOrderManager().replaceGraph(
        vda5050pp::core::state::Graph{vda5050pp::core::state::GraphElement(start),
                                      vda5050pp::core::state::GraphElement(edge),
                                      vda5050pp::core::state::GraphElement(goal)},
        "o1");
    instance->getOrderManager().setAGVLastNode(0);

    int counter = 0;
    auto sub = instance->getNavigationStatusManager().getScopedNavigationStatusSubscriber();
    sub.subscribe(
        [&counter](std::shared_ptr<vda5050pp::events::NavigationStatusNodeReached>) { counter++; });

    instance->getNavigationStatusManager().dispatch(pos_reached_check);
    instance->getNavigationStatusManager().dispatch(pos_not_reached_check);
    instance->getNavigationStatusManager().dispatch(pos_other_map_check);
    THEN("The correct result is returned") {
      auto future1 = pos_reached_check->getFuture();
      auto future2 = pos_not_reached_check->getFuture();
      auto future3 = pos_other_map_check->getFuture();
      REQUIRE(future1.wait_for(0s) == std::future_status::ready);
      REQUIRE(future1.get());
      REQUIRE(future2.wait_for(0s) == std::future_status::ready);
      REQUIRE_FALSE(future2.get());
      REQUIRE(future3.wait_for(0s) == std::future_status::ready);
      REQUIRE_FALSE(future3.get());
    }

    THEN("The node reached event is correctly dispatched") { REQUIRE(counter == 1); }

    WHEN("A no-check event is dispatched") {
      instance->getNavigationStatusManager().dispatch(pos_reached_no_check);
      THEN("The no result is returned") {
        auto future1 = pos_reached_no_check->getFuture();
        REQUIRE(future1.wait_for(0s) == std::future_status::timeout);
      }
      THEN("The node reached event is not dispatched") { REQUIRE(counter == 1); }
    }
  }

  WHEN("There is no goal and an event is dispatched") {
    int counter = 0;
    auto sub = instance->getNavigationStatusManager().getScopedNavigationStatusSubscriber();
    sub.subscribe(
        [&counter](std::shared_ptr<vda5050pp::events::NavigationStatusNodeReached>) { counter++; });

    instance->getNavigationStatusManager().dispatch(pos_reached_check);

    THEN("It is not reached") {
      REQUIRE(counter == 0);
      auto future1 = pos_reached_check->getFuture();
      REQUIRE(future1.wait_for(0s) == std::future_status::ready);
      REQUIRE_FALSE(future1.get());
    }
  }
}