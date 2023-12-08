//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/order/action_task.h"

#include <spdlog/fmt/fmt.h>

#include <array>
#include <catch2/catch.hpp>
#include <future>

#include "test/data.h"
#include "vda5050++/core/instance.h"

// MSVC cannot handle constexpr arrays.
static std::array<std::function<void(vda5050pp::core::order::ActionTask &)>, 0> k_no_assertions;

template <typename Container>
void runAssertions(vda5050pp::core::order::ActionTask &task, const Container &assertions) {
  for (const auto &a : assertions) {
    a(task);
  }
}

template <typename Container>
std::function<void(vda5050pp::core::order::ActionTask &)> assertValid(
    vda5050pp::core::order::ActionTransition transition, const Container &and_then) {
  return [transition, and_then](auto &task) {
    WHEN(fmt::format("Transitioning with {}", transition)) {
      THEN("it succeeds") {
        REQUIRE_NOTHROW(task.transition(transition));
        runAssertions(task, and_then);
      }
    }
  };
}

static std::function<void(vda5050pp::core::order::ActionTask &)> assertInvalid(
    vda5050pp::core::order::ActionTransition transition) {
  return [transition](auto &task) {
    WHEN(fmt::format("Transitioning with {}", transition)) {
      THEN("it fails") {
        REQUIRE_THROWS_AS(task.transition(transition), vda5050pp::VDA5050PPInvalidState);
      }
    }
  };
}

static std::function<void(vda5050pp::core::order::ActionTask &)> assertTerminal(bool terminal) {
  return [terminal](const auto &task) {
    if (terminal) {
      THEN("Task is terminal") { REQUIRE(task.isTerminal()); }
    } else {
      THEN("Task is not terminal") { REQUIRE_FALSE(task.isTerminal()); }
    }
  };
}

static std::function<void(vda5050pp::core::order::ActionTask &)> assertPaused(bool paused) {
  return [paused](const auto &task) {
    if (paused) {
      THEN("Task is paused") { REQUIRE(task.isPaused()); }
    } else {
      THEN("Task is not paused") { REQUIRE_FALSE(task.isPaused()); }
    }
  };
}

template <typename Event>
std::function<void(vda5050pp::core::order::ActionTask &)> assertActionEvent(
    std::string_view action_id) {
  auto sub = std::make_shared<vda5050pp::core::ScopedActionEventSubscriber>(
      vda5050pp::core::Instance::ref().getActionEventManager().getScopedActionEventSubscriber());

  auto promise = std::make_shared<std::promise<void>>();
  auto future = std::make_shared<std::future<void>>(promise->get_future());

  sub->subscribe([promise, action_id](std::shared_ptr<Event> evt) mutable {
    if (action_id == evt->action_id) {
      promise->set_value();
    }
  });

  return [sub, future](const auto &) {
    THEN(fmt::format("An {} event was received", typeid(Event).name())) {
      using namespace std::chrono_literals;
      REQUIRE(future->wait_for(100ms) == std::future_status::ready);
    }
  };
}

static std::function<void(vda5050pp::core::order::ActionTask &)> assertActionStatusEvent(
    std::string_view action_id, vda5050::ActionStatus status) {
  auto sub = std::make_shared<
      vda5050pp::core::GenericEventManager<vda5050pp::core::events::OrderEvent>::ScopedSubscriber>(
      vda5050pp::core::Instance::ref().getOrderEventManager().getScopedSubscriber());

  auto promise = std::make_shared<std::promise<void>>();
  auto future = std::make_shared<std::future<void>>(promise->get_future());

  sub->subscribe<vda5050pp::core::events::OrderActionStatusChanged>(
      [promise, action_id, status](auto evt) {
        if (action_id == evt->action_id && status == evt->action_status) {
          promise->set_value();
        }
      });

  return [sub, future, status](const auto &) {
    THEN(fmt::format("An OrderActionStatus with status={} event was received", (int)status)) {
      using namespace std::chrono_literals;
      REQUIRE(future->wait_for(100ms) == std::future_status::ready);
    }
  };
}

