//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/order/navigation_task.h"

#include <array>
#include <catch2/catch.hpp>

#include "vda5050++/core/instance.h"
#include "vda5050++/exception.h"

// MSVC cannot handle constexpr arrays.
static std::array<std::function<void(vda5050pp::core::order::NavigationTask &)>, 0> k_no_assertions;

template <typename Container>
void runAssertions(vda5050pp::core::order::NavigationTask &task, const Container &assertions) {
  for (const auto &a : assertions) {
    a(task);
  }
}

template <typename Container>
std::function<void(vda5050pp::core::order::NavigationTask &)> assertValid(
    vda5050pp::core::order::NavigationTransition transition, const Container &and_then) {
  return [transition, and_then](auto &task) {
    THEN(fmt::format("Transitioning with {} succeeds", transition)) {
      REQUIRE_NOTHROW(task.transition(transition));
      runAssertions(task, and_then);
    }
  };
}

static std::function<void(vda5050pp::core::order::NavigationTask &)> assertInvalid(
    vda5050pp::core::order::NavigationTransition transition) {
  return [transition](auto &task) {
    THEN(fmt::format("Transitioning with {} fails", transition)) {
      REQUIRE_THROWS_AS(task.transition(transition), vda5050pp::VDA5050PPInvalidState);
    }
  };
}

static std::function<void(vda5050pp::core::order::NavigationTask &)> assertTerminal(bool terminal) {
  return [terminal](auto &task) {
    if (terminal) {
      THEN("Task is terminal") { REQUIRE(task.isTerminal()); }
    } else {
      THEN("Task is not terminal") { REQUIRE_FALSE(task.isTerminal()); }
    }
  };
}

static std::function<void(vda5050pp::core::order::NavigationTask &)> assertPaused(bool paused) {
  return [paused](auto &task) {
    if (paused) {
      THEN("Task is paused") { REQUIRE(task.isPaused()); }
    } else {
      THEN("Task is not paused") { REQUIRE_FALSE(task.isPaused()); }
    }
  };
}

static std::function<void(vda5050pp::core::order::NavigationTask &)> assertNavigationControlEvent(
    vda5050pp::events::NavigationControlType type) {
  auto sub = std::make_shared<vda5050pp::core::ScopedNavigationEventSubscriber>(
      vda5050pp::core::Instance::ref()
          .getNavigationEventManager()
          .getScopedNavigationEventSubscriber());

  auto promise = std::make_shared<std::promise<void>>();
  auto future = std::make_shared<std::future<void>>(promise->get_future());

  sub->subscribe([promise, type](std::shared_ptr<vda5050pp::events::NavigationControl> evt) {
    if (type == evt->type) {
      promise->set_value();
    }
  });

  return [sub, future, type](const auto &) {
    THEN(fmt::format("An NavigationControlEvent with type={} event was received", (int)type)) {
      using namespace std::chrono_literals;
      REQUIRE(future->wait_for(100ms) == std::future_status::ready);
    }
  };
}

