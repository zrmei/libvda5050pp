//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/checks/action.h"

#include <spdlog/fmt/fmt.h>

#include <catch2/catch.hpp>

#include "vda5050++/core/common/formatters.h"
#include "vda5050++/core/instance.h"

TEST_CASE("core::checks::matchActionType", "[core][checks]") {
  vda5050pp::agv_description::ActionDeclaration decl1;
  decl1.action_type = "type1";

  vda5050pp::agv_description::ActionDeclaration decl2;
  decl2.action_type = "type2";

  vda5050::Action action1;
  action1.actionType = "type1";
  vda5050::Action action2;
  action2.actionType = "type2";
  vda5050::Action action3;
  action3.actionType = "type3";

  WHEN("An action matches the declaration") {
    THEN("True is returned") {
      REQUIRE(vda5050pp::core::checks::matchActionType(decl1, action1));
      REQUIRE(vda5050pp::core::checks::matchActionType(decl2, action2));
      REQUIRE_FALSE(vda5050pp::core::checks::matchActionType(decl1, action3));
      REQUIRE_FALSE(vda5050pp::core::checks::matchActionType(decl2, action3));
      REQUIRE_FALSE(vda5050pp::core::checks::matchActionType(decl1, action2));
      REQUIRE_FALSE(vda5050pp::core::checks::matchActionType(decl2, action1));
    }
  }
}