TEST_CASE("core::order::ActionTask - transition", "[core][order]") {
  using Transition = vda5050pp::core::order::ActionTransition;

  vda5050pp::core::Instance::reset();
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().useWhiteList();
  vda5050pp::core::Instance::init(cfg);

  auto action = std::make_shared<vda5050::Action>(
      test::data::mkAction("testId", "testType", vda5050::BlockingType::HARD));

  vda5050pp::core::order::ActionTask task(action);

  std::array assertions_resuming{
      assertInvalid(Transition::isFinished()),
      assertValid(Transition::isFailed(), k_no_assertions),
      assertValid(Transition::isRunning(), k_no_assertions),
      assertInvalid(Transition::isPaused()),
      assertInvalid(Transition::doStart()),
      assertInvalid(Transition::doResume()),
      assertValid(Transition::isInitializing(), k_no_assertions),
      assertInvalid(Transition::doPause()),
      assertValid(Transition::doCancel(), k_no_assertions),
      assertPaused(true),
      assertTerminal(false),
      assertActionEvent<vda5050pp::events::ActionResume>(action->actionId),
  };

  std::array assertions_paused{
      assertInvalid(Transition::isFinished()),
      assertValid(Transition::isFailed(), k_no_assertions),
      assertValid(Transition::isRunning(), k_no_assertions),
      assertInvalid(Transition::isPaused()),
      assertInvalid(Transition::doStart()),
      assertValid(Transition::doResume(), assertions_resuming),
      assertValid(Transition::isInitializing(), k_no_assertions),
      assertInvalid(Transition::doPause()),
      assertValid(Transition::doCancel(), k_no_assertions),
      assertPaused(true),
      assertTerminal(false),
      assertActionStatusEvent(action->actionId, vda5050::ActionStatus::PAUSED),
  };

  std::array assertions_pausing{
      assertInvalid(Transition::isFinished()),
      assertValid(Transition::isFailed(), k_no_assertions),
      assertInvalid(Transition::isRunning()),
      assertValid(Transition::isPaused(), assertions_paused),
      assertInvalid(Transition::doStart()),
      assertInvalid(Transition::doResume()),
      assertInvalid(Transition::isInitializing()),
      assertInvalid(Transition::doPause()),
      assertValid(Transition::doCancel(), k_no_assertions),
      assertPaused(false),
      assertTerminal(false),
      assertActionEvent<vda5050pp::events::ActionPause>(action->actionId),
  };

  std::array assertions_running{
      assertValid(Transition::doCancel(), k_no_assertions),
      assertValid(Transition::doPause(), assertions_pausing),
      assertValid(Transition::isFinished(), k_no_assertions),
      assertValid(Transition::isFailed(), k_no_assertions),
      assertInvalid(Transition::doStart()),
      assertInvalid(Transition::isRunning()),
      assertInvalid(Transition::doResume()),
      assertInvalid(Transition::isInitializing()),
      assertInvalid(Transition::isPaused()),
      assertPaused(false),
      assertTerminal(false),
      assertActionStatusEvent(action->actionId, vda5050::ActionStatus::RUNNING),
  };

  std::array assertions_initializing{
      assertValid(Transition::isRunning(), assertions_running),
      assertValid(Transition::isFailed(), k_no_assertions),
      assertValid(Transition::doCancel(), k_no_assertions),
      assertValid(Transition::isFinished(), k_no_assertions),
      assertValid(Transition::isPaused(), assertions_paused),
      assertInvalid(Transition::doStart()),
      assertValid(Transition::doPause(), assertions_pausing),
      assertInvalid(Transition::doResume()),
      assertValid(Transition::isInitializing(), k_no_assertions),
      assertPaused(false),
      assertTerminal(false),
      assertActionStatusEvent(action->actionId, vda5050::ActionStatus::INITIALIZING),
      assertActionEvent<vda5050pp::events::ActionStart>(action->actionId),
  };

  std::array assertions_finished{
      assertInvalid(Transition::isFinished()),
      assertInvalid(Transition::isFailed()),
      assertInvalid(Transition::isRunning()),
      assertInvalid(Transition::isPaused()),
      assertInvalid(Transition::doStart()),
      assertInvalid(Transition::doResume()),
      assertInvalid(Transition::isInitializing()),
      assertInvalid(Transition::doPause()),
      assertInvalid(Transition::doCancel()),
      assertPaused(false),
      assertTerminal(true),
      assertActionStatusEvent(action->actionId, vda5050::ActionStatus::FINISHED),
  };

  std::array assertions_failed{
      assertInvalid(Transition::isFinished()),
      assertInvalid(Transition::isFailed()),
      assertInvalid(Transition::isRunning()),
      assertInvalid(Transition::isPaused()),
      assertInvalid(Transition::doStart()),
      assertInvalid(Transition::doResume()),
      assertInvalid(Transition::isInitializing()),
      assertInvalid(Transition::doPause()),
      assertInvalid(Transition::doCancel()),
      assertPaused(false),
      assertTerminal(true),
      assertActionStatusEvent(action->actionId, vda5050::ActionStatus::FAILED),
  };

  std::array assertions_canceling{
      assertValid(Transition::isFinished(), assertions_finished),
      assertValid(Transition::isFailed(), assertions_failed),
      assertInvalid(Transition::isRunning()),
      assertInvalid(Transition::isPaused()),
      assertInvalid(Transition::doStart()),
      assertInvalid(Transition::doResume()),
      assertInvalid(Transition::isInitializing()),
      assertInvalid(Transition::doPause()),
      assertInvalid(Transition::doCancel()),
      assertPaused(false),
      assertTerminal(false),
      assertActionEvent<vda5050pp::events::ActionCancel>(action->actionId),
  };

  std::array assertions_waiting{
      assertValid(Transition::doCancel(), assertions_canceling),
      assertValid(Transition::doStart(), assertions_initializing),
      assertInvalid(Transition::isFailed()),
      assertInvalid(Transition::isFinished()),
      assertInvalid(Transition::isPaused()),
      assertInvalid(Transition::isRunning()),
      assertInvalid(Transition::isInitializing()),
      assertInvalid(Transition::doPause()),
      assertInvalid(Transition::doResume()),
      assertPaused(false),
      assertTerminal(false),
  };

  runAssertions(task, assertions_waiting);
}