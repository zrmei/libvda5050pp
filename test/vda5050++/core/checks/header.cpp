// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/core/checks/header.h"

#include <catch2/catch_all.hpp>

#include "vda5050++/core/instance.h"
#include "vda5050++/version.h"

TEST_CASE("core::checks::checkHeader", "[core][checks]") {
  vda5050pp::Config cfg;
  cfg.refAgvDescription().manufacturer = "manufacturer";
  cfg.refAgvDescription().serial_number = "serial_number";
  cfg.refGlobalConfig().useWhiteList();
  vda5050pp::core::Instance::reset();
  vda5050pp::core::Instance::init(cfg);

  WHEN("A header with an invalid manufacturer is checked") {
    vda5050::HeaderVDA5050 header;
    header.manufacturer = "invalid";
    header.serialNumber = "serial_number";
    header.version = vda5050pp::version::getCurrentVersion();

    THEN("The check fails") { REQUIRE_FALSE(vda5050pp::core::checks::checkHeader(header).empty()); }
  }

  WHEN("A header with an invalid serial number is checked") {
    vda5050::HeaderVDA5050 header;
    header.manufacturer = "manufacturer";
    header.serialNumber = "invalid";
    header.version = vda5050pp::version::getCurrentVersion();

    THEN("The check fails") { REQUIRE_FALSE(vda5050pp::core::checks::checkHeader(header).empty()); }
  }

  WHEN("A valid header is checked") {
    vda5050::HeaderVDA5050 header;
    header.manufacturer = "manufacturer";
    header.serialNumber = "serial_number";
    header.version = vda5050pp::version::getCurrentVersion();

    THEN("The check succeeds") { REQUIRE(vda5050pp::core::checks::checkHeader(header).empty()); }
  }

  WHEN("A header with any compatible version is checked") {
    vda5050::HeaderVDA5050 header;
    header.manufacturer = "manufacturer";
    header.serialNumber = "serial_number";

    for (const auto &compat : vda5050pp::version::getCompatible()) {
      header.version = compat;
      THEN("The check succeeds") { REQUIRE(vda5050pp::core::checks::checkHeader(header).empty()); }
    }
  }
  WHEN("A header with anc incompatible version is checked") {
    vda5050::HeaderVDA5050 header;
    header.manufacturer = "manufacturer";
    header.serialNumber = "serial_number";
    header.version = "invalid";

    THEN("The check fails") { REQUIRE_FALSE(vda5050pp::core::checks::checkHeader(header).empty()); }
  }
}