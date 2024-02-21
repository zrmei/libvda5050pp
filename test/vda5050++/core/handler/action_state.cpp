//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/agv_handler/action_state.h"

#include <catch2/catch_all.hpp>
#include <chrono>
#include <string_view>

#include "vda5050++/core/instance.h"

using namespace std::chrono_literals;

TEST_CASE("ActionState state propagation", "[core][ActionState]") {
  static constexpr std::string_view test_action_id_1 = "1uDHeAva";
  static constexpr std::string_view test_action_id_2 = "EtXJ8t45";
  static constexpr std::string_view test_result = "lKdjG3V+dKSo";

  auto test_action_1 = std::make_shared<vda5050::Action>();
  auto test_action_2 = std::make_shared<vda5050::Action>();
  test_action_1->actionId = test_action_id_1;
  test_action_2->actionId = test_action_id_2;

  vda5050pp::core::Instance::reset();
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().useWhiteList();
  vda5050pp::core::Instance::init(cfg);

  vda5050pp::core::agv_handler::ActionState action_state_1(test_action_1);
  vda5050pp::core::agv_handler::ActionState action_state_2(test_action_2);

  auto &status_manager = vda5050pp::core::Instance::ref().getActionStatusManager();

  WHEN("action states are set running") {
    auto subscriber = status_manager.getScopedActionStatusSubscriber();

    std::promise<void> notify1;
    std::promise<void> notify2;
    auto got1 = notify1.get_future();
    auto got2 = notify2.get_future();

    subscriber.subscribe(
        [&notify1, &notify2](std::shared_ptr<vda5050pp::events::ActionStatusRunning> d) {
          if (d->action_id == test_action_id_1) {
            notify1.set_value();
          }
          if (d->action_id == test_action_id_2) {
            notify2.set_value();
          }
        });

    action_state_1.setRunning();
    action_state_2.setRunning();

    THEN("The events were dispatched") {
      REQUIRE(got1.wait_for(100ms) == std::future_status::ready);
      REQUIRE(got2.wait_for(100ms) == std::future_status::ready);
    }
  }
  WHEN("action states are set paused") {
    auto subscriber = status_manager.getScopedActionStatusSubscriber();

    std::promise<void> notify1;
    std::promise<void> notify2;
    auto got1 = notify1.get_future();
    auto got2 = notify2.get_future();

    subscriber.subscribe(
        [&notify1, &notify2](std::shared_ptr<vda5050pp::events::ActionStatusPaused> d) {
          if (d->action_id == test_action_id_1) {
            notify1.set_value();
          }
          if (d->action_id == test_action_id_2) {
            notify2.set_value();
          }
        });

    action_state_1.setPaused();
    action_state_2.setPaused();

    THEN("The events were dispatched") {
      REQUIRE(got1.wait_for(100ms) == std::future_status::ready);
      REQUIRE(got2.wait_for(100ms) == std::future_status::ready);
    }
  }
  WHEN("action states are set finished") {
    auto subscriber = status_manager.getScopedActionStatusSubscriber();

    std::promise<std::optional<std::string>> notify1;
    std::promise<std::optional<std::string>> notify2;
    auto got1 = notify1.get_future();
    auto got2 = notify2.get_future();

    subscriber.subscribe(
        [&notify1, &notify2](std::shared_ptr<vda5050pp::events::ActionStatusFinished> d) {
          if (d->action_id == test_action_id_1) {
            notify1.set_value(d->action_result);
          }
          if (d->action_id == test_action_id_2) {
            notify2.set_value(d->action_result);
          }
        });

    action_state_1.setFinished();
    action_state_2.setFinished(test_result);

    THEN("The events were dispatched") {
      REQUIRE(got1.wait_for(100ms) == std::future_status::ready);
      REQUIRE(got2.wait_for(100ms) == std::future_status::ready);
      REQUIRE(got1.get() == std::nullopt);
      REQUIRE(got2.get() == test_result);
    }
  }
  WHEN("action states are set failed") {
    auto subscriber = status_manager.getScopedActionStatusSubscriber();

    std::promise<std::list<vda5050::Error>> notify1;
    std::promise<std::list<vda5050::Error>> notify2;
    auto got1 = notify1.get_future();
    auto got2 = notify2.get_future();

    subscriber.subscribe(
        [&notify1, &notify2](std::shared_ptr<vda5050pp::events::ActionStatusFailed> d) {
          if (d->action_id == test_action_id_1) {
            notify1.set_value(d->action_errors);
          }
          if (d->action_id == test_action_id_2) {
            notify2.set_value(d->action_errors);
          }
        });

    vda5050::Error error;
    error.errorType = test_result;

    action_state_1.setFailed();
    action_state_2.setFailed({error});

    THEN("The events were dispatched") {
      REQUIRE(got1.wait_for(100ms) == std::future_status::ready);
      REQUIRE(got2.wait_for(100ms) == std::future_status::ready);
      REQUIRE(got1.get().empty());
      REQUIRE(got2.get().begin()->errorType == test_result);
    }
  }
}