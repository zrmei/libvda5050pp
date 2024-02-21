//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/core/agv_handler/action_event_handler.h"

#include <catch2/catch_all.hpp>

#include "test/test_action_handler.h"
#include "vda5050++/core/common/container.h"
#include "vda5050++/core/instance.h"

using namespace std::chrono_literals;

TEST_CASE("ActionEventHandler event propagation", "[core][ActionEventHandler]") {
  vda5050pp::core::Instance::reset();
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().useWhiteList();
  cfg.refGlobalConfig().bwListModule(vda5050pp::core::module_keys::k_action_event_handler_key);
  vda5050pp::core::Instance::init(cfg);

  vda5050pp::agv_description::ActionDeclaration decl1;
  decl1.action_type = "type1";
  decl1.blocking_types = {vda5050::BlockingType::NONE};
  decl1.edge = true;
  decl1.node = true;
  decl1.instant = true;

  auto decl2 = decl1;
  decl2.action_type = "type2";

  auto test_handler_1 = std::make_shared<test::TestActionHandler>(decl1);
  auto test_handler_2 = std::make_shared<test::TestActionHandler>(decl2);

  vda5050pp::core::Instance::ref().addActionHandler(test_handler_1);
  vda5050pp::core::Instance::ref().addActionHandler(test_handler_2);

  WHEN("An action matching handler 1 is validated") {
    auto match1 = std::make_shared<vda5050::Action>();
    match1->actionType = decl1.action_type;
    match1->actionId = "ATx0vuIQe6Qg";
    match1->blockingType = vda5050::BlockingType::NONE;

    auto event = std::make_shared<vda5050pp::events::ActionValidate>();
    event->action = match1;
    auto future = event->getFuture();

    vda5050pp::core::Instance::ref().getActionEventManager().dispatch(event);

    REQUIRE(future.wait_for(1000ms) == std::future_status::ready);

    WHEN("An action matching handler 1 is prepared") {
      auto event1 = std::make_shared<vda5050pp::events::ActionPrepare>();
      event1->action = match1;

      vda5050pp::core::Instance::ref().getActionEventManager().dispatch(event1);

      THEN("Handler1.prepare was invoked") {
        auto future1 = test_handler_1->getPrepareFuture();
        REQUIRE(future1.wait_for(100ms) == std::future_status::ready);
        auto action_callbacks1 = future1.get();

        WHEN("An action matching handler 1 is started") {
          auto event2 = std::make_shared<vda5050pp::events::ActionStart>();
          event2->action_id = match1->actionId;

          vda5050pp::core::Instance::ref().getActionEventManager().dispatch(event2);

          THEN("only setOnStart called") {
            REQUIRE(action_callbacks1.on_start_called.wait_for(100ms) == std::future_status::ready);

            WHEN("An action matching handler 1 is paused") {
              auto event3 = std::make_shared<vda5050pp::events::ActionPause>();
              event3->action_id = match1->actionId;

              vda5050pp::core::Instance::ref().getActionEventManager().dispatch(event3);

              THEN("setOnPause was called") {
                REQUIRE(action_callbacks1.on_pause_called.wait_for(100ms) ==
                        std::future_status::ready);
              }
            }
            WHEN("An action matching handler 1 is resumed") {
              auto event4 = std::make_shared<vda5050pp::events::ActionResume>();
              event4->action_id = match1->actionId;

              vda5050pp::core::Instance::ref().getActionEventManager().dispatch(event4);

              THEN("setOnResume was called") {
                REQUIRE(action_callbacks1.on_resume_called.wait_for(100ms) ==
                        std::future_status::ready);
              }
            }
            WHEN("An action matching handler 1 is canceled") {
              auto event5 = std::make_shared<vda5050pp::events::ActionCancel>();
              event5->action_id = match1->actionId;

              vda5050pp::core::Instance::ref().getActionEventManager().dispatch(event5);

              THEN("setOnCancel was called") {
                REQUIRE(action_callbacks1.on_cancel_called.wait_for(100ms) ==
                        std::future_status::ready);
              }
            }
          }
        }
      }
      THEN("Handler2.prepare was not invoked") {
        auto future2 = test_handler_2->getPrepareFuture();
        REQUIRE(future2.wait_for(100ms) == std::future_status::timeout);
      }
    }
  }
}

