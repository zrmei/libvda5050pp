// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/misc/action_parameter_view.h"

#include <catch2/catch_all.hpp>

#include "vda5050++/exception.h"

TEST_CASE("misc::ActionParameterValueView", "[misc]") {
  SECTION("String views") {
    std::string_view value = "arbitrary_string";
    vda5050pp::misc::ActionParameterValueView view(value);

    REQUIRE(view.get() == value);
    REQUIRE_THROWS_AS(view.getInt(), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(view.getBool(), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(view.getFloat(), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(int64_t(view), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(bool(view), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(double(view), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE(view.tryGetBool() == std::nullopt);
    REQUIRE(view.tryGetFloat() == std::nullopt);
    REQUIRE(view.tryGetInt() == std::nullopt);
  }

  SECTION("Integer views") {
    int64_t value1 = 123;
    int64_t value2 = -456;
    auto s1 = std::to_string(value1);
    auto s2 = std::to_string(value2);
    vda5050pp::misc::ActionParameterValueView view1(s1);
    vda5050pp::misc::ActionParameterValueView view2(s2);

    REQUIRE(view1.getInt() == value1);
    REQUIRE(view2.getInt() == value2);
    REQUIRE(int64_t(view1) == value1);
    REQUIRE(int64_t(view2) == value2);
    REQUIRE_THROWS_AS(view1.getBool(), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(view2.getBool(), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(bool(view1), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(bool(view2), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE(view1.tryGetBool() == std::nullopt);
    REQUIRE(view2.tryGetBool() == std::nullopt);
    REQUIRE(view1.tryGetInt().value() == value1);
    REQUIRE(view2.tryGetInt().value() == value2);
  }

  SECTION("Bool views") {
    bool value1 = true;
    bool value2 = false;
    vda5050pp::misc::ActionParameterValueView view1("true");
    vda5050pp::misc::ActionParameterValueView view2("false");

    REQUIRE(view1.getBool() == value1);
    REQUIRE(view2.getBool() == value2);
    REQUIRE(bool(view1) == value1);
    REQUIRE(bool(view2) == value2);
    REQUIRE_THROWS_AS(view1.getInt(), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(view2.getInt(), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(int64_t(view1), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(int64_t(view2), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(view1.getFloat(), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(view2.getFloat(), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(double(view1), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(double(view2), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE(view1.tryGetBool().value() == value1);
    REQUIRE(view2.tryGetBool().value() == value2);
    REQUIRE(view1.tryGetFloat() == std::nullopt);
    REQUIRE(view1.tryGetInt() == std::nullopt);
    REQUIRE(view2.tryGetFloat() == std::nullopt);
    REQUIRE(view2.tryGetInt() == std::nullopt);
  }

  SECTION("Float views") {
    double value1 = 64.5;
    double value2 = -128.25;
    auto s1 = std::to_string(value1);
    auto s2 = std::to_string(value2);
    vda5050pp::misc::ActionParameterValueView view1(s1);
    vda5050pp::misc::ActionParameterValueView view2(s2);

    REQUIRE(view1.getFloat() == value1);
    REQUIRE(view2.getFloat() == value2);
    REQUIRE(double(view1) == value1);
    REQUIRE(double(view2) == value2);
    REQUIRE_THROWS_AS(view1.getInt(), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(view2.getInt(), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(int64_t(view1), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(int64_t(view2), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(view1.getBool(), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(view2.getBool(), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(bool(view1), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(bool(view2), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE(view1.tryGetFloat().value() == value1);
    REQUIRE(view2.tryGetFloat().value() == value2);
    REQUIRE(view1.tryGetBool() == std::nullopt);
    REQUIRE(view1.tryGetInt() == std::nullopt);
    REQUIRE(view2.tryGetBool() == std::nullopt);
    REQUIRE(view2.tryGetInt() == std::nullopt);
  }
}

TEST_CASE("misc::ActionParameterView", "[misc]") {
  vda5050::ActionParameter p_int{"int", "123"};
  vda5050::ActionParameter p_bool{"bool", "Yes"};
  vda5050::ActionParameter p_float{"float", "-128.125"};
  vda5050::ActionParameter p_string{"string", "this_is_a_string"};
  std::vector<vda5050::ActionParameter> action_parameters{
      p_int,
      p_bool,
      p_float,
      p_string,
  };

  vda5050pp::misc::ActionParameterView view(action_parameters);

  SECTION("Getting a unknown key") {
    REQUIRE_THROWS_AS(view.get("unknown"), vda5050pp::VDA5050PPInvalidActionParameterKey);
    REQUIRE_THROWS_AS(view.getInt("unknown"), vda5050pp::VDA5050PPInvalidActionParameterKey);
    REQUIRE_THROWS_AS(view.getFloat("unknown"), vda5050pp::VDA5050PPInvalidActionParameterKey);
    REQUIRE_THROWS_AS(view.getBool("unknown"), vda5050pp::VDA5050PPInvalidActionParameterKey);
    REQUIRE(view.tryGet("unknown") == std::nullopt);
    REQUIRE(view.tryGetInt("unknown") == std::nullopt);
    REQUIRE(view.tryGetBool("unknown") == std::nullopt);
    REQUIRE(view.tryGetFloat("unknown") == std::nullopt);
  }

  SECTION("Getting a string") {
    REQUIRE(view.get(p_int.key) == p_int.value);
    REQUIRE(view.get(p_float.key) == p_float.value);
    REQUIRE(view.get(p_bool.key) == p_bool.value);
    REQUIRE(view.get(p_string.key) == p_string.value);
  }

  SECTION("Getting an integer") {
    REQUIRE(view.getInt("int") == 123);
    REQUIRE_THROWS_AS(view.getBool("int"), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE(view.tryGetBool("int") == std::nullopt);
  }

  SECTION("Getting a float") {
    REQUIRE(view.getFloat("float") == -128.125);
    REQUIRE_THROWS_AS(view.getBool("float"), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(view.getInt("float"), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE(view.tryGetBool("float") == std::nullopt);
    REQUIRE(view.tryGetInt("float") == std::nullopt);
  }

  SECTION("Getting a bool") {
    REQUIRE(view.getBool("bool") == true);
    REQUIRE_THROWS_AS(view.getInt("bool"), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE_THROWS_AS(view.getFloat("bool"), vda5050pp::VDA5050PPInvalidActionParameterType);
    REQUIRE(view.tryGetInt("bool") == std::nullopt);
    REQUIRE(view.tryGetFloat("bool") == std::nullopt);
  }
}