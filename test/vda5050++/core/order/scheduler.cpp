//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/order/scheduler.h"

#include <spdlog/fmt/fmt.h>

#include <catch2/catch.hpp>

#include "test/data.h"
#include "vda5050++/core/instance.h"

static auto assertState(vda5050pp::core::order::SchedulerStateType state) {
  return [state](vda5050pp::core::order::Scheduler &scheduler) {
    THEN(fmt::format("SchedulerState is {}", (int)state)) {
      REQUIRE(scheduler.getState() == state);
    }
  };
}

static void dispatchNodeReached(uint32_t seq_id) {
  auto evt = std::make_shared<vda5050pp::events::NavigationStatusNodeReached>();
  evt->node_seq_id = seq_id;

  vda5050pp::core::Instance::ref().getNavigationStatusManager().dispatch(evt);
}

template <typename T> constexpr bool const_true(T &&) { return true; }
template <typename T> constexpr bool const_false(T &&) { return false; }

template <typename Event> class EventAsserter {
private:
  std::shared_ptr<void> subscriber_;
  std::future<std::shared_ptr<Event>> future_;

  template <typename Future>
  EventAsserter(Future &&f, std::shared_ptr<void> sub)
      : subscriber_(sub), future_(std::forward<Future>(f)) {}

public:
  EventAsserter(const EventAsserter &) = delete;
  EventAsserter(EventAsserter &&) = delete;
  ~EventAsserter() {
    using namespace std::chrono_literals;
    if (this->future_.valid()) {
      this->future_.wait_for(100ms);
    }
  }

  template <typename FilterPredicate>
  static EventAsserter<Event> forOrderEvent(FilterPredicate &&p) {
    auto sub = test::data::wrap_shared(
        vda5050pp::core::Instance::ref().getOrderEventManager().getScopedSubscriber());
    auto promise = std::make_shared<std::promise<std::shared_ptr<Event>>>();
    sub->subscribe<Event>([filter_predicate = std::forward<FilterPredicate>(p),
                           promise](std::shared_ptr<Event> event) mutable {
      if (filter_predicate(event) && promise != nullptr) {
        promise->set_value(event);
        promise = nullptr;
      }
    });

    return EventAsserter(promise->get_future(), sub);
  }

  template <typename FilterPredicate>
  static EventAsserter<Event> forNavigationEvent(FilterPredicate &&p) {
    auto sub = test::data::wrap_shared(vda5050pp::core::Instance::ref()
                                           .getNavigationEventManager()
                                           .getScopedNavigationEventSubscriber());
    auto promise = std::make_shared<std::promise<std::shared_ptr<Event>>>();
    sub->subscribe([filter_predicate = std::forward<FilterPredicate>(p),
                    promise](std::shared_ptr<Event> event) mutable {
      if (filter_predicate(event) && promise != nullptr) {
        promise->set_value(event);
        promise = nullptr;
      }
    });

    return EventAsserter(promise->get_future(), sub);
  }

  template <typename FilterPredicate>
  static EventAsserter<Event> forActionEvent(FilterPredicate &&p) {
    auto sub = test::data::wrap_shared(
        vda5050pp::core::Instance::ref().getActionEventManager().getScopedActionEventSubscriber());
    auto promise = std::make_shared<std::promise<std::shared_ptr<Event>>>();
    sub->subscribe([filter_predicate = std::forward<FilterPredicate>(p),
                    promise](std::shared_ptr<Event> event) mutable {
      if (filter_predicate(event) && promise != nullptr) {
        promise->set_value(event);
        promise = nullptr;
      }
    });

    return EventAsserter(promise->get_future(), sub);
  }

  template <typename Predicate> void assertNow(Predicate &&p) {
    using namespace std::chrono_literals;
    REQUIRE(this->future_.wait_for(100ms) == std::future_status::ready);
    p(this->future_.get());
  }

  void assertNone() {
    using namespace std::chrono_literals;
    REQUIRE_FALSE(this->future_.wait_for(100ms) == std::future_status::ready);
  }
};

static void assertOrderStatus(EventAsserter<vda5050pp::core::events::OrderStatus> &asserter,
                              vda5050pp::misc::OrderStatus status) {
  THEN(fmt::format("A OrderStatus={} is received", int(status))) {
    asserter.assertNow([status](auto evt_ptr) { REQUIRE(evt_ptr->status == status); });
  }
}

static void assertNavigation(EventAsserter<vda5050pp::events::NavigationNextNode> &asserter,
                             std::string_view node_id, std::string_view edge_id) {
  THEN(fmt::format("A NavigationNextNode=(node_id={}, edge_id={}) is received", node_id, edge_id)) {
    asserter.assertNow([node_id, edge_id](auto evt_ptr) {
      REQUIRE(evt_ptr->next_node->nodeId == node_id);
      REQUIRE(evt_ptr->via_edge->edgeId == edge_id);
    });
  }
}

static void assertOrderNewLastNodeId(
    EventAsserter<vda5050pp::core::events::OrderNewLastNodeId> &asserter,
    std::string_view last_node_id) {
  THEN(fmt::format("A OrderNewLastNodeId=(node_id={}) is received", last_node_id)) {
    asserter.assertNow(
        [last_node_id](auto evt_ptr) { REQUIRE(evt_ptr->last_node_id == last_node_id); });
  }
}

static void assertActionFinished(
    EventAsserter<vda5050pp::core::events::OrderActionStatusChanged> &asserter,
    std::string_view action_id) {
  THEN(fmt::format("A OrderActionStatusChanged=(action={}, finished) is received", action_id)) {
    asserter.assertNow([action_id](auto evt_ptr) {
      REQUIRE(evt_ptr->action_id == action_id);
      REQUIRE(evt_ptr->action_status == vda5050::ActionStatus::FINISHED);
    });
  }
}