TEST_CASE("ActionEventHandler validation", "[core][ActionEventHandler]") {
  vda5050pp::core::Instance::reset();
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().useWhiteList();
  cfg.refGlobalConfig().bwListModule(vda5050pp::core::module_keys::k_action_event_handler_key);
  vda5050pp::core::Instance::init(cfg);

  vda5050pp::agv_description::ActionDeclaration decl1;
  decl1.action_type = "type1";
  decl1.blocking_types = {vda5050::BlockingType::NONE};
  decl1.edge = true;
  decl1.node = true;
  decl1.instant = true;

  auto decl2 = decl1;
  decl2.action_type = "type2";

  auto test_handler_1 = std::make_shared<test::TestActionHandler>(decl1);
  auto test_handler_2 = std::make_shared<test::TestActionHandler>(decl2);

  vda5050pp::core::Instance::ref().addActionHandler(test_handler_1);
  vda5050pp::core::Instance::ref().addActionHandler(test_handler_2);

  WHEN("An unknown Action is validated") {
    auto unknown = std::make_shared<vda5050::Action>();
    unknown->actionId = "wvesf5HKOq2a";
    unknown->actionType = "unknown";

    auto event = std::make_shared<vda5050pp::events::ActionValidate>();
    event->action = unknown;

    auto future = event->getFuture();

    vda5050pp::core::Instance::ref().getActionEventManager().dispatch(event);

    THEN("The validation result will not be set by the ActionEventHandler") {
      REQUIRE(future.wait_for(100ms) == std::future_status::timeout);
    }
  }

  WHEN("An invalid blocking type action matching handler 1 is validated") {
    auto invalid1 = std::make_shared<vda5050::Action>();
    invalid1->actionId = "Xf4X4Mi1mtex";
    invalid1->actionType = decl1.action_type;
    invalid1->blockingType = vda5050::BlockingType::SOFT;

    auto event = std::make_shared<vda5050pp::events::ActionValidate>();
    event->action = invalid1;

    auto future = event->getFuture();

    vda5050pp::core::Instance::ref().getActionEventManager().dispatch(event);

    THEN("The validation results contains errors") {
      REQUIRE(future.wait_for(100ms) == std::future_status::ready);
      REQUIRE_FALSE(future.get().empty());
    }
  }

  WHEN("An invalid blocking type action matching handler 1 is validated") {
    auto invalid1 = std::make_shared<vda5050::Action>();
    invalid1->actionId = "Xf4X4Mi1mtex";
    invalid1->actionType = decl1.action_type;
    invalid1->blockingType = vda5050::BlockingType::SOFT;

    auto event = std::make_shared<vda5050pp::events::ActionValidate>();
    event->action = invalid1;

    auto future = event->getFuture();

    vda5050pp::core::Instance::ref().getActionEventManager().dispatch(event);

    THEN("The validation results contains errors") {
      REQUIRE(future.wait_for(100ms) == std::future_status::ready);
      REQUIRE_FALSE(future.get().empty());
    }
  }

  WHEN("A valid action matching handler 2 is validated") {
    auto valid2 = std::make_shared<vda5050::Action>();
    valid2->actionId = "ULtiVnmYNNvP";
    valid2->actionType = decl2.action_type;
    valid2->blockingType = vda5050::BlockingType::NONE;

    auto event = std::make_shared<vda5050pp::events::ActionValidate>();
    event->action = valid2;

    auto future = event->getFuture();

    vda5050pp::core::Instance::ref().getActionEventManager().dispatch(event);

    THEN("The validation results contains no errors") {
      REQUIRE(future.wait_for(100ms) == std::future_status::ready);
      REQUIRE(future.get().empty());
    }
  }
}