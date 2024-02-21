//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/common/exception.h"

#include <catch2/catch_all.hpp>

static void testMkFnContext() {
  vda5050pp::VDA5050PPError err(MK_FN_EX_CONTEXT("description"));
  REQUIRE_FALSE(err.getClassContext().has_value());
  REQUIRE(err.getDescription().has_value());
  REQUIRE(err.getFunctionContext().has_value());
  REQUIRE(*err.getDescription() == "description");
}

class TestMkContext {
public:
  void runTest() const noexcept(true) {
    vda5050pp::VDA5050PPError err(MK_EX_CONTEXT("description"));
    REQUIRE(err.getClassContext().has_value());
    REQUIRE(err.getDescription().has_value());
    REQUIRE(err.getFunctionContext().has_value());
    REQUIRE(err.getClassContext()->find_first_of("TestMkContext") != std::string_view::npos);
    REQUIRE(*err.getDescription() == "description");
  }
};

TEST_CASE("core::common::Exception contents", "[core][common]") {
  SECTION("MK_FN_EX_CONTEXT") { testMkFnContext(); }
  SECTION("MK_EX_CONTEXT") { TestMkContext().runTest(); }
  SECTION("Additional context") {
    vda5050pp::VDA5050PPError error(MK_FN_EX_CONTEXT(""));
    error.addAdditionalContext("key1", "value1");
    error.addAdditionalContext({{"key2", "value2"}, {"key3", "value3"}});
    REQUIRE(error.getAdditionalContext().size() == 3);
    REQUIRE(error.getAdditionalContext().at("key1") == "value1");
    REQUIRE(error.getAdditionalContext().at("key2") == "value2");
    REQUIRE(error.getAdditionalContext().at("key3") == "value3");
  }
}
