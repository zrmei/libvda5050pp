// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/handler/base_navigation_handler.h"

#include <catch2/catch.hpp>

#include "test/data.h"
#include "vda5050++/core/instance.h"

class NavigationHandler : public vda5050pp::handler::BaseNavigationHandler {
public:
  void horizonUpdated(const std::list<std::shared_ptr<const vda5050::Node>> &,
                      const std::list<std::shared_ptr<const vda5050::Edge>> &) override { /* NOP */
  }
  void baseIncreased(const std::list<std::shared_ptr<const vda5050::Node>> &,
                     const std::list<std::shared_ptr<const vda5050::Edge>> &) override { /* NOP */
  }
  void navigateToNextNode(std::shared_ptr<const vda5050::Node>,
                          std::shared_ptr<const vda5050::Edge>) override { /* NOP */
  }
  void upcomingSegment(decltype(vda5050::Node::sequenceId),
                       decltype(vda5050::Node::sequenceId)) override { /* NOP */
  }
  void cancel() override { /* NOP */
  }
  void pause() override { /* NOP */
  }
  void resume() override { /* NOP */
  }
};

TEST_CASE("handler::BaseNavigationHandler member functions", "[handler]") {
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().useWhiteList();
  cfg.refGlobalConfig().refEventManagerOptions().synchronous_event_dispatch = true;
  vda5050pp::core::Instance::reset();
  auto instance = vda5050pp::core::Instance::init(cfg).lock();

  WHEN("setPaused() is called") {
    auto sub = instance->getNavigationStatusManager().getScopedNavigationStatusSubscriber();
    std::optional<vda5050pp::events::NavigationStatusControlType> type;
    sub.subscribe([&type](std::shared_ptr<vda5050pp::events::NavigationStatusControl> evt) {
      type = evt->type;
    });
    NavigationHandler().setPaused();

    THEN("A NavigationStatusControlEvent(type=paused) was dispatched") {
      REQUIRE(type.has_value());
      REQUIRE(type.value() == vda5050pp::events::NavigationStatusControlType::k_paused);
    }
  }

  WHEN("setResumed() is called") {
    auto sub = instance->getNavigationStatusManager().getScopedNavigationStatusSubscriber();
    std::optional<vda5050pp::events::NavigationStatusControlType> type;
    sub.subscribe([&type](std::shared_ptr<vda5050pp::events::NavigationStatusControl> evt) {
      type = evt->type;
    });
    NavigationHandler().setResumed();

    THEN("A NavigationStatusControlEvent(type=resumed) was dispatched") {
      REQUIRE(type.has_value());
      REQUIRE(type.value() == vda5050pp::events::NavigationStatusControlType::k_resumed);
    }
  }

  WHEN("setFailed() is called") {
    auto sub = instance->getNavigationStatusManager().getScopedNavigationStatusSubscriber();
    std::optional<vda5050pp::events::NavigationStatusControlType> type;
    sub.subscribe([&type](std::shared_ptr<vda5050pp::events::NavigationStatusControl> evt) {
      type = evt->type;
    });
    NavigationHandler().setFailed();

    THEN("A NavigationStatusControlEvent(type=failed) was dispatched") {
      REQUIRE(type.has_value());
      REQUIRE(type.value() == vda5050pp::events::NavigationStatusControlType::k_failed);
    }
  }

  WHEN("setNodeReached(vda5050::Node) is called") {
    auto sub = instance->getNavigationStatusManager().getScopedNavigationStatusSubscriber();
    std::shared_ptr<vda5050pp::events::NavigationStatusNodeReached> event;
    sub.subscribe([&event](std::shared_ptr<vda5050pp::events::NavigationStatusNodeReached> evt) {
      event = evt;
    });

    NavigationHandler().setNodeReached(test::data::mkNode("n1", 2, true, {}));

    THEN("A NavigationStatusNodeReachedEvent was dispatched") {
      REQUIRE(event);
      REQUIRE(event->node_seq_id == 2);
    }
  }

  WHEN("setNodeReached(seq_id) is called") {
    auto sub = instance->getNavigationStatusManager().getScopedNavigationStatusSubscriber();
    std::shared_ptr<vda5050pp::events::NavigationStatusNodeReached> event;
    sub.subscribe([&event](std::shared_ptr<vda5050pp::events::NavigationStatusNodeReached> evt) {
      event = evt;
    });

    NavigationHandler().setNodeReached(2);

    THEN("A NavigationStatusNodeReachedEvent was dispatched") {
      REQUIRE(event);
      REQUIRE(event->node_seq_id == 2);
    }
  }

  WHEN("setPosition(x,y,theta,map) is called") {
    auto sub = instance->getNavigationStatusManager().getScopedNavigationStatusSubscriber();
    std::shared_ptr<vda5050pp::events::NavigationStatusPosition> event;
    sub.subscribe([&event](std::shared_ptr<vda5050pp::events::NavigationStatusPosition> evt) {
      event = evt;
    });

    NavigationHandler().setPosition(1.0, 2.0, 3.0, "test_map");

    THEN("A NavigationStatusPosition event was dispatched") {
      REQUIRE(event);
      REQUIRE_FALSE(event->auto_check_node_reached);
      REQUIRE(event->position.x == 1.0);
      REQUIRE(event->position.y == 2.0);
      REQUIRE(event->position.theta == 3.0);
      REQUIRE(event->position.mapId == "test_map");
      REQUIRE_FALSE(event->position.deviationRange.has_value());
    }
  }

  WHEN("setPosition(x,y,theta,map,deviation) is called") {
    auto sub = instance->getNavigationStatusManager().getScopedNavigationStatusSubscriber();
    std::shared_ptr<vda5050pp::events::NavigationStatusPosition> event;
    sub.subscribe([&event](std::shared_ptr<vda5050pp::events::NavigationStatusPosition> evt) {
      event = evt;
    });

    NavigationHandler().setPosition(1.0, 2.0, 3.0, "test_map", 4.0);

    THEN("A NavigationStatusPosition event was dispatched") {
      REQUIRE(event);
      REQUIRE_FALSE(event->auto_check_node_reached);
      REQUIRE(event->position.x == 1.0);
      REQUIRE(event->position.y == 2.0);
      REQUIRE(event->position.theta == 3.0);
      REQUIRE(event->position.mapId == "test_map");
      REQUIRE(event->position.deviationRange.has_value());
      REQUIRE(event->position.deviationRange.value() == 4.0);
    }
  }

  WHEN("evalPosition(x,y,theta,map) is called") {
    auto sub = instance->getNavigationStatusManager().getScopedNavigationStatusSubscriber();
    std::shared_ptr<vda5050pp::events::NavigationStatusPosition> event;
    sub.subscribe([&event](std::shared_ptr<vda5050pp::events::NavigationStatusPosition> evt) {
      event = evt;
      auto tkn = evt->acquireResultToken();
      tkn.setValue(true);
    });

    auto ret = NavigationHandler().evalPosition(1.0, 2.0, 3.0, "test_map");

    THEN("A NavigationStatusPosition event was dispatched") {
      REQUIRE(event);
      REQUIRE(event->auto_check_node_reached);
      REQUIRE(event->position.x == 1.0);
      REQUIRE(event->position.y == 2.0);
      REQUIRE(event->position.theta == 3.0);
      REQUIRE(event->position.mapId == "test_map");
      REQUIRE_FALSE(event->position.deviationRange.has_value());
    }

    THEN("The return value is true") { REQUIRE(ret); }
  }

  WHEN("evalPosition(x,y,theta,map,deviation) is called") {
    auto sub = instance->getNavigationStatusManager().getScopedNavigationStatusSubscriber();
    std::shared_ptr<vda5050pp::events::NavigationStatusPosition> event;
    sub.subscribe([&event](std::shared_ptr<vda5050pp::events::NavigationStatusPosition> evt) {
      event = evt;
      auto tkn = evt->acquireResultToken();
      tkn.setValue(true);
    });

    auto ret = NavigationHandler().evalPosition(1.0, 2.0, 3.0, "test_map", 4.0);

    THEN("A NavigationStatusPosition event was dispatched") {
      REQUIRE(event);
      REQUIRE(event->auto_check_node_reached);
      REQUIRE(event->position.x == 1.0);
      REQUIRE(event->position.y == 2.0);
      REQUIRE(event->position.theta == 3.0);
      REQUIRE(event->position.mapId == "test_map");
      REQUIRE(event->position.deviationRange.has_value());
      REQUIRE(event->position.deviationRange.value() == 4.0);
    }

    THEN("The return value is true") { REQUIRE(ret); }
  }
}