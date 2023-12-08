//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/interpreter/functional.h"

#include <spdlog/fmt/fmt.h>

#include <catch2/catch.hpp>

#include "test/data.h"
#include "vda5050++/core/instance.h"
#include "vda5050++/core/logger.h"
#include "vda5050++/exception.h"

inline std::shared_ptr<vda5050pp::core::events::YieldNavigationStepEvent> mkNavStepEvt(
    std::string_view node_id, std::string_view edge_id, uint32_t node_seq_id, bool stop_hint) {
  auto gn = std::make_shared<vda5050::Node>();
  gn->nodeId = node_id;
  gn->sequenceId = node_seq_id;

  auto ve = std::make_shared<vda5050::Edge>();
  ve->edgeId = edge_id;
  ve->sequenceId = node_seq_id - 1;

  auto ret = std::make_shared<vda5050pp::core::events::YieldNavigationStepEvent>();
  ret->goal_node = gn;
  ret->via_edge = ve;

  return ret;
}

static std::function<void(std::shared_ptr<vda5050pp::core::events::InterpreterEvent>)>
assertActionClearEvent() {
  return [](std::shared_ptr<vda5050pp::core::events::InterpreterEvent> event) {
    THEN("The Event is a YieldClearActions Event") {
      REQUIRE(event->getId() ==
              vda5050pp::core::events::InterpreterEventType::k_yield_clear_actions);
    }
  };
}

static std::function<void(std::shared_ptr<vda5050pp::core::events::InterpreterEvent>)>
assertActionGroupEvent(vda5050::BlockingType group_b_type,
                       std::initializer_list<std::string_view> ids) {
  return [group_b_type, ids](std::shared_ptr<vda5050pp::core::events::InterpreterEvent> event) {
    THEN("The Event is a YieldActionGroupEvent") {
      REQUIRE(event->getId() ==
              vda5050pp::core::events::InterpreterEventType::k_yield_action_group);
    }
    auto c_event = std::static_pointer_cast<vda5050pp::core::events::YieldActionGroupEvent>(event);

    THEN("The BlockingType matches") { REQUIRE(c_event->blocking_type_ceiling == group_b_type); }
    THEN("The Actions match") {
      REQUIRE(c_event->actions.size() == ids.size());
      auto it = c_event->actions.begin();
      for (const auto &id : ids) {
        THEN(fmt::format("Has Action with id {}", id)) { REQUIRE(it->get()->actionId == id); }
        it++;
      }
    }
  };
}

static std::function<void(std::shared_ptr<vda5050pp::core::events::InterpreterEvent>)>
assertNewActionEvent(std::string_view id) {
  return [id](std::shared_ptr<vda5050pp::core::events::InterpreterEvent> event) {
    THEN("The Event is a YieldNewActionEvent") {
      REQUIRE(event->getId() == vda5050pp::core::events::InterpreterEventType::k_yield_new_action);
    }
    auto c_event = std::static_pointer_cast<vda5050pp::core::events::YieldNewAction>(event);

    THEN(fmt::format("The Action ID is {}", id)) { REQUIRE(c_event->action->actionId == id); }
  };
}

static std::function<void(std::shared_ptr<vda5050pp::core::events::InterpreterEvent>)>
assertGraphExtensionEvent(uint32_t begin_seq, uint32_t end_seq) {
  return [begin_seq, end_seq](std::shared_ptr<vda5050pp::core::events::InterpreterEvent> event) {
    THEN("The Event is a YieldGraphExtensionEvent") {
      REQUIRE(event->getId() ==
              vda5050pp::core::events::InterpreterEventType::k_yield_graph_extension);
    }
    auto c_event = std::static_pointer_cast<vda5050pp::core::events::YieldGraphExtension>(event);
    THEN(fmt::format("The Sequence bounding is [{}, {}]", begin_seq, end_seq)) {
      auto [is_beg, is_end] = c_event->graph->bounds();
      REQUIRE(is_beg == begin_seq);
      REQUIRE(is_end == end_seq);
    }
  };
}