TEST_CASE("core::checks::validateActionWithDeclaration", "[core][checks]") {
  vda5050pp::agv_description::ParameterRange param_int;
  param_int.key = "int";
  param_int.type = vda5050pp::agv_description::ParameterValueType::k_integer;

  vda5050pp::agv_description::ParameterRange param_float01;
  param_float01.key = "float01";
  param_float01.type = vda5050pp::agv_description::ParameterValueType::k_float;
  param_float01.ordinal_min = "0";
  param_float01.ordinal_max = "1";

  vda5050pp::agv_description::ParameterRange param_bool;
  param_bool.key = "bool";
  param_bool.type = vda5050pp::agv_description::ParameterValueType::k_boolean;

  vda5050pp::agv_description::ParameterRange param_str;
  param_str.key = "str";
  param_str.type = vda5050pp::agv_description::ParameterValueType::k_string;
  param_str.value_set = {{"v1", "v2", "v3"}};

  vda5050pp::agv_description::ParameterRange param_custom;
  param_custom.key = "custom";
  param_custom.type = vda5050pp::agv_description::ParameterValueType::k_custom;

  vda5050pp::agv_description::ActionDeclaration decl1;
  decl1.action_type = "type1";
  decl1.blocking_types = {vda5050::BlockingType::HARD, vda5050::BlockingType::SOFT};
  decl1.parameter = {param_int, param_float01};
  decl1.optional_parameter = {param_str, param_bool};

  WHEN("An action with only required parameters is checked") {
    vda5050::Action a1;
    a1.actionType = "type1";
    a1.blockingType = vda5050::BlockingType::HARD;
    a1.actionParameters = {{{"int", "-1999"}, {"float01", "0.25"}}};

    vda5050pp::handler::ParametersMap parameters;
    auto err = vda5050pp::core::checks::validateActionWithDeclaration(
        a1, vda5050pp::misc::ActionContext::k_unspecified, decl1, parameters);

    THEN("No error is returned") { REQUIRE(err.empty()); }

    THEN("All parameters were parsed") {
      REQUIRE(std::get<int64_t>(parameters.at("int")) == -1999);
      REQUIRE(std::get<double>(parameters.at("float01")) == 0.25);
    }
  }

  WHEN("An action with optional parameters is checked") {
    vda5050::Action a1;
    a1.actionType = "type1";
    a1.blockingType = vda5050::BlockingType::SOFT;
    a1.actionParameters = {
        {{"int", "-1999"}, {"float01", "0.25"}, {"str", "v2"}, {"bool", "true"}}};

    vda5050pp::handler::ParametersMap parameters;
    auto err = vda5050pp::core::checks::validateActionWithDeclaration(
        a1, vda5050pp::misc::ActionContext::k_unspecified, decl1, parameters);

    THEN("No error is returned") { REQUIRE(err.empty()); }

    THEN("All parameters were parsed") {
      REQUIRE(std::get<int64_t>(parameters.at("int")) == -1999);
      REQUIRE(std::get<double>(parameters.at("float01")) == 0.25);
      REQUIRE(std::get<std::string>(parameters.at("str")) == "v2");
      REQUIRE(std::get<bool>(parameters.at("bool")) == true);
    }
  }

  WHEN("An action with a missing required parameter is checked") {
    vda5050::Action a1;
    a1.actionType = "type1";
    a1.blockingType = vda5050::BlockingType::SOFT;
    a1.actionParameters = {{{"int", "-1999"}}};

    auto err = vda5050pp::core::checks::validateActionWithDeclaration(
        a1, vda5050pp::misc::ActionContext::k_unspecified, decl1);

    THEN("Errors are returned") { REQUIRE_FALSE(err.empty()); }
  }

  WHEN("An action with an invalid blocking type checked") {
    vda5050::Action a1;
    a1.actionType = "type1";
    a1.blockingType = vda5050::BlockingType::NONE;
    a1.actionParameters = {{{"int", "-1999"}, {"float01", "0.25"}, {"str", "v2"}}};

    auto err = vda5050pp::core::checks::validateActionWithDeclaration(
        a1, vda5050pp::misc::ActionContext::k_unspecified, decl1);

    THEN("Errors are returned") { REQUIRE_FALSE(err.empty()); }
  }

  WHEN("An action with an invalid integer is checked") {
    vda5050::Action a1;
    a1.actionType = "type1";
    a1.blockingType = vda5050::BlockingType::SOFT;
    a1.actionParameters = {{{"int", "ne du"}, {"float01", "0.25"}, {"str", "v2"}}};

    auto err = vda5050pp::core::checks::validateActionWithDeclaration(
        a1, vda5050pp::misc::ActionContext::k_unspecified, decl1);

    THEN("Errors are returned") { REQUIRE_FALSE(err.empty()); }
  }

  WHEN("An action with an invalid integer is checked") {
    vda5050::Action a1;
    a1.actionType = "type1";
    a1.blockingType = vda5050::BlockingType::SOFT;
    a1.actionParameters = {{{"int", "-1999"}, {"float01", "-five"}, {"str", "v2"}}};

    auto err = vda5050pp::core::checks::validateActionWithDeclaration(
        a1, vda5050pp::misc::ActionContext::k_unspecified, decl1);

    THEN("Errors are returned") { REQUIRE_FALSE(err.empty()); }
  }

  WHEN("An action with an invalid string value is checked") {
    vda5050::Action a1;
    a1.actionType = "type1";
    a1.blockingType = vda5050::BlockingType::SOFT;
    a1.actionParameters = {{{"int", "-1999"}, {"float01", "0.25"}, {"str", "v4"}}};

    auto err = vda5050pp::core::checks::validateActionWithDeclaration(
        a1, vda5050pp::misc::ActionContext::k_unspecified, decl1);

    THEN("Errors are returned") { REQUIRE_FALSE(err.empty()); }
  }

  WHEN("An action with an invalid float value is checked") {
    vda5050::Action a1;
    a1.actionType = "type1";
    a1.blockingType = vda5050::BlockingType::SOFT;
    a1.actionParameters = {{{"int", "-1999"}, {"float01", "-0.1"}, {"str", "v2"}}};

    auto err = vda5050pp::core::checks::validateActionWithDeclaration(
        a1, vda5050pp::misc::ActionContext::k_unspecified, decl1);

    THEN("Errors are returned") { REQUIRE_FALSE(err.empty()); }
  }

  WHEN("An action with an invalid float value is checked") {
    vda5050::Action a1;
    a1.actionType = "type1";
    a1.blockingType = vda5050::BlockingType::SOFT;
    a1.actionParameters = {{{"int", "-1999"}, {"float01", "1.03"}, {"str", "v2"}}};

    auto err = vda5050pp::core::checks::validateActionWithDeclaration(
        a1, vda5050pp::misc::ActionContext::k_unspecified, decl1);

    THEN("Errors are returned") { REQUIRE_FALSE(err.empty()); }
  }

  WHEN("An action with an invalid bool value is checked") {
    vda5050::Action a1;
    a1.actionType = "type1";
    a1.blockingType = vda5050::BlockingType::SOFT;
    a1.actionParameters = {{{"int", "-1999"}, {"float01", "0.25"}, {"bool", "tralse"}}};

    auto err = vda5050pp::core::checks::validateActionWithDeclaration(
        a1, vda5050pp::misc::ActionContext::k_unspecified, decl1);

    THEN("Errors are returned") { REQUIRE_FALSE(err.empty()); }
  }

  WHEN("An action with an invalid action type is checked") {
    vda5050::Action a1;
    a1.actionType = "type2";

    auto err = vda5050pp::core::checks::validateActionWithDeclaration(
        a1, vda5050pp::misc::ActionContext::k_unspecified, decl1);

    THEN("Errors are returned") { REQUIRE_FALSE(err.empty()); }
  }

  WHEN("An action with an unknown parameter type is checked") {
    vda5050::Action a1;
    a1.actionType = "type1";
    a1.blockingType = vda5050::BlockingType::SOFT;
    a1.actionParameters = {{{"int", "-1999"}, {"float01", "0.25"}, {"unknown", "void"}}};

    auto err = vda5050pp::core::checks::validateActionWithDeclaration(
        a1, vda5050pp::misc::ActionContext::k_unspecified, decl1);

    THEN("Errors are returned") { REQUIRE_FALSE(err.empty()); }
  }
}

