//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/handle.h"

#include <catch2/catch.hpp>

#include "vda5050++/core/instance.h"
#include "vda5050++/exception.h"

TEST_CASE("Handle functions", "[interface]") {
  SECTION("Handle::registerActionHandler") {
    vda5050pp::Handle().shutdown();
    WHEN("The initialize() was not called yet") {
      THEN("registerActionHandler throws") {
        REQUIRE_THROWS_AS(vda5050pp::Handle().registerActionHandler(nullptr),
                          vda5050pp::VDA5050PPNotInitialized);
      }
    }

    WHEN("initialize() was called") {
      THEN("registerActionHandler does not throw") {
        vda5050pp::Config cfg;
        cfg.refGlobalConfig().useWhiteList();
        vda5050pp::Handle().initialize(cfg);
        REQUIRE_NOTHROW(vda5050pp::Handle().registerActionHandler(nullptr));
      }
    }
  }

  SECTION("Handle::registerNavigationHandler") {
    vda5050pp::Handle().shutdown();
    WHEN("The initialize() was not called yet") {
      THEN("registerNavigation throws") {
        REQUIRE_THROWS_AS(vda5050pp::Handle().registerNavigationHandler(nullptr),
                          vda5050pp::VDA5050PPNotInitialized);
      }
    }

    WHEN("initialize() was called") {
      THEN("registerActionHandler does not throw") {
        vda5050pp::Config cfg;
        cfg.refGlobalConfig().useWhiteList();
        vda5050pp::Handle().initialize(cfg);
        REQUIRE_NOTHROW(vda5050pp::Handle().registerNavigationHandler(nullptr));
      }
    }
  }

  SECTION("Handle::registerQueryHandler") {
    vda5050pp::Handle().shutdown();
    WHEN("The initialize() was not called yet") {
      THEN("registerQueryHandler throws") {
        REQUIRE_THROWS_AS(vda5050pp::Handle().registerQueryHandler(nullptr),
                          vda5050pp::VDA5050PPNotInitialized);
      }
    }

    WHEN("initialize() was called") {
      THEN("registerQueryHandler does not throw") {
        vda5050pp::Config cfg;
        cfg.refGlobalConfig().useWhiteList();
        vda5050pp::Handle().initialize(cfg);
        REQUIRE_NOTHROW(vda5050pp::Handle().registerQueryHandler(nullptr));
      }
    }
  }

  REQUIRE_NOTHROW(vda5050pp::Handle().getNavigationSink());
  REQUIRE_NOTHROW(vda5050pp::Handle().getStatusSink());
}