static std::function<void(std::shared_ptr<vda5050pp::core::events::InterpreterEvent>)>
assertGraphReplacementEvent(uint32_t base_beg, uint32_t base_end, uint32_t horz_beg,
                            uint32_t horz_end) {
  return [base_beg, base_end, horz_beg,
          horz_end](std::shared_ptr<vda5050pp::core::events::InterpreterEvent> event) {
    THEN("The Event is a YieldGraphReplacementEvent") {
      REQUIRE(event->getId() ==
              vda5050pp::core::events::InterpreterEventType::k_yield_graph_replacement);
    }
    auto c_event = std::static_pointer_cast<vda5050pp::core::events::YieldGraphReplacement>(event);
    THEN(fmt::format("The Base bounding is [{}, {}]", base_beg, base_end)) {
      auto [is_beg, is_end] = c_event->graph->baseBounds();
      REQUIRE(is_beg == base_beg);
      REQUIRE(is_end == base_end);
    }
    THEN(fmt::format("The Horizon bounding is [{}, {}]", horz_beg, horz_end)) {
      auto [is_beg, is_end] = c_event->graph->horizonBounds();
      REQUIRE(is_beg == horz_beg);
      REQUIRE(is_end == horz_end);
    }
  };
}

static std::function<void(std::shared_ptr<vda5050pp::core::events::InterpreterEvent>)>
assertGraphReplacementEvent(uint32_t base_beg, uint32_t base_end) {
  return [base_beg, base_end](std::shared_ptr<vda5050pp::core::events::InterpreterEvent> event) {
    THEN("The Event is a YieldGraphReplacementEvent") {
      REQUIRE(event->getId() ==
              vda5050pp::core::events::InterpreterEventType::k_yield_graph_replacement);
    }
    auto c_event = std::static_pointer_cast<vda5050pp::core::events::YieldGraphReplacement>(event);
    THEN(fmt::format("The Base bounding is [{}, {}]", base_beg, base_end)) {
      auto [is_beg, is_end] = c_event->graph->baseBounds();
      REQUIRE(is_beg == base_beg);
      REQUIRE(is_end == base_end);
    }
    THEN(fmt::format("The Horizon bounding is empty")) {
      REQUIRE_FALSE(c_event->graph->hasHorizon());
    }
  };
}

static std::function<void(std::shared_ptr<vda5050pp::core::events::InterpreterEvent>)>
assertNavigationStepEvent(std::string_view node_id, std::string_view edge_id, uint32_t node_seq_id,
                          bool stop_hint) {
  return [node_id, edge_id, node_seq_id,
          stop_hint](std::shared_ptr<vda5050pp::core::events::InterpreterEvent> event) {
    THEN("The Event is a YieldNavigationStepEvent") {
      REQUIRE(event->getId() ==
              vda5050pp::core::events::InterpreterEventType::k_yield_navigation_step);
    }
    auto c_event =
        std::static_pointer_cast<vda5050pp::core::events::YieldNavigationStepEvent>(event);

    THEN(fmt::format("Goal node is {}@{}", node_id, node_seq_id)) {
      REQUIRE(c_event->goal_node->nodeId == node_id);
      REQUIRE(c_event->goal_node->sequenceId == node_seq_id);
    }
    THEN(fmt::format("Via edge is {}@{}", edge_id, node_seq_id - 1)) {
      REQUIRE(c_event->via_edge->edgeId == edge_id);
      REQUIRE(c_event->via_edge->sequenceId == node_seq_id - 1);
    }
    THEN(fmt::format("The stop hint is {}", stop_hint)) {
      REQUIRE(c_event->has_stop_at_goal_hint == stop_hint);
    }
  };
}

static void testInterpreter(
    const vda5050::Order &order,
    const std::list<std::function<void(std::shared_ptr<vda5050pp::core::events::InterpreterEvent>)>>
        &assertions) {
  auto event_iter = vda5050pp::core::interpreter::EventIter::fromOrder(order);

  int counter = 0;
  for (const auto &assertion : assertions) {
    auto [event, it] = vda5050pp::core::interpreter::nextEvent(std::move(event_iter));
    event_iter = std::move(it);
    counter++;

    THEN(fmt::format("Event {} is present", counter)) { REQUIRE(event != nullptr); }
    THEN(fmt::format("Event {} satisfies assertions", counter)) { assertion(event); }
  }

  THEN("No events remain") {
    auto [evt, _] = vda5050pp::core::interpreter::nextEvent(std::move(event_iter));
    REQUIRE(evt == nullptr);
  }
}