TEST_CASE("core::checks::uniqueActionId", "[core][checks]") {
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().useWhiteList();
  vda5050pp::core::Instance::reset();
  auto instance = vda5050pp::core::Instance::init(cfg).lock();

  vda5050::Action a1;
  a1.actionId = "a1";

  vda5050::Action a2;
  a2.actionId = "a2";

  vda5050::Action a3;
  a3.actionId = "a3";

  std::set<std::string_view, std::less<>> seen;

  WHEN("Three distinct actions are checked") {
    auto e1 = vda5050pp::core::checks::uniqueActionId(a1, seen);
    auto e2 = vda5050pp::core::checks::uniqueActionId(a2, seen);
    auto e3 = vda5050pp::core::checks::uniqueActionId(a3, seen);

    REQUIRE(e1.empty());
    REQUIRE(e2.empty());
    REQUIRE(e3.empty());
  }

  WHEN("Three non distinct actions are checked") {
    instance->getOrderManager().addNewAction(std::make_shared<vda5050::Action>(a3));
    auto e1 = vda5050pp::core::checks::uniqueActionId(a1, seen);
    auto e2 = vda5050pp::core::checks::uniqueActionId(a2, seen);
    auto e3 = vda5050pp::core::checks::uniqueActionId(a1, seen);
    auto e4 = vda5050pp::core::checks::uniqueActionId(a3, seen);

    REQUIRE(e1.empty());
    REQUIRE(e2.empty());
    REQUIRE_FALSE(e4.empty());
  }
}