static void assertNavigationControl(EventAsserter<vda5050pp::events::NavigationControl> &asserter,
                                    vda5050pp::events::NavigationControlType type) {
  THEN(fmt::format("A NavigationControl(type={}) is received", int(type))) {
    asserter.assertNow([type](std::shared_ptr<vda5050pp::events::NavigationControl> evt) {
      REQUIRE(evt->type == type);
    });
  }
}

template <typename ActionEvent>
static void assertActionEvent(EventAsserter<ActionEvent> &asserter, std::string_view action_id) {
  THEN(fmt::format("A ActionEvent for action_id={} is received", action_id)) {
    asserter.assertNow([action_id](auto evt_ptr) { REQUIRE(evt_ptr->action_id == action_id); });
  }
}

TEST_CASE("core::order::Scheduler - only navigation", "[core][order]") {
  vda5050pp::core::Instance::reset();
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().useWhiteList();
  vda5050pp::core::Instance::init(cfg);

  auto evt1 = std::make_shared<vda5050pp::core::events::YieldNavigationStepEvent>();
  evt1->goal_node = test::data::wrap_shared(test::data::mkNode("n0", 0, true, {}));
  evt1->via_edge = test::data::wrap_shared(test::data::mkEdge("e1", 1, true, {}));
  evt1->has_stop_at_goal_hint = false;

  auto evt2 = std::make_shared<vda5050pp::core::events::YieldNavigationStepEvent>();
  evt2->goal_node = test::data::wrap_shared(test::data::mkNode("n2", 2, true, {}));
  evt2->via_edge = test::data::wrap_shared(test::data::mkEdge("e3", 3, true, {}));
  evt2->has_stop_at_goal_hint = false;

  auto evt3 = std::make_shared<vda5050pp::core::events::YieldNavigationStepEvent>();
  evt3->goal_node = test::data::wrap_shared(test::data::mkNode("n4", 4, true, {}));
  evt3->via_edge = test::data::wrap_shared(test::data::mkEdge("e5", 5, true, {}));
  evt3->has_stop_at_goal_hint = false;

  vda5050pp::core::order::Scheduler scheduler;

  WHEN("The first step is enqueued") {
    {
      auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
          [](auto) { return true; });
      auto evt_navigation =
          EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
              [](auto) { return true; });

      scheduler.enqueue(evt1);
      scheduler.update();
      assertState(vda5050pp::core::order::SchedulerStateType::k_active)(scheduler);
      assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_active);
      assertNavigation(evt_navigation, "n0", "e1");
    }
    WHEN("Reaching the first node") {
      {
        auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
            [](auto) { return true; });
        auto evt_navigation =
            EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
                [](auto) { return true; });
        scheduler.navigationTransition(vda5050pp::core::order::NavigationTransition::toSeqId(0));
        assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_idle);
        THEN("No nav event is generated") { evt_navigation.assertNone(); }
      }
    }
    WHEN("The remaining steps are enqueued") {
      {
        auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
            [](auto) { return true; });
        auto evt_navigation =
            EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
                [](auto) { return true; });
        scheduler.enqueue(evt2);
        scheduler.enqueue(evt3);
        scheduler.update();

        THEN("No events are generated") {
          evt_order_status.assertNone();
          evt_navigation.assertNone();
        }
      }

      WHEN("The first node is reached") {
        {
          auto evt_order_status =
              EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
                  [](auto) { return true; });
          auto evt_order_last_node =
              EventAsserter<vda5050pp::core::events::OrderNewLastNodeId>::forOrderEvent(
                  [](auto) { return true; });
          auto evt_navigation =
              EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
                  [](auto) { return true; });

          THEN("Reaching future nodes throws") {
            REQUIRE_THROWS_AS(scheduler.navigationTransition(
                                  vda5050pp::core::order::NavigationTransition::toSeqId(2)),
                              vda5050pp::VDA5050PPInvalidState);
          }
          scheduler.navigationTransition(vda5050pp::core::order::NavigationTransition::toSeqId(0));
          THEN("Order status is not updated") { evt_order_status.assertNone(); }
          assertNavigation(evt_navigation, "n2", "e3");
          assertOrderNewLastNodeId(evt_order_last_node, "n0");
        }
        WHEN("The second node is reached") {
          {
            auto evt_order_status =
                EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
                    [](auto) { return true; });
            auto evt_order_last_node =
                EventAsserter<vda5050pp::core::events::OrderNewLastNodeId>::forOrderEvent(
                    [](auto) { return true; });
            auto evt_navigation =
                EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
                    [](auto) { return true; });

            THEN("Reaching future nodes throws") {
              REQUIRE_THROWS_AS(scheduler.navigationTransition(
                                    vda5050pp::core::order::NavigationTransition::toSeqId(4)),
                                vda5050pp::VDA5050PPInvalidState);
            }
            scheduler.navigationTransition(
                vda5050pp::core::order::NavigationTransition::toSeqId(2));
            THEN("Order status is not updated") { evt_order_status.assertNone(); }
            assertNavigation(evt_navigation, "n4", "e5");
            assertOrderNewLastNodeId(evt_order_last_node, "n2");
          }
          WHEN("The third node is reached") {
            {
              auto evt_order_status =
                  EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
                      [](auto) { return true; });
              auto evt_order_last_node =
                  EventAsserter<vda5050pp::core::events::OrderNewLastNodeId>::forOrderEvent(
                      [](auto) { return true; });
              auto evt_navigation =
                  EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
                      [](auto) { return true; });

              THEN("Reaching future nodes throws") {
                REQUIRE_THROWS_AS(scheduler.navigationTransition(
                                      vda5050pp::core::order::NavigationTransition::toSeqId(6)),
                                  vda5050pp::VDA5050PPInvalidState);
              }
              scheduler.navigationTransition(
                  vda5050pp::core::order::NavigationTransition::toSeqId(4));
              THEN("No new navigation is generated") { evt_navigation.assertNone(); }
              assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_idle);
              assertOrderNewLastNodeId(evt_order_last_node, "n4");
            }
          }
        }
      }
    }
  }
}