TEST_CASE("core::interpreter - simple order with horizon, but without actions",
          "[core][interpreter]") {
  vda5050pp::core::Instance::reset();
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().useWhiteList();
  cfg.refGlobalConfig().setLogLevel(vda5050pp::config::LogLevel::k_debug);
  vda5050pp::core::Instance::init(cfg);
  auto test_order = test::data::mkTemplateOrder({
      test::data::TemplateElement{"n0", 0, true, {}},
      test::data::TemplateElement{"e0", 1, true, {}},
      test::data::TemplateElement{"n1", 2, true, {}},
      test::data::TemplateElement{"e1", 3, true, {}},
      test::data::TemplateElement{"n2", 4, true, {}},
      test::data::TemplateElement{"e2", 5, true, {}},
      test::data::TemplateElement{"n3", 6, true, {}},
      test::data::TemplateElement{"e3", 7, true, {}},
      test::data::TemplateElement{"n4", 8, true, {}},
      test::data::TemplateElement{"e4", 9, false, {}},
      test::data::TemplateElement{"n5", 10, false, {}},
  });
  auto invalid_order = test::data::mkTemplateOrder({
      test::data::TemplateElement{"n0", 0, true, {}},
      test::data::TemplateElement{"e0", 1, true, {}},
      test::data::TemplateElement{"n1", 2, true, {}},
      test::data::TemplateElement{"e1", 3, true, {}},
      test::data::TemplateElement{"n2", 4, false, {}},
  });
  test_order.orderId = "test_order_1";
  test_order.orderUpdateId = 0;

  std::list test_order_event_assertions{
      assertActionClearEvent(),
      assertNavigationStepEvent("n1", "e0", 2, false),
      assertNavigationStepEvent("n2", "e1", 4, false),
      assertNavigationStepEvent("n3", "e2", 6, false),
      assertNavigationStepEvent("n4", "e3", 8, false),
      assertGraphReplacementEvent(0, 8, 9, 10),
  };

  WHEN("Interpreting") {
    THEN("All events occur as expected") {
      testInterpreter(test_order, test_order_event_assertions);
    }
  }
  WHEN("Interpreting a order with an invalid horizon") {
    auto [_, it] = vda5050pp::core::interpreter::nextEvent(
        vda5050pp::core::interpreter::EventIter::fromOrder(invalid_order));
    auto exhaust_events = [](auto &&it) {
      std::shared_ptr<vda5050pp::core::events::InterpreterEvent> evt;
      do {
        auto [new_evt, new_it] = vda5050pp::core::interpreter::nextEvent(std::move(it));
        evt = new_evt;
        it = std::move(new_it);
      } while (evt != nullptr);
    };
    THEN("An exception is returned") {
      REQUIRE_THROWS_AS(exhaust_events(std::move(it)), vda5050pp::VDA5050PPInvalidArgument);
    }
  }
}

TEST_CASE("core::interpreter - complex order with only hard actions", "[core][interpreter]") {
  using namespace test::data;
  auto test_order = test::data::mkTemplateOrder({
      TemplateElement{"n0", 0, true, {mkAction("a1", "", vda5050::BlockingType::HARD)}},
      TemplateElement{"e0", 1, true, {}},
      TemplateElement{"n1", 2, true, {mkAction("a2", "", vda5050::BlockingType::HARD)}},
      TemplateElement{"e1", 3, true, {mkAction("a3", "", vda5050::BlockingType::HARD)}},
      TemplateElement{"n2", 4, true, {}},
      TemplateElement{"e2", 5, true, {}},
      TemplateElement{"n3",
                      6,
                      true,
                      {mkAction("a4", "", vda5050::BlockingType::HARD),
                       mkAction("a5", "", vda5050::BlockingType::HARD)}},
      TemplateElement{"e3", 7, true, {}},
      TemplateElement{"n4", 8, true, {}},
  });

  std::list test_order_event_assertions{
      assertActionClearEvent(),
      assertNewActionEvent("a1"),
      assertActionGroupEvent(vda5050::BlockingType::HARD, {"a1"}),
      assertNewActionEvent("a2"),
      assertNewActionEvent("a3"),
      assertNavigationStepEvent("n1", "e0", 2, true),
      assertActionGroupEvent(vda5050::BlockingType::HARD, {"a2"}),
      assertActionGroupEvent(vda5050::BlockingType::HARD, {"a3"}),
      assertNavigationStepEvent("n2", "e1", 4, false),
      assertNewActionEvent("a4"),
      assertNewActionEvent("a5"),
      assertNavigationStepEvent("n3", "e2", 6, true),
      assertActionGroupEvent(vda5050::BlockingType::HARD, {"a4"}),
      assertActionGroupEvent(vda5050::BlockingType::HARD, {"a5"}),
      assertNavigationStepEvent("n4", "e3", 8, false),
      assertGraphReplacementEvent(0, 8),
  };

  WHEN("Interpreting") {
    THEN("All events occur as expected") {
      testInterpreter(test_order, test_order_event_assertions);
    }
  }
}