TEST_CASE("core::order::NavigationTask - transition", "[core][order]") {
  vda5050pp::core::Instance::reset();
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().useWhiteList();
  vda5050pp::core::Instance::init(cfg);

  auto goal_node = std::make_shared<vda5050::Node>();
  goal_node->sequenceId = 4;
  auto via_edge = std::make_shared<vda5050::Edge>();

  vda5050pp::core::order::NavigationTask task(goal_node, via_edge);

  using Transition = vda5050pp::core::order::NavigationTransition;

  std::array assertions_failed{
      assertInvalid(Transition::doStart()),
      assertInvalid(Transition::doPause()),
      assertInvalid(Transition::doResume()),
      assertInvalid(Transition::doCancel()),
      assertInvalid(Transition::isResumed()),
      assertInvalid(Transition::isPaused()),
      assertInvalid(Transition::isFailed()),
      assertInvalid(Transition::toSeqId(goal_node->sequenceId)),
      assertTerminal(true),
      assertPaused(false),
  };

  std::array assertions_done{
      assertInvalid(Transition::doStart()),
      assertInvalid(Transition::doPause()),
      assertInvalid(Transition::doResume()),
      assertInvalid(Transition::doCancel()),
      assertInvalid(Transition::isResumed()),
      assertInvalid(Transition::isPaused()),
      assertInvalid(Transition::isFailed()),
      assertInvalid(Transition::toSeqId(goal_node->sequenceId)),
      assertTerminal(true),
      assertPaused(false),
  };

  std::array assertions_in_progress{
      assertInvalid(Transition::doStart()),
      assertValid(Transition::doPause(), k_no_assertions),
      assertInvalid(Transition::doResume()),
      assertValid(Transition::doCancel(), k_no_assertions),
      assertInvalid(Transition::isResumed()),
      assertValid(Transition::isPaused(), k_no_assertions),
      assertValid(Transition::isFailed(), assertions_failed),
      assertValid(Transition::toSeqId(goal_node->sequenceId), assertions_done),
      assertInvalid(Transition::toSeqId(goal_node->sequenceId - 1)),
      assertInvalid(Transition::toSeqId(goal_node->sequenceId + 1)),
      assertTerminal(false),
      assertPaused(false),
  };

  std::array assertions_resuming{
      assertInvalid(Transition::doStart()),
      assertInvalid(Transition::doPause()),
      assertInvalid(Transition::doResume()),
      assertValid(Transition::doCancel(), k_no_assertions),
      assertValid(Transition::isResumed(), assertions_in_progress),
      assertValid(Transition::isPaused(), k_no_assertions),
      assertValid(Transition::isFailed(), assertions_failed),
      assertInvalid(Transition::toSeqId(goal_node->sequenceId)),
      assertTerminal(false),
      assertPaused(true),
      assertNavigationControlEvent(vda5050pp::events::NavigationControlType::k_resume),
  };

  std::array assertions_paused{
      assertInvalid(Transition::doStart()),
      assertInvalid(Transition::doPause()),
      assertValid(Transition::doResume(), assertions_resuming),
      assertValid(Transition::doCancel(), k_no_assertions),
      assertValid(Transition::isResumed(), k_no_assertions),
      assertInvalid(Transition::isPaused()),
      assertValid(Transition::isFailed(), assertions_failed),
      assertInvalid(Transition::toSeqId(goal_node->sequenceId)),
      assertTerminal(false),
      assertPaused(true),
  };

  std::array assertions_canceling{
      assertInvalid(Transition::doStart()),
      assertInvalid(Transition::doPause()),
      assertInvalid(Transition::doResume()),
      assertInvalid(Transition::doCancel()),
      assertInvalid(Transition::isResumed()),
      assertInvalid(Transition::isPaused()),
      assertValid(Transition::isFailed(), assertions_failed),
      assertValid(Transition::toSeqId(goal_node->sequenceId), assertions_done),
      assertTerminal(false),
      assertPaused(false),
      assertNavigationControlEvent(vda5050pp::events::NavigationControlType::k_cancel),
  };

  std::array assertions_pausing{
      assertInvalid(Transition::doStart()),
      assertInvalid(Transition::doPause()),
      assertInvalid(Transition::doResume()),
      assertValid(Transition::doCancel(), assertions_canceling),
      assertInvalid(Transition::isResumed()),
      assertValid(Transition::isPaused(), assertions_paused),
      assertValid(Transition::isFailed(), assertions_failed),
      assertValid(Transition::toSeqId(goal_node->sequenceId), assertions_done),
      assertTerminal(false),
      assertPaused(false),
      assertNavigationControlEvent(vda5050pp::events::NavigationControlType::k_pause),
  };

  std::array assertions_first_in_progress{
      assertInvalid(Transition::doStart()),
      assertValid(Transition::doPause(), assertions_pausing),
      assertInvalid(Transition::doResume()),
      assertValid(Transition::doCancel(), assertions_canceling),
      assertInvalid(Transition::isResumed()),
      assertValid(Transition::isPaused(), k_no_assertions),
      assertValid(Transition::isFailed(), assertions_failed),
      assertValid(Transition::toSeqId(goal_node->sequenceId), k_no_assertions),
      assertInvalid(Transition::toSeqId(goal_node->sequenceId - 1)),
      assertInvalid(Transition::toSeqId(goal_node->sequenceId + 1)),
      assertTerminal(false),
      assertPaused(false),
  };

  std::array assertions_waiting{
      assertValid(Transition::doStart(), assertions_first_in_progress),
      assertInvalid(Transition::doPause()),
      assertInvalid(Transition::doResume()),
      assertValid(Transition::doCancel(), assertions_canceling),
      assertInvalid(Transition::isResumed()),
      assertInvalid(Transition::isPaused()),
      assertValid(Transition::isFailed(), assertions_failed),
      assertInvalid(Transition::toSeqId(goal_node->sequenceId)),
      assertTerminal(false),
      assertPaused(false),
  };

  runAssertions(task, assertions_waiting);
}
