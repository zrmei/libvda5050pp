// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
//

#define _USE_MATH_DEFINES
#include "vda5050++/core/agv_handler/functional.h"

#include <catch2/catch.hpp>
#include <cmath>

TEST_CASE("core::agv_handler::isOnNode", "[core][agv_handler]") {
  vda5050::NodePosition np1;
  np1.x = 0;
  np1.y = 1;
  np1.theta = -M_PI;
  np1.allowedDeviationTheta = 0.1;
  np1.allowedDeviationXY = 1;

  vda5050::AGVPosition pos1;
  pos1.x = 0.3;
  pos1.y = 0.3;
  pos1.theta = M_PI;
  pos1.deviationRange = 0.1;

  auto pos2 = pos1;
  pos2.deviationRange = 0.5;

  WHEN("Pos1 an NodePos1 are checked") {
    THEN("It returns true") { REQUIRE(vda5050pp::core::agv_handler::isOnNode(pos1, np1, 0, 0)); }
  }

  WHEN("Pos2 an NodePos1 are checked") {
    THEN("It returns false") {
      REQUIRE_FALSE(vda5050pp::core::agv_handler::isOnNode(pos2, np1, 0, 0));
    }
  }
}