TEST_CASE("core::interpreter - complex order with mixed actions", "[core][interpreter]") {
  using namespace test::data;
  auto test_order = test::data::mkTemplateOrder({
      TemplateElement{"n0",
                      0,
                      true,
                      {mkAction("a0", "", vda5050::BlockingType::SOFT),
                       mkAction("a1", "", vda5050::BlockingType::SOFT),
                       mkAction("a2", "", vda5050::BlockingType::HARD)}},
      TemplateElement{"e0", 1, true, {mkAction("a3", "", vda5050::BlockingType::NONE)}},
      TemplateElement{"n1", 2, true, {}},
      TemplateElement{"e1", 3, true, {mkAction("a4", "", vda5050::BlockingType::NONE)}},
      TemplateElement{"n2", 4, true, {mkAction("a5", "", vda5050::BlockingType::NONE)}},
      TemplateElement{"e2", 5, true, {mkAction("a6", "", vda5050::BlockingType::SOFT)}},
      TemplateElement{"n3", 6, true, {mkAction("a7", "", vda5050::BlockingType::NONE)}},
      TemplateElement{"e3", 7, true, {}},
      TemplateElement{"n4", 8, true, {}},
  });

  std::list test_order_event_assertions{
      assertActionClearEvent(),
      assertNewActionEvent("a0"),
      assertNewActionEvent("a1"),
      assertNewActionEvent("a2"),
      assertNewActionEvent("a3"),
      assertActionGroupEvent(vda5050::BlockingType::SOFT, {"a0", "a1"}),
      assertActionGroupEvent(vda5050::BlockingType::HARD, {"a2"}),
      assertActionGroupEvent(vda5050::BlockingType::NONE, {"a3"}),
      assertNewActionEvent("a4"),
      assertNavigationStepEvent("n1", "e0", 2, false),
      assertActionGroupEvent(vda5050::BlockingType::NONE, {"a4"}),
      assertNewActionEvent("a5"),
      assertNewActionEvent("a6"),
      assertNavigationStepEvent("n2", "e1", 4, true),
      assertActionGroupEvent(vda5050::BlockingType::NONE, {"a5"}),
      assertActionGroupEvent(vda5050::BlockingType::SOFT, {"a6"}),
      assertNewActionEvent("a7"),
      assertNavigationStepEvent("n3", "e2", 6, false),
      assertActionGroupEvent(vda5050::BlockingType::NONE, {"a7"}),
      assertNavigationStepEvent("n4", "e3", 8, false),
      assertGraphReplacementEvent(0, 8),
  };

  WHEN("Interpreting") {
    THEN("All events occur as expected") {
      testInterpreter(test_order, test_order_event_assertions);
    }
  }
}

TEST_CASE("core::interpreter - complex appending order with mixed actions", "[core][interpreter]") {
  using namespace test::data;
  auto test_order = test::data::mkTemplateOrder({
      TemplateElement{"n0", 4, true, {mkAction("a1", "", vda5050::BlockingType::HARD)}},
      TemplateElement{"e0", 5, true, {mkAction("a2", "", vda5050::BlockingType::SOFT)}},
      TemplateElement{"n1", 6, true, {}},
      TemplateElement{"e1", 7, true, {}},
      TemplateElement{"n2",
                      8,
                      true,
                      {mkAction("a3", "", vda5050::BlockingType::NONE),
                       mkAction("a4", "", vda5050::BlockingType::NONE)}},
      TemplateElement{"e2", 9, true, {}},
      TemplateElement{"n3", 10, true, {}},
      TemplateElement{"e3",
                      11,
                      true,
                      {
                          mkAction("a5", "", vda5050::BlockingType::SOFT),
                          mkAction("a6", "", vda5050::BlockingType::HARD),
                          mkAction("a7", "", vda5050::BlockingType::SOFT),
                      }},
      TemplateElement{"n4", 12, true, {}},
  });
  test_order.orderUpdateId = 1;  // set append flag

  std::list test_order_event_assertions{
      assertNewActionEvent("a2"),
      assertActionGroupEvent(vda5050::BlockingType::SOFT, {"a2"}),
      assertNavigationStepEvent("n1", "e0", 6, false),
      assertNewActionEvent("a3"),
      assertNewActionEvent("a4"),
      assertNavigationStepEvent("n2", "e1", 8, false),
      assertActionGroupEvent(vda5050::BlockingType::NONE, {"a3", "a4"}),
      assertNewActionEvent("a5"),
      assertNewActionEvent("a6"),
      assertNewActionEvent("a7"),
      assertNavigationStepEvent("n3", "e2", 10, true),
      assertActionGroupEvent(vda5050::BlockingType::SOFT, {"a5"}),
      assertActionGroupEvent(vda5050::BlockingType::HARD, {"a6"}),
      assertActionGroupEvent(vda5050::BlockingType::SOFT, {"a7"}),
      assertNavigationStepEvent("n4", "e3", 12, false),
      assertGraphExtensionEvent(4, 12),
  };

  WHEN("Interpreting") {
    THEN("All events occur as expected") {
      testInterpreter(test_order, test_order_event_assertions);
    }
  }
}

