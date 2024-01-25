// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/observer/message_observer.h"

#include <catch2/catch.hpp>

#include "vda5050++/core/instance.h"

TEST_CASE("observer::MessageObserver - event handling", "[observer]") {
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().useWhiteList();
  cfg.refGlobalConfig().refEventManagerOptions().synchronous_event_dispatch = true;
  vda5050pp::core::Instance::reset();
  auto instance = vda5050pp::core::Instance::init(cfg).lock();

  vda5050pp::observer::MessageObserver observer;

  WHEN("No events were received yet") {
    THEN("The number of received order messages is 0") {
      REQUIRE(observer.getReceivedOrderMessages() == 0);
    }

    THEN("The number of received instant action messages is 0") {
      REQUIRE(observer.getReceivedInstantActionsMessages() == 0);
    }

    THEN("The number of sent factsheet messages is 0") {
      REQUIRE(observer.getSentFactsheetMessages() == 0);
    }

    THEN("The number of sent state messages is 0") {
      REQUIRE(observer.getSentStateMessages() == 0);
    }

    THEN("The number of sent visualization messages is 0") {
      REQUIRE(observer.getSentVisualizationMessages() == 0);
    }

    THEN("The number of errors is 0") { REQUIRE(observer.getErrors() == 0); }

    THEN("The last error message is empty") { REQUIRE_FALSE(observer.getLastError().has_value()); }

    THEN("The connection status is not known") {
      REQUIRE_FALSE(observer.getConnectionStatus().has_value());
    }
  }

  WHEN("An order message is received") {
    auto evt = std::make_shared<vda5050pp::core::events::ValidOrderMessageEvent>();
    evt->valid_order = std::make_shared<vda5050::Order>();

    std::shared_ptr<const vda5050::Order> cb_order;

    observer.onValidOrderMessage([&cb_order](auto order) { cb_order = order; });

    instance->getMessageEventManager().dispatch(evt);

    THEN("The number of received order messages is 1") {
      REQUIRE(observer.getReceivedOrderMessages() == 1);
    }

    THEN("The callback was called") { REQUIRE(cb_order == evt->valid_order); }
  }

  WHEN("An instant action message is received") {
    auto evt = std::make_shared<vda5050pp::core::events::ValidInstantActionMessageEvent>();
    evt->valid_instant_actions = std::make_shared<vda5050::InstantActions>();

    std::shared_ptr<const vda5050::InstantActions> cb_instant_actions;

    observer.onValidInstantActionsMessage(
        [&cb_instant_actions](auto instant_actions) { cb_instant_actions = instant_actions; });

    instance->getMessageEventManager().dispatch(evt);

    THEN("The number of received instant action messages is 1") {
      REQUIRE(observer.getReceivedInstantActionsMessages() == 1);
    }

    THEN("The callback was called") { REQUIRE(cb_instant_actions == evt->valid_instant_actions); }
  }

  WHEN("A factsheet message is sent") {
    auto evt = std::make_shared<vda5050pp::core::events::SendFactsheetMessageEvent>();
    evt->factsheet = std::make_shared<vda5050::AgvFactsheet>();

    std::shared_ptr<const vda5050::AgvFactsheet> cb_factsheet;

    observer.onSentFactsheetMessage([&cb_factsheet](auto factsheet) { cb_factsheet = factsheet; });

    instance->getMessageEventManager().dispatch(evt);

    THEN("The number of sent factsheet messages is 1") {
      REQUIRE(observer.getSentFactsheetMessages() == 1);
    }

    THEN("The callback was called") { REQUIRE(cb_factsheet == evt->factsheet); }
  }

  WHEN("A state message is sent") {
    auto evt = std::make_shared<vda5050pp::core::events::SendStateMessageEvent>();
    evt->state = std::make_shared<vda5050::State>();

    std::shared_ptr<const vda5050::State> cb_state;

    observer.onSentStateMessage([&cb_state](auto state) { cb_state = state; });

    instance->getMessageEventManager().dispatch(evt);

    THEN("The number of sent state messages is 1") {
      REQUIRE(observer.getSentStateMessages() == 1);
    }

    THEN("The callback was called") { REQUIRE(cb_state == evt->state); }
  }

  WHEN("A visualization message is sent") {
    auto evt = std::make_shared<vda5050pp::core::events::SendVisualizationMessageEvent>();
    evt->visualization = std::make_shared<vda5050::Visualization>();

    std::shared_ptr<const vda5050::Visualization> cb_visualization;

    observer.onSentVisualizationMessage(
        [&cb_visualization](auto visualization) { cb_visualization = visualization; });

    instance->getMessageEventManager().dispatch(evt);

    THEN("The number of sent visualization messages is 1") {
      REQUIRE(observer.getSentVisualizationMessages() == 1);
    }

    THEN("The callback was called") { REQUIRE(cb_visualization == evt->visualization); }
  }

  WHEN("An error message is received") {
    auto evt = std::make_shared<vda5050pp::core::events::MessageErrorEvent>();
    evt->error_type = vda5050pp::misc::MessageErrorType::k_json_deserialization;
    evt->description = "test";

    std::pair<vda5050pp::misc::MessageErrorType, std::string> cb_error;

    observer.onMessageError([&cb_error](auto error) { cb_error = error; });

    instance->getMessageEventManager().dispatch(evt);

    THEN("The number of errors is 1") { REQUIRE(observer.getErrors() == 1); }

    THEN("The callback was called") {
      auto [t, d] = cb_error;
      REQUIRE(t == evt->error_type);
      REQUIRE(d == evt->description);
    }
  }

  WHEN("A connection status message is received") {
    auto evt = std::make_shared<vda5050pp::core::events::ConnectionChangedEvent>();
    evt->status = vda5050pp::misc::ConnectionStatus::k_online;

    vda5050pp::misc::ConnectionStatus cb_status = vda5050pp::misc::ConnectionStatus::k_offline;

    observer.onConnectionChanged([&cb_status](auto status) { cb_status = status; });

    instance->getMessageEventManager().dispatch(evt);

    THEN("The connection status is correct") {
      REQUIRE(observer.getConnectionStatus() == vda5050pp::misc::ConnectionStatus::k_online);
    }

    THEN("The callback was called") {
      REQUIRE(cb_status == vda5050pp::misc::ConnectionStatus::k_online);
    }
  }
}