TEST_CASE("core::order::Scheduler - only navigation with actions", "[core][order]") {
  vda5050pp::core::Instance::reset();
  vda5050pp::Config cfg;
  vda5050pp::config::EventManagerOptions opts;
  opts.synchronous_event_dispatch = true;
  cfg.refGlobalConfig().setEventManagerOptions(opts);
  cfg.refGlobalConfig().useWhiteList();
  vda5050pp::core::Instance::init(cfg);

  auto evt1 = std::make_shared<vda5050pp::core::events::YieldNavigationStepEvent>();
  evt1->goal_node = test::data::wrap_shared(test::data::mkNode("n0", 0, true, {}));
  evt1->via_edge = test::data::wrap_shared(test::data::mkEdge("e1", 1, true, {}));
  evt1->has_stop_at_goal_hint = true;

  auto evt1a = std::make_shared<vda5050pp::core::events::YieldActionGroupEvent>();
  evt1a->actions = {
      test::data::wrap_shared(test::data::mkAction("a11", "", vda5050::BlockingType::SOFT)),
      test::data::wrap_shared(test::data::mkAction("a12", "", vda5050::BlockingType::NONE)),
  };
  evt1a->blocking_type_ceiling = vda5050::BlockingType::SOFT;

  auto evt1b = std::make_shared<vda5050pp::core::events::YieldActionGroupEvent>();
  evt1b->actions = {
      test::data::wrap_shared(test::data::mkAction("a13", "", vda5050::BlockingType::HARD)),
  };
  evt1b->blocking_type_ceiling = vda5050::BlockingType::HARD;

  auto evt2 = std::make_shared<vda5050pp::core::events::YieldNavigationStepEvent>();
  evt2->goal_node = test::data::wrap_shared(test::data::mkNode("n2", 2, true, {}));
  evt2->via_edge = test::data::wrap_shared(test::data::mkEdge("e3", 3, true, {}));
  evt2->has_stop_at_goal_hint = false;

  auto evt2a = std::make_shared<vda5050pp::core::events::YieldActionGroupEvent>();
  evt2a->actions = {
      test::data::wrap_shared(test::data::mkAction("a21", "", vda5050::BlockingType::NONE)),
      test::data::wrap_shared(test::data::mkAction("a22", "", vda5050::BlockingType::NONE)),
  };
  evt2a->blocking_type_ceiling = vda5050::BlockingType::NONE;

  auto evt3 = std::make_shared<vda5050pp::core::events::YieldNavigationStepEvent>();
  evt3->goal_node = test::data::wrap_shared(test::data::mkNode("n4", 4, true, {}));
  evt3->via_edge = test::data::wrap_shared(test::data::mkEdge("e5", 5, true, {}));
  evt3->has_stop_at_goal_hint = false;

  vda5050pp::core::order::Scheduler scheduler;

  // Start
  {
    auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
        [](auto) { return true; });
    auto evt_navigation = EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
        [](auto) { return true; });

    scheduler.enqueue(evt1);
    scheduler.enqueue(evt1a);
    scheduler.enqueue(evt1b);
    scheduler.enqueue(evt2);
    scheduler.enqueue(evt2a);
    scheduler.enqueue(evt3);
    scheduler.update();

    assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_active);
    assertNavigation(evt_navigation, "n0", "e1");
  }

  WHEN("The first node is reached") {
    {
      auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
          [](auto) { return true; });
      auto evt_navigation =
          EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
              [](auto) { return true; });
      auto evt_a11 = EventAsserter<vda5050pp::events::ActionStart>::forActionEvent(
          [](auto evt) { return evt->action_id == "a11"; });
      auto evt_a12 = EventAsserter<vda5050pp::events::ActionStart>::forActionEvent(
          [](auto evt) { return evt->action_id == "a12"; });

      scheduler.navigationTransition(vda5050pp::core::order::NavigationTransition::toSeqId(0));

      THEN("no navigation and order status event were generated") {
        evt_order_status.assertNone();
        evt_navigation.assertNone();
      }
      assertActionEvent<vda5050pp::events::ActionStart>(evt_a11, "a11");
      assertActionEvent<vda5050pp::events::ActionStart>(evt_a12, "a12");
    }

    WHEN("One action finishes") {
      {
        auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
            [](auto) { return true; });
        auto evt_navigation =
            EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
                [](auto) { return true; });
        auto evt_action_status =
            EventAsserter<vda5050pp::core::events::OrderActionStatusChanged>::forOrderEvent(
                [](auto) { return true; });
        scheduler.actionTransition("a11", vda5050pp::core::order::ActionTransition::isFinished());
        THEN("no navigation and order status event were generated") {
          evt_order_status.assertNone();
          evt_navigation.assertNone();
        }
        assertActionFinished(evt_action_status, "a11");
      }
      WHEN("The next action finishes") {
        {
          auto evt_order_status =
              EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
                  [](auto) { return true; });
          auto evt_navigation =
              EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
                  [](auto) { return true; });
          auto evt_action_status =
              EventAsserter<vda5050pp::core::events::OrderActionStatusChanged>::forOrderEvent(
                  [](auto) { return true; });
          auto evt_a13 = EventAsserter<vda5050pp::events::ActionStart>::forActionEvent(
              [](auto evt) { return evt->action_id == "a13"; });
          scheduler.actionTransition("a12", vda5050pp::core::order::ActionTransition::isFinished());
          THEN("no navigation and order status event were generated") {
            evt_order_status.assertNone();
            evt_navigation.assertNone();
          }
          assertActionFinished(evt_action_status, "a12");
          assertActionEvent<vda5050pp::events::ActionStart>(evt_a13, "a13");
        }
        WHEN("The action a13 finishes") {
          {
            auto evt_order_status =
                EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
                    [](auto) { return true; });
            auto evt_navigation =
                EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
                    [](auto) { return true; });
            auto evt_action_status =
                EventAsserter<vda5050pp::core::events::OrderActionStatusChanged>::forOrderEvent(
                    [](auto) { return true; });
            auto evt_a21 = EventAsserter<vda5050pp::events::ActionStart>::forActionEvent(
                [](auto evt) { return evt->action_id == "a21"; });
            auto evt_a22 = EventAsserter<vda5050pp::events::ActionStart>::forActionEvent(
                [](auto evt) { return evt->action_id == "a22"; });
            scheduler.actionTransition("a13",
                                       vda5050pp::core::order::ActionTransition::isFinished());
            THEN("No order status event was generated") { evt_order_status.assertNone(); }
            assertActionFinished(evt_action_status, "a13");
            assertNavigation(evt_navigation, "n2", "e3");
            assertActionEvent<vda5050pp::events::ActionStart>(evt_a21, "a21");
            assertActionEvent<vda5050pp::events::ActionStart>(evt_a22, "a22");
          }

          WHEN("n2 is reached") {
            {
              auto evt_order_status =
                  EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
                      [](auto) { return true; });
              auto evt_navigation =
                  EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
                      [](auto) { return true; });
              scheduler.navigationTransition(
                  vda5050pp::core::order::NavigationTransition::toSeqId(2));
              THEN("No order status event was generated") { evt_order_status.assertNone(); }
              assertNavigation(evt_navigation, "n4", "e5");
            }

            WHEN("A21 finishes") {
              {
                auto evt_order_status =
                    EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
                        [](auto) { return true; });
                auto evt_navigation =
                    EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
                        [](auto) { return true; });
                auto evt_action_status =
                    EventAsserter<vda5050pp::core::events::OrderActionStatusChanged>::forOrderEvent(
                        [](auto) { return true; });
                scheduler.actionTransition("a21",
                                           vda5050pp::core::order::ActionTransition::isFinished());
                THEN("No order status event was generated") { evt_order_status.assertNone(); }
                assertActionFinished(evt_action_status, "a21");
              }

              WHEN("A22 finishes") {
                {
                  auto evt_order_status =
                      EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
                          [](auto) { return true; });
                  auto evt_navigation =
                      EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
                          [](auto) { return true; });
                  auto evt_action_status = EventAsserter<
                      vda5050pp::core::events::OrderActionStatusChanged>::forOrderEvent([](auto) {
                    return true;
                  });
                  scheduler.actionTransition(
                      "a22", vda5050pp::core::order::ActionTransition::isFinished());
                  assertActionFinished(evt_action_status, "a22");
                  THEN("No order status event was generated") { evt_order_status.assertNone(); }
                }

                WHEN("The n4 is reached") {
                  {
                    auto evt_order_status =
                        EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
                            [](auto) { return true; });
                    auto evt_navigation =
                        EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
                            [](auto) { return true; });
                    scheduler.navigationTransition(
                        vda5050pp::core::order::NavigationTransition::toSeqId(4));
                    assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_idle);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