TEST_CASE("core::interpreter - with horizon", "[core][interpreter]") {
  using namespace test::data;
  auto test_order = test::data::mkTemplateOrder({
      TemplateElement{"n0",
                      0,
                      true,
                      {mkAction("a0", "", vda5050::BlockingType::SOFT),
                       mkAction("a1", "", vda5050::BlockingType::SOFT),
                       mkAction("a2", "", vda5050::BlockingType::HARD)}},
      TemplateElement{"e0", 1, true, {mkAction("a3", "", vda5050::BlockingType::NONE)}},
      TemplateElement{"n1", 2, true, {}},
      TemplateElement{"e1", 3, true, {mkAction("a4", "", vda5050::BlockingType::NONE)}},
      TemplateElement{"n2", 4, true, {mkAction("a5", "", vda5050::BlockingType::NONE)}},
      TemplateElement{"e2", 5, false, {mkAction("a6", "", vda5050::BlockingType::SOFT)}},
      TemplateElement{"n3", 6, false, {mkAction("a7", "", vda5050::BlockingType::NONE)}},
      TemplateElement{"e3", 7, false, {}},
      TemplateElement{"n4", 8, false, {}},
  });

  std::list test_order_event_assertions{
      assertActionClearEvent(),
      assertNewActionEvent("a0"),
      assertNewActionEvent("a1"),
      assertNewActionEvent("a2"),
      assertNewActionEvent("a3"),
      assertActionGroupEvent(vda5050::BlockingType::SOFT, {"a0", "a1"}),
      assertActionGroupEvent(vda5050::BlockingType::HARD, {"a2"}),
      assertActionGroupEvent(vda5050::BlockingType::NONE, {"a3"}),
      assertNewActionEvent("a4"),
      assertNavigationStepEvent("n1", "e0", 2, false),
      assertActionGroupEvent(vda5050::BlockingType::NONE, {"a4"}),
      assertNewActionEvent("a5"),
      assertNavigationStepEvent("n2", "e1", 4, false),
      assertActionGroupEvent(vda5050::BlockingType::NONE, {"a5"}),
      assertGraphReplacementEvent(0, 4, 5, 8),
  };

  WHEN("Interpreting") {
    THEN("All events occur as expected") {
      testInterpreter(test_order, test_order_event_assertions);
    }
  }
}

TEST_CASE("core::interpreter - edge case order with an action at the last node",
          "[core][interpreter]") {
  using namespace test::data;
  auto test_order = test::data::mkTemplateOrder({
      TemplateElement{"n0", 0, true, {}},
      TemplateElement{"e0", 1, true, {}},
      TemplateElement{"n1", 2, true, {}},
      TemplateElement{"e1", 3, true, {}},
      TemplateElement{"n2", 4, true, {}},
      TemplateElement{"e2", 5, true, {}},
      TemplateElement{"n3", 6, true, {}},
      TemplateElement{"e3", 7, true, {}},
      TemplateElement{"n4", 8, true, {test::data::mkAction("a1", "", vda5050::BlockingType::HARD)}},
  });

  std::list test_order_event_assertions{
      assertActionClearEvent(),
      assertNavigationStepEvent("n1", "e0", 2, false),
      assertNavigationStepEvent("n2", "e1", 4, false),
      assertNavigationStepEvent("n3", "e2", 6, false),
      assertNewActionEvent("a1"),
      assertNavigationStepEvent("n4", "e3", 8, true),
      assertActionGroupEvent(vda5050::BlockingType::HARD, {"a1"}),
      assertGraphReplacementEvent(0, 8),
  };

  WHEN("Interpreting") {
    THEN("All events occur as expected") {
      testInterpreter(test_order, test_order_event_assertions);
    }
  }
}