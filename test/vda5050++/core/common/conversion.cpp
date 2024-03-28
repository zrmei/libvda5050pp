// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
//

#include "vda5050++/core/common/conversion.h"

#include <catch2/catch_all.hpp>

TEST_CASE("core::common::fromActionDeclaration ", "[core][common]") {
  vda5050pp::agv_description::ParameterRange p1;
  p1.key = "p1";
  p1.type = vda5050pp::agv_description::ParameterValueType::k_integer;
  p1.description = "p1_desc";

  vda5050pp::agv_description::ParameterRange p2;
  p2.key = "p2";
  p2.type = vda5050pp::agv_description::ParameterValueType::k_string;
  p2.description = "p2_desc";

  vda5050pp::agv_description::ParameterRange p3;
  p3.key = "p3";
  p3.type = vda5050pp::agv_description::ParameterValueType::k_boolean;
  p3.description = "p3_desc";

  vda5050pp::agv_description::ParameterRange p4;
  p4.key = "p4";
  p4.type = vda5050pp::agv_description::ParameterValueType::k_float;
  p4.description = "p4_desc";

  vda5050pp::agv_description::ActionDeclaration decl;
  decl.action_type = "test_type";
  decl.blocking_types = {vda5050::BlockingType::HARD, vda5050::BlockingType::SOFT};
  decl.description = "test_description";
  decl.edge = true;
  decl.node = true;
  decl.instant = false;
  decl.result_description = "test_result_description";
  decl.parameter = {p1, p2, p3};
  decl.optional_parameter = {p4};

  WHEN("Converting the test declaration") {
    auto to = vda5050pp::core::common::fromActionDeclaration(decl);

    THEN("The conversion is correct") {
      REQUIRE(to.actionDescription == decl.description);
      REQUIRE(to.actionScopes ==
              std::vector{vda5050::ActionScope::EDGE, vda5050::ActionScope::NODE});
      REQUIRE(to.actionType == decl.action_type);
      REQUIRE(to.resultDescription == decl.result_description);
      REQUIRE(to.actionParameters.has_value());
      REQUIRE(to.actionParameters->size() == 4);
      REQUIRE(to.actionParameters->at(0).key == p1.key);
      REQUIRE(to.actionParameters->at(1).key == p2.key);
      REQUIRE(to.actionParameters->at(2).key == p3.key);
      REQUIRE(to.actionParameters->at(3).key == p4.key);
      REQUIRE(to.actionParameters->at(0).valueDataType == vda5050::ValueDataType::INTEGER);
      REQUIRE(to.actionParameters->at(1).valueDataType == vda5050::ValueDataType::STRING);
      REQUIRE(to.actionParameters->at(2).valueDataType == vda5050::ValueDataType::BOOLEAN);
      REQUIRE(to.actionParameters->at(3).valueDataType == vda5050::ValueDataType::FLOAT);
      REQUIRE_FALSE(to.actionParameters->at(0).isOptional.value());
      REQUIRE_FALSE(to.actionParameters->at(1).isOptional.value());
      REQUIRE_FALSE(to.actionParameters->at(2).isOptional.value());
      REQUIRE(to.actionParameters->at(3).isOptional.value());
    }
  }
}