//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/common/formatters.h"

#include <catch2/catch_all.hpp>

TEST_CASE("fmt::formatter vda5050pp::BlockingType", "[core][common][formatter]") {
  SECTION("BlockingType::HARD") {
    REQUIRE(fmt::format("{}", vda5050::BlockingType::HARD) == "HARD");
  }
  SECTION("BlockingType::SOFT") {
    REQUIRE(fmt::format("{}", vda5050::BlockingType::SOFT) == "SOFT");
  }
  SECTION("BlockingType::NONE") {
    REQUIRE(fmt::format("{}", vda5050::BlockingType::NONE) == "NONE");
  }
  SECTION("unknown BlockingType") {
    REQUIRE(fmt::format("{}", vda5050::BlockingType(-1)) == "unknown");
  }
}