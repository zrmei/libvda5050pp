// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/config/key_value_config.h"

#include <catch2/catch_all.hpp>

#include "vda5050++/config.h"

using namespace std::string_view_literals;

TEST_CASE("config::KeyValueConfig - load/save", "[config]") {
  GIVEN("A KeyValueConfig with required non-default entries") {
    auto kv_config = std::make_shared<vda5050pp::config::KeyValueConfig>();

    kv_config->addIntegerEntry("int", true, 123);
    kv_config->addFloatEntry("float", true, 3.14);
    kv_config->addStringEntry("string", true, "test string");
    kv_config->addBooleanEntry("bool", true);

    vda5050pp::Config cfg;
    std::string serialized;
    cfg.save(serialized);
    cfg.registerCustomConfig("kv", kv_config);

    kv_config->setBoolean("bool", true);
    std::string serialized_2;
    cfg.save(serialized_2);

    WHEN("Loading an empty config with required entries") {
      THEN("An exception is thrown") {
        REQUIRE_THROWS_AS(cfg.load(std::string_view(serialized)), vda5050pp::VDA5050PPTOMLError);
      }
    }

    WHEN("Loading a config having the required entries set") {
      cfg.load(std::string_view(serialized_2));
      THEN("The KeyValueConfig can be restored") {
        REQUIRE(cfg.lookupCustomConfigAs<vda5050pp::config::KeyValueConfig>("kv")->getInteger(
                    "int") == 123);
        REQUIRE(cfg.lookupCustomConfigAs<vda5050pp::config::KeyValueConfig>("kv")->getFloat(
                    "float") == 3.14);
        REQUIRE(cfg.lookupCustomConfigAs<vda5050pp::config::KeyValueConfig>("kv")->getString(
                    "string") == "test string");
        REQUIRE(cfg.lookupCustomConfigAs<vda5050pp::config::KeyValueConfig>("kv")->getBoolean(
                    "bool") == true);
      }

      WHEN("Setting the entries") {
        kv_config->setBoolean("bool", true);
        kv_config->setFloat("float", -3.14);
        kv_config->setInteger("int", -123);
        kv_config->setString("string", "Another string");

        THEN("They are properly set") {
          REQUIRE(kv_config->getBoolean("bool") == true);
          REQUIRE(kv_config->getFloat("float") == -3.14);
          REQUIRE(kv_config->getInteger("int") == -123);
          REQUIRE(kv_config->getString("string") == "Another string");
        }
      }

      WHEN("Setting invalid entries") {
        THEN("Exceptions are thrown") {
          REQUIRE_THROWS_AS(kv_config->getBoolean("float"), vda5050pp::VDA5050PPInvalidArgument);
          REQUIRE_THROWS_AS(kv_config->getFloat("int"), vda5050pp::VDA5050PPInvalidArgument);
          REQUIRE_THROWS_AS(kv_config->getInteger("string"), vda5050pp::VDA5050PPInvalidArgument);
          REQUIRE_THROWS_AS(kv_config->getString("bool"), vda5050pp::VDA5050PPInvalidArgument);
          REQUIRE_THROWS_AS(kv_config->getBoolean("bool2"), vda5050pp::VDA5050PPInvalidArgument);
          REQUIRE_THROWS_AS(kv_config->getFloat("float2"), vda5050pp::VDA5050PPInvalidArgument);
          REQUIRE_THROWS_AS(kv_config->getInteger("integer2"), vda5050pp::VDA5050PPInvalidArgument);
          REQUIRE_THROWS_AS(kv_config->getString("string2"), vda5050pp::VDA5050PPInvalidArgument);
        }
      }

      WHEN("Accessing entries with different types") {
        THEN("An exception is thrown") {
          REQUIRE_THROWS_AS(
              cfg.lookupCustomConfig("kv")->as<vda5050pp::config::KeyValueConfig>().getFloat("int"),
              vda5050pp::VDA5050PPInvalidArgument);
          REQUIRE_THROWS_AS(
              cfg.lookupCustomConfig("kv")->as<vda5050pp::config::KeyValueConfig>().getString(
                  "int"),
              vda5050pp::VDA5050PPInvalidArgument);
          REQUIRE_THROWS_AS(
              cfg.lookupCustomConfig("kv")->as<vda5050pp::config::KeyValueConfig>().getBoolean(
                  "string"),
              vda5050pp::VDA5050PPInvalidArgument);
          REQUIRE_THROWS_AS(
              cfg.lookupCustomConfig("kv")->as<vda5050pp::config::KeyValueConfig>().getInteger(
                  "bool"),
              vda5050pp::VDA5050PPInvalidArgument);
        }
      }

      WHEN("Accessing unknown entries") {
        THEN("An exception is thrown") {
          REQUIRE_THROWS_AS(
              cfg.lookupCustomConfig("kv")->as<vda5050pp::config::KeyValueConfig>().getFloat(
                  "float2"),
              vda5050pp::VDA5050PPInvalidArgument);
          REQUIRE_THROWS_AS(
              cfg.lookupCustomConfig("kv")->as<vda5050pp::config::KeyValueConfig>().getString(
                  "string2"),
              vda5050pp::VDA5050PPInvalidArgument);
          REQUIRE_THROWS_AS(
              cfg.lookupCustomConfig("kv")->as<vda5050pp::config::KeyValueConfig>().getBoolean(
                  "bool2"),
              vda5050pp::VDA5050PPInvalidArgument);
          REQUIRE_THROWS_AS(
              cfg.lookupCustomConfig("kv")->as<vda5050pp::config::KeyValueConfig>().getInteger(
                  "int2"),
              vda5050pp::VDA5050PPInvalidArgument);
        }
      }
    }
  }

  GIVEN("A KeyValueConfig with validator entries") {
    auto kv_config = std::make_shared<vda5050pp::config::KeyValueConfig>();
    kv_config->addStringEntry("string", false, std::nullopt,
                              [](auto v) { return v.length() == 5; });
    kv_config->addBooleanEntry("bool", false, std::nullopt, [](auto v) { return v; });
    kv_config->addFloatEntry("float", false, std::nullopt, [](auto v) { return v > 0; });
    kv_config->addIntegerEntry("int", false, std::nullopt, [](auto v) { return v > 0; });

    vda5050pp::Config cfg;
    cfg.registerCustomConfig("kv", kv_config);

    WHEN("Restoring an invalid string") {
      kv_config->setString("string", "123456");

      std::string invalid;
      cfg.save(invalid);
      THEN("cfg.load throws") {
        REQUIRE_THROWS_AS(cfg.load(std::string_view(invalid)), vda5050pp::VDA5050PPTOMLError);
      }
    }
    WHEN("Restoring an invalid boolean") {
      kv_config->setBoolean("bool", false);

      std::string invalid;
      cfg.save(invalid);
      THEN("cfg.load throws") {
        REQUIRE_THROWS_AS(cfg.load(std::string_view(invalid)), vda5050pp::VDA5050PPTOMLError);
      }
    }
    WHEN("Restoring an invalid float") {
      kv_config->setFloat("float", -123.0);

      std::string invalid;
      cfg.save(invalid);
      THEN("cfg.load throws") {
        REQUIRE_THROWS_AS(cfg.load(std::string_view(invalid)), vda5050pp::VDA5050PPTOMLError);
      }
    }
    WHEN("Restoring an invalid integer") {
      kv_config->setInteger("int", -123);

      std::string invalid;
      cfg.save(invalid);
      THEN("cfg.load throws") {
        REQUIRE_THROWS_AS(cfg.load(std::string_view(invalid)), vda5050pp::VDA5050PPTOMLError);
      }
    }

    WHEN("Restoring a valid set of entries") {
      kv_config->setString("string", "hello");
      kv_config->setBoolean("bool", true);
      kv_config->setFloat("float", 123.0);
      kv_config->setInteger("int", 123);
      std::string valid;
      cfg.save(valid);
      THEN("cfg.load succeeds") { REQUIRE_NOTHROW(cfg.load(std::string_view(valid))); }
    }
  }

  GIVEN("A KeyValueConfig with a SubConfig") {
    auto kv_config = std::make_shared<vda5050pp::config::KeyValueConfig>();
    auto kv_sub_config = std::make_shared<vda5050pp::config::KeyValueConfig>();

    kv_sub_config->addStringEntry("sub_str", true);
    kv_config->addSubConfigEntry("sub", kv_sub_config);
    kv_config->addStringEntry("string", false);

    vda5050pp::Config cfg;
    cfg.registerCustomConfig("kv", kv_config);

    WHEN("The sub_config is invalid") {
      std::string serialized;
      cfg.save(serialized);
      THEN("An exception is propagated") {
        REQUIRE_THROWS_AS(cfg.load(std::string_view(serialized)), vda5050pp::VDA5050PPTOMLError);
      }
    }

    WHEN("The sub_config is valid") {
      kv_sub_config->setString("sub_str", "hello");
      std::string serialized;
      cfg.save(serialized);

      THEN("It can be loaded") {
        REQUIRE_NOTHROW(cfg.load(std::string_view(serialized)));
        REQUIRE(cfg.lookupCustomConfigAs<vda5050pp::config::KeyValueConfig>("kv")
                    ->getSubConfigAs<vda5050pp::config::KeyValueConfig>("sub")
                    ->getString("sub_str") == "hello");
      }
    }

    WHEN("An the config is accessed incorrectly") {
      THEN("An exception is thrown") {
        REQUIRE_THROWS_AS(kv_config->getSubConfig("unknown"), vda5050pp::VDA5050PPInvalidArgument);
        REQUIRE_THROWS_AS(kv_config->getSubConfig("string"), vda5050pp::VDA5050PPInvalidArgument);
      }
    }
  }
}