TEST_CASE("core::order::Scheduler - pause/resume", "[core][order]") {
  vda5050pp::core::Instance::reset();
  vda5050pp::Config cfg;
  vda5050pp::config::EventManagerOptions opts;
  opts.synchronous_event_dispatch = true;
  cfg.refGlobalConfig().setEventManagerOptions(opts);
  cfg.refGlobalConfig().useWhiteList();
  vda5050pp::core::Instance::init(cfg);

  auto evt1 = std::make_shared<vda5050pp::core::events::YieldNavigationStepEvent>();
  evt1->goal_node = test::data::wrap_shared(test::data::mkNode("n0", 0, true, {}));
  evt1->via_edge = test::data::wrap_shared(test::data::mkEdge("e1", 1, true, {}));
  evt1->has_stop_at_goal_hint = false;

  auto evt1a = std::make_shared<vda5050pp::core::events::YieldActionGroupEvent>();
  evt1a->actions = {
      test::data::wrap_shared(test::data::mkAction("a11", "", vda5050::BlockingType::NONE)),
      test::data::wrap_shared(test::data::mkAction("a12", "", vda5050::BlockingType::NONE)),
  };
  evt1a->blocking_type_ceiling = vda5050::BlockingType::NONE;

  vda5050pp::core::order::Scheduler scheduler;

  // Start
  {
    auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
        [](auto) { return true; });
    auto evt_navigation = EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
        [](auto) { return true; });
    auto evt_a11 = EventAsserter<vda5050pp::events::ActionStart>::forActionEvent(
        [](auto evt) { return evt->action_id == "a11"; });
    auto evt_a12 = EventAsserter<vda5050pp::events::ActionStart>::forActionEvent(
        [](auto evt) { return evt->action_id == "a12"; });

    scheduler.enqueue(evt1);
    scheduler.enqueue(evt1a);
    scheduler.update();

    assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_active);
    assertNavigation(evt_navigation, "n0", "e1");
    assertActionEvent<vda5050pp::events::ActionStart>(evt_a11, "a11");
    assertActionEvent<vda5050pp::events::ActionStart>(evt_a12, "a12");
  }

  WHEN("A pause is requested") {
    {
      auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
          [](auto) { return true; });
      auto evt_nav_ctrl = EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
          [](auto) { return true; });
      auto evt_a11 = EventAsserter<vda5050pp::events::ActionPause>::forActionEvent(
          [](auto evt) { return evt->action_id == "a11"; });
      auto evt_a12 = EventAsserter<vda5050pp::events::ActionPause>::forActionEvent(
          [](auto evt) { return evt->action_id == "a12"; });
      scheduler.pause();

      assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_pausing);
      assertActionEvent<vda5050pp::events::ActionPause>(evt_a11, "a11");
      assertActionEvent<vda5050pp::events::ActionPause>(evt_a12, "a12");
      assertNavigationControl(evt_nav_ctrl, vda5050pp::events::NavigationControlType::k_pause);
    }
    WHEN("A11 pauses") {
      {
        auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
            [](auto) { return true; });
        auto evt_nav_ctrl = EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
            [](auto) { return true; });
        auto evt_a11 = EventAsserter<vda5050pp::events::ActionPause>::forActionEvent(
            [](auto evt) { return evt->action_id == "a11"; });
        auto evt_a12 = EventAsserter<vda5050pp::events::ActionPause>::forActionEvent(
            [](auto evt) { return evt->action_id == "a12"; });

        scheduler.actionTransition("a11", vda5050pp::core::order::ActionTransition::isPaused());

        THEN("The no events are generated") {
          evt_order_status.assertNone();
          evt_nav_ctrl.assertNone();
          evt_a11.assertNone();
          evt_a12.assertNone();
        }
      }
      WHEN("A12 pauses") {
        {
          auto evt_order_status =
              EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
                  [](auto) { return true; });
          auto evt_nav_ctrl =
              EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
                  [](auto) { return true; });
          auto evt_a11 = EventAsserter<vda5050pp::events::ActionPause>::forActionEvent(
              [](auto evt) { return evt->action_id == "a11"; });
          auto evt_a12 = EventAsserter<vda5050pp::events::ActionPause>::forActionEvent(
              [](auto evt) { return evt->action_id == "a12"; });

          scheduler.actionTransition("a12", vda5050pp::core::order::ActionTransition::isPaused());

          THEN("The no events are generated") {
            evt_order_status.assertNone();
            evt_nav_ctrl.assertNone();
            evt_a11.assertNone();
            evt_a12.assertNone();
          }
        }
        WHEN("lastly navigation pauses") {
          {
            auto evt_order_status =
                EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
                    [](auto) { return true; });
            auto evt_nav_ctrl =
                EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
                    [](auto) { return true; });
            auto evt_a11 = EventAsserter<vda5050pp::events::ActionPause>::forActionEvent(
                [](auto evt) { return evt->action_id == "a11"; });
            auto evt_a12 = EventAsserter<vda5050pp::events::ActionPause>::forActionEvent(
                [](auto evt) { return evt->action_id == "a12"; });

            scheduler.navigationTransition(
                vda5050pp::core::order::NavigationTransition::isPaused());

            assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_paused);
            THEN("No agv related events are generated") {
              evt_nav_ctrl.assertNone();
              evt_a11.assertNone();
              evt_a12.assertNone();
            }
          }
          WHEN("Resuming") {
            {
              auto evt_order_status =
                  EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
                      [](auto) { return true; });
              auto evt_nav_ctrl =
                  EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
                      [](auto) { return true; });
              auto evt_a11 = EventAsserter<vda5050pp::events::ActionResume>::forActionEvent(
                  [](auto evt) { return evt->action_id == "a11"; });
              auto evt_a12 = EventAsserter<vda5050pp::events::ActionResume>::forActionEvent(
                  [](auto evt) { return evt->action_id == "a12"; });

              scheduler.resume();

              assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_resuming);
              assertNavigationControl(evt_nav_ctrl,
                                      vda5050pp::events::NavigationControlType::k_resume);
              assertActionEvent<vda5050pp::events::ActionResume>(evt_a11, "a11");
              assertActionEvent<vda5050pp::events::ActionResume>(evt_a12, "a12");
            }
            WHEN("A11 is resumed") {
              {
                auto evt_order_status =
                    EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
                        [](auto) { return true; });
                auto evt_nav_ctrl =
                    EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
                        [](auto) { return true; });
                auto evt_a11 = EventAsserter<vda5050pp::events::ActionResume>::forActionEvent(
                    [](auto evt) { return evt->action_id == "a11"; });
                auto evt_a12 = EventAsserter<vda5050pp::events::ActionResume>::forActionEvent(
                    [](auto evt) { return evt->action_id == "a12"; });

                scheduler.actionTransition("a11",
                                           vda5050pp::core::order::ActionTransition::isRunning());

                THEN("No events were generated") {
                  evt_order_status.assertNone();
                  evt_nav_ctrl.assertNone();
                  evt_a11.assertNone();
                  evt_a12.assertNone();
                }
              }
              WHEN("A12 is resumed (to init)") {
                {
                  auto evt_order_status =
                      EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
                          [](auto) { return true; });
                  auto evt_nav_ctrl =
                      EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
                          [](auto) { return true; });
                  auto evt_a11 = EventAsserter<vda5050pp::events::ActionResume>::forActionEvent(
                      [](auto evt) { return evt->action_id == "a11"; });
                  auto evt_a12 = EventAsserter<vda5050pp::events::ActionResume>::forActionEvent(
                      [](auto evt) { return evt->action_id == "a12"; });

                  scheduler.actionTransition(
                      "a12", vda5050pp::core::order::ActionTransition::isInitializing());

                  THEN("No events were generated") {
                    evt_order_status.assertNone();
                    evt_nav_ctrl.assertNone();
                    evt_a11.assertNone();
                    evt_a12.assertNone();
                  }
                }
                WHEN("Lastly navigation is resumed") {
                  {
                    auto evt_order_status =
                        EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
                            [](auto) { return true; });
                    auto evt_nav_ctrl =
                        EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
                            [](auto) { return true; });
                    auto evt_a11 = EventAsserter<vda5050pp::events::ActionResume>::forActionEvent(
                        [](auto evt) { return evt->action_id == "a11"; });
                    auto evt_a12 = EventAsserter<vda5050pp::events::ActionResume>::forActionEvent(
                        [](auto evt) { return evt->action_id == "a12"; });

                    scheduler.navigationTransition(
                        vda5050pp::core::order::NavigationTransition::isResumed());

                    assertOrderStatus(evt_order_status,
                                      vda5050pp::misc::OrderStatus::k_order_active);

                    THEN("No agv events were generated") {
                      evt_nav_ctrl.assertNone();
                      evt_a11.assertNone();
                      evt_a12.assertNone();
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

TEST_CASE("core::order::Scheduler - cancel", "[core][order]") {
  vda5050pp::core::Instance::reset();
  vda5050pp::Config cfg;
  vda5050pp::config::EventManagerOptions opts;
  opts.synchronous_event_dispatch = true;
  cfg.refGlobalConfig().setEventManagerOptions(opts);
  cfg.refGlobalConfig().useWhiteList();
  vda5050pp::core::Instance::init(cfg);

  auto evt1 = std::make_shared<vda5050pp::core::events::YieldNavigationStepEvent>();
  evt1->goal_node = test::data::wrap_shared(test::data::mkNode("n2", 2, true, {}));
  evt1->via_edge = test::data::wrap_shared(test::data::mkEdge("e1", 1, true, {}));
  evt1->has_stop_at_goal_hint = false;

  auto evt1a = std::make_shared<vda5050pp::core::events::YieldActionGroupEvent>();
  evt1a->actions = {
      test::data::wrap_shared(test::data::mkAction("a11", "", vda5050::BlockingType::NONE)),
      test::data::wrap_shared(test::data::mkAction("a12", "", vda5050::BlockingType::NONE)),
  };
  evt1a->blocking_type_ceiling = vda5050::BlockingType::NONE;

  auto evt2 = std::make_shared<vda5050pp::core::events::YieldNavigationStepEvent>();
  evt2->goal_node = test::data::wrap_shared(test::data::mkNode("n4", 4, true, {}));
  evt2->via_edge = test::data::wrap_shared(test::data::mkEdge("e3", 3, true, {}));
  evt2->has_stop_at_goal_hint = false;

  vda5050pp::core::order::Scheduler scheduler;

  // Start
  {
    auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
        [](auto) { return true; });
    auto evt_navigation = EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
        [](auto) { return true; });
    auto evt_a11 = EventAsserter<vda5050pp::events::ActionStart>::forActionEvent(
        [](auto evt) { return evt->action_id == "a11"; });
    auto evt_a12 = EventAsserter<vda5050pp::events::ActionStart>::forActionEvent(
        [](auto evt) { return evt->action_id == "a12"; });

    scheduler.enqueue(evt1);
    scheduler.enqueue(evt1a);
    scheduler.enqueue(evt2);
    scheduler.update();

    assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_active);
    assertNavigation(evt_navigation, "n2", "e1");
    assertActionEvent<vda5050pp::events::ActionStart>(evt_a11, "a11");
    assertActionEvent<vda5050pp::events::ActionStart>(evt_a12, "a12");
  }

  WHEN("A cancel request is started") {
    {
      auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
          [](auto) { return true; });
      auto evt_navigation =
          EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
              [](auto) { return true; });
      auto evt_nav_ctrl = EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
          [](auto) { return true; });
      auto evt_a11 = EventAsserter<vda5050pp::events::ActionCancel>::forActionEvent(
          [](auto evt) { return evt->action_id == "a11"; });
      auto evt_a12 = EventAsserter<vda5050pp::events::ActionCancel>::forActionEvent(
          [](auto evt) { return evt->action_id == "a12"; });

      scheduler.cancel();

      assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_canceling);

      assertNavigationControl(evt_nav_ctrl, vda5050pp::events::NavigationControlType::k_cancel);
      assertActionEvent<vda5050pp::events::ActionCancel>(evt_a11, "a11");
      assertActionEvent<vda5050pp::events::ActionCancel>(evt_a12, "a12");
    }

    WHEN("The navigation finishes") {
      {
        auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
            [](auto) { return true; });
        auto evt_navigation =
            EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
                [](auto) { return true; });
        auto evt_nav_ctrl = EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
            [](auto) { return true; });

        scheduler.navigationTransition(vda5050pp::core::order::NavigationTransition::toSeqId(2));

        THEN("No further events were generated") {
          evt_order_status.assertNone();
          evt_navigation.assertNone();
          evt_nav_ctrl.assertNone();
        }
      }
      WHEN("The actions finish/fail") {
        {
          auto evt_order_status =
              EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
                  [](auto) { return true; });
          auto evt_navigation =
              EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
                  [](auto) { return true; });
          auto evt_nav_ctrl =
              EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
                  [](auto) { return true; });

          scheduler.actionTransition("a11", vda5050pp::core::order::ActionTransition::isFinished());
          scheduler.actionTransition("a12", vda5050pp::core::order::ActionTransition::isFailed());

          assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_idle);

          THEN("No further events were generated") {
            evt_navigation.assertNone();
            evt_nav_ctrl.assertNone();
          }
        }
      }
    }
    WHEN("The navigation fails") {
      {
        auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
            [](auto) { return true; });
        auto evt_navigation =
            EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
                [](auto) { return true; });
        auto evt_nav_ctrl = EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
            [](auto) { return true; });

        scheduler.navigationTransition(vda5050pp::core::order::NavigationTransition::isFailed());

        THEN("No further events were generated") {
          evt_order_status.assertNone();
          evt_navigation.assertNone();
          evt_nav_ctrl.assertNone();
        }
      }
    }
  }
}