TEST_CASE("core::checks::controlActionFeasible", "[core][checks]") {
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().useWhiteList();
  vda5050pp::core::Instance::reset();
  auto instance = vda5050pp::core::Instance::init(cfg).lock();

  vda5050::Action pause;
  pause.actionType = "startPause";

  vda5050::Action resume;
  resume.actionType = "stopPause";

  vda5050::Action cancel;
  cancel.actionType = "cancelOrder";

  WHEN("The order status is active") {
    instance->getOrderManager().setOrderStatus(vda5050pp::misc::OrderStatus::k_order_active);
    THEN("Pause is feasible") {
      REQUIRE(vda5050pp::core::checks::controlActionFeasible(pause).empty());
    }
    THEN("Resume is not feasible") {
      REQUIRE_FALSE(vda5050pp::core::checks::controlActionFeasible(resume).empty());
    }
    THEN("Cancel is feasible") {
      REQUIRE(vda5050pp::core::checks::controlActionFeasible(cancel).empty());
    }
  }
  WHEN("The order status is paused") {
    instance->getOrderManager().setOrderStatus(vda5050pp::misc::OrderStatus::k_order_paused);
    THEN("Pause is not feasible") {
      REQUIRE_FALSE(vda5050pp::core::checks::controlActionFeasible(pause).empty());
    }
    THEN("Resume is feasible") {
      REQUIRE(vda5050pp::core::checks::controlActionFeasible(resume).empty());
    }
    THEN("Cancel is feasible") {
      REQUIRE(vda5050pp::core::checks::controlActionFeasible(cancel).empty());
    }
  }
  WHEN("The order status is canceling") {
    instance->getOrderManager().setOrderStatus(vda5050pp::misc::OrderStatus::k_order_canceling);
    THEN("Pause is not feasible") {
      REQUIRE_FALSE(vda5050pp::core::checks::controlActionFeasible(pause).empty());
    }
    THEN("Resume is not feasible") {
      REQUIRE_FALSE(vda5050pp::core::checks::controlActionFeasible(resume).empty());
    }
    THEN("Cancel is not feasible") {
      REQUIRE_FALSE(vda5050pp::core::checks::controlActionFeasible(cancel).empty());
    }
  }
}

TEST_CASE("core::checks::contextCheck", "[core][checks]") {
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().useWhiteList();
  vda5050pp::core::Instance::reset();
  auto instance = vda5050pp::core::Instance::init(cfg).lock();

  vda5050pp::agv_description::ActionDeclaration instant;
  instant.node = false;
  instant.edge = false;
  instant.instant = true;

  vda5050pp::agv_description::ActionDeclaration node;
  node.node = true;
  node.edge = false;
  node.instant = false;

  vda5050pp::agv_description::ActionDeclaration edge;
  edge.node = false;
  edge.edge = true;
  edge.instant = false;

  SECTION("Context mismatch") {
    REQUIRE_FALSE(vda5050pp::core::checks::contextCheck(
                      vda5050::Action{}, vda5050pp::misc::ActionContext::k_edge, instant)
                      .empty());
    REQUIRE_FALSE(vda5050pp::core::checks::contextCheck(
                      vda5050::Action{}, vda5050pp::misc::ActionContext::k_edge, node)
                      .empty());
    REQUIRE_FALSE(vda5050pp::core::checks::contextCheck(
                      vda5050::Action{}, vda5050pp::misc::ActionContext::k_node, instant)
                      .empty());
    REQUIRE_FALSE(vda5050pp::core::checks::contextCheck(
                      vda5050::Action{}, vda5050pp::misc::ActionContext::k_node, edge)
                      .empty());
    REQUIRE_FALSE(vda5050pp::core::checks::contextCheck(
                      vda5050::Action{}, vda5050pp::misc::ActionContext::k_instant, node)
                      .empty());
    REQUIRE_FALSE(vda5050pp::core::checks::contextCheck(
                      vda5050::Action{}, vda5050pp::misc::ActionContext::k_instant, edge)
                      .empty());
  }

  SECTION("Context match") {
    REQUIRE(vda5050pp::core::checks::contextCheck(vda5050::Action{},
                                                  vda5050pp::misc::ActionContext::k_edge, edge)
                .empty());
    REQUIRE(vda5050pp::core::checks::contextCheck(vda5050::Action{},
                                                  vda5050pp::misc::ActionContext::k_node, node)
                .empty());
    REQUIRE(vda5050pp::core::checks::contextCheck(
                vda5050::Action{}, vda5050pp::misc::ActionContext::k_instant, instant)
                .empty());
  }
}