TEST_CASE("core::order::Scheduler - interrupt", "[core][order]") {
  vda5050pp::core::Instance::reset();
  vda5050pp::Config cfg;
  vda5050pp::config::EventManagerOptions opts;
  opts.synchronous_event_dispatch = true;
  cfg.refGlobalConfig().setEventManagerOptions(opts);
  cfg.refGlobalConfig().useWhiteList();
  vda5050pp::core::Instance::init(cfg);

  auto evt_intr_hard = std::make_shared<vda5050pp::core::events::YieldInstantActionGroup>();
  evt_intr_hard->blocking_type_ceiling = vda5050::BlockingType::HARD;
  evt_intr_hard->instant_actions = {
      test::data::wrap_shared(test::data::mkAction("i1", "", vda5050::BlockingType::HARD)),
  };
  auto evt_intr_soft = std::make_shared<vda5050pp::core::events::YieldInstantActionGroup>();
  evt_intr_soft->blocking_type_ceiling = vda5050::BlockingType::SOFT;
  evt_intr_soft->instant_actions = {
      test::data::wrap_shared(test::data::mkAction("i2", "", vda5050::BlockingType::SOFT)),
  };
  auto evt_intr_none = std::make_shared<vda5050pp::core::events::YieldInstantActionGroup>();
  evt_intr_none->blocking_type_ceiling = vda5050::BlockingType::NONE;
  evt_intr_none->instant_actions = {
      test::data::wrap_shared(test::data::mkAction("i3", "", vda5050::BlockingType::NONE)),
  };

  auto evt1 = std::make_shared<vda5050pp::core::events::YieldNavigationStepEvent>();
  evt1->goal_node = test::data::wrap_shared(test::data::mkNode("n2", 2, true, {}));
  evt1->via_edge = test::data::wrap_shared(test::data::mkEdge("e1", 1, true, {}));
  evt1->has_stop_at_goal_hint = false;

  auto evt1a = std::make_shared<vda5050pp::core::events::YieldActionGroupEvent>();
  evt1a->actions = {
      test::data::wrap_shared(test::data::mkAction("a11", "", vda5050::BlockingType::NONE)),
  };
  evt1a->blocking_type_ceiling = vda5050::BlockingType::NONE;

  auto evt2 = std::make_shared<vda5050pp::core::events::YieldNavigationStepEvent>();
  evt2->goal_node = test::data::wrap_shared(test::data::mkNode("n4", 4, true, {}));
  evt2->via_edge = test::data::wrap_shared(test::data::mkEdge("e3", 3, true, {}));
  evt2->has_stop_at_goal_hint = false;

  vda5050pp::core::order::Scheduler scheduler;

  // Start
  {
    auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
        [](auto) { return true; });
    auto evt_navigation = EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
        [](auto) { return true; });
    auto evt_a11 = EventAsserter<vda5050pp::events::ActionStart>::forActionEvent(
        [](auto evt) { return evt->action_id == "a11"; });

    scheduler.enqueue(evt1);
    scheduler.enqueue(evt1a);
    scheduler.enqueue(evt2);
    scheduler.update();

    assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_active);
    assertNavigation(evt_navigation, "n2", "e1");
    assertActionEvent<vda5050pp::events::ActionStart>(evt_a11, "a11");
  }

  WHEN("Interrupting with a HARD action") {
    {
      auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
          [](auto) { return true; });
      auto evt_nav_ctrl = EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
          [](auto) { return true; });
      auto evt_a11 = EventAsserter<vda5050pp::events::ActionCancel>::forActionEvent(
          [](auto evt) { return evt->action_id == "a11"; });

      scheduler.enqueueInterruptActions(evt_intr_hard);

      assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_interrupting);
      assertNavigationControl(evt_nav_ctrl, vda5050pp::events::NavigationControlType::k_pause);
      assertActionEvent(evt_a11, "a11");
    }
    WHEN("actions and navigation pause/finish/failed") {
      {
        auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
            [](auto) { return true; });
        auto evt_navigation =
            EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
                [](auto) { return true; });
        auto evt_nav_ctrl = EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
            [](auto) { return true; });
        auto evt_i1 = EventAsserter<vda5050pp::events::ActionStart>::forActionEvent(
            [](auto evt) { return evt->action_id == "i1"; });

        scheduler.actionTransition("a11", vda5050pp::core::order::ActionTransition::isFinished());
        scheduler.navigationTransition(vda5050pp::core::order::NavigationTransition::isPaused());

        THEN("No nav events were generated") {
          evt_nav_ctrl.assertNone();
          evt_navigation.assertNone();
        }
        assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_active);
        assertActionEvent(evt_i1, "i1");
      }
      WHEN("i1 finishes") {
        {
          auto evt_order_status =
              EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
                  [](auto) { return true; });
          auto evt_nav_ctrl =
              EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
                  [](auto) { return true; });

          scheduler.actionTransition("i1", vda5050pp::core::order::ActionTransition::isFinished());
          evt_order_status.assertNone();

          assertNavigationControl(evt_nav_ctrl, vda5050pp::events::NavigationControlType::k_resume);
        }
        WHEN("Reaching n2 and n4") {
          {
            auto evt_order_status =
                EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
                    [](auto) { return true; });
            auto evt_nav_ctrl =
                EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
                    [](auto) { return true; });

            scheduler.navigationTransition(
                vda5050pp::core::order::NavigationTransition::isResumed());
            scheduler.navigationTransition(
                vda5050pp::core::order::NavigationTransition::toSeqId(2));
            scheduler.navigationTransition(
                vda5050pp::core::order::NavigationTransition::toSeqId(4));

            assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_idle);
            evt_nav_ctrl.assertNone();
          }
        }
      }
    }
  }

  WHEN("Interrupting with a SOFT action") {
    {
      auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
          [](auto) { return true; });
      auto evt_nav_ctrl = EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
          [](auto) { return true; });
      auto evt_a11 = EventAsserter<vda5050pp::events::ActionCancel>::forActionEvent(
          [](auto evt) { return evt->action_id == "a11"; });

      scheduler.enqueueInterruptActions(evt_intr_soft);

      assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_interrupting);
      assertNavigationControl(evt_nav_ctrl, vda5050pp::events::NavigationControlType::k_pause);
      evt_a11.assertNone();
    }
    WHEN("Navigation is finished") {
      {
        auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
            [](auto) { return true; });
        auto evt_nav_ctrl = EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
            [](auto) { return true; });
        auto evt_i2 = EventAsserter<vda5050pp::events::ActionStart>::forActionEvent(
            [](auto evt) { return evt->action_id == "i2"; });

        scheduler.navigationTransition(vda5050pp::core::order::NavigationTransition::toSeqId(2));

        assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_active);
        assertActionEvent(evt_i2, "i2");
      }
      WHEN("A11 and I2 are finished") {
        {
          auto evt_order_status =
              EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
                  [](auto) { return true; });
          auto evt_navigation =
              EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
                  [](auto) { return true; });

          scheduler.actionTransition("a11", vda5050pp::core::order::ActionTransition::isFinished());
          scheduler.actionTransition("i2", vda5050pp::core::order::ActionTransition::isFinished());

          evt_order_status.assertNone();
          assertNavigation(evt_navigation, "n4", "e3");
        }
      }
    }
  }

  WHEN("Interrupting with a NONE action") {
    {
      auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
          [](auto) { return true; });
      auto evt_nav_ctrl = EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
          [](auto) { return true; });
      auto evt_a11 = EventAsserter<vda5050pp::events::ActionCancel>::forActionEvent(
          [](auto evt) { return evt->action_id == "a11"; });
      auto evt_i3 = EventAsserter<vda5050pp::events::ActionStart>::forActionEvent(
          [](auto evt) { return evt->action_id == "i3"; });

      scheduler.enqueueInterruptActions(evt_intr_none);

      assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_interrupting);
      evt_nav_ctrl.assertNone();
      evt_a11.assertNone();
      assertActionEvent(evt_i3, "i3");
    }
    WHEN("Navigation is finished") {
      {
        auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
            [](auto) { return true; });
        auto evt_nav_ctrl = EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
            [](auto) { return true; });
        auto evt_navigation =
            EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
                [](auto) { return true; });

        scheduler.navigationTransition(vda5050pp::core::order::NavigationTransition::toSeqId(2));
        assertNavigation(evt_navigation, "n4", "e3");
        evt_nav_ctrl.assertNone();
      }
    }
  }
}

TEST_CASE("core::order::Scheduler - interrupt edge cases", "[core][order]") {
  vda5050pp::core::Instance::reset();
  vda5050pp::Config cfg;
  vda5050pp::config::EventManagerOptions opts;
  opts.synchronous_event_dispatch = true;
  cfg.refGlobalConfig().setEventManagerOptions(opts);
  cfg.refGlobalConfig().useWhiteList();
  vda5050pp::core::Instance::init(cfg);

  auto evt_intr_hard = std::make_shared<vda5050pp::core::events::YieldInstantActionGroup>();
  evt_intr_hard->blocking_type_ceiling = vda5050::BlockingType::HARD;
  evt_intr_hard->instant_actions = {
      test::data::wrap_shared(test::data::mkAction("i1", "", vda5050::BlockingType::HARD)),
  };

  auto evt_intr_none = std::make_shared<vda5050pp::core::events::YieldInstantActionGroup>();
  evt_intr_none->blocking_type_ceiling = vda5050::BlockingType::NONE;
  evt_intr_none->instant_actions = {
      test::data::wrap_shared(test::data::mkAction("i2", "", vda5050::BlockingType::NONE)),
  };

  auto evt1 = std::make_shared<vda5050pp::core::events::YieldNavigationStepEvent>();
  evt1->goal_node = test::data::wrap_shared(test::data::mkNode("n2", 2, true, {}));
  evt1->via_edge = test::data::wrap_shared(test::data::mkEdge("e1", 1, true, {}));
  evt1->has_stop_at_goal_hint = true;

  vda5050pp::core::order::Scheduler scheduler;

  WHEN("The scheduler receives a HARD instant action while beeing IDLE") {
    {
      auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
          [](auto) { return true; });
      auto evt_nav_ctrl = EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
          [](auto) { return true; });
      auto evt_nav = EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
          [](auto) { return true; });
      auto evt_i1 = EventAsserter<vda5050pp::events::ActionStart>::forActionEvent(
          [](auto evt) { return evt->action_id == "i1"; });

      scheduler.enqueueInterruptActions(evt_intr_hard);

      assertActionEvent(evt_i1, "i1");
      evt_nav.assertNone();
      evt_nav_ctrl.assertNone();
      assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_interrupting);
    }

    WHEN("The action finishes") {
      auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
          [](auto) { return true; });
      auto evt_nav_ctrl = EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
          [](auto) { return true; });
      auto evt_nav = EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
          [](auto) { return true; });

      scheduler.actionTransition("i1", vda5050pp::core::order::ActionTransition::isFinished());

      evt_nav.assertNone();
      evt_nav_ctrl.assertNone();
      assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_idle);
    }

    WHEN("A navigation step is received") {
      {
        auto evt_nav = EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
            [](auto) { return true; });

        scheduler.enqueue(evt1);
        scheduler.update();

        evt_nav.assertNone();
      }
      WHEN("I1 finishes") {
        {
          auto evt_nav = EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
              [](auto) { return true; });

          scheduler.actionTransition("i1", vda5050pp::core::order::ActionTransition::isFinished());

          assertNavigation(evt_nav, "n2", "e1");
        }
      }
    }
  }

  WHEN("The scheduler receives a NONE instant action while beeing IDLE") {
    {
      auto evt_order_status = EventAsserter<vda5050pp::core::events::OrderStatus>::forOrderEvent(
          [](auto) { return true; });
      auto evt_nav_ctrl = EventAsserter<vda5050pp::events::NavigationControl>::forNavigationEvent(
          [](auto) { return true; });
      auto evt_nav = EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
          [](auto) { return true; });
      auto evt_i2 = EventAsserter<vda5050pp::events::ActionStart>::forActionEvent(
          [](auto evt) { return evt->action_id == "i2"; });

      scheduler.enqueueInterruptActions(evt_intr_none);

      assertActionEvent(evt_i2, "i2");
      evt_nav.assertNone();
      evt_nav_ctrl.assertNone();
      assertOrderStatus(evt_order_status, vda5050pp::misc::OrderStatus::k_order_interrupting);
    }

    WHEN("A navigation step is received") {
      {
        auto evt_nav = EventAsserter<vda5050pp::events::NavigationNextNode>::forNavigationEvent(
            [](auto) { return true; });

        scheduler.enqueue(evt1);
        scheduler.update();

        assertNavigation(evt_nav, "n2", "e1");
      }
    }
  }
}