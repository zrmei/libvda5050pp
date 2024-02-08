//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/config.h"

#include <catch2/catch.hpp>
#include <memory>
#include <random>

#include "vda5050++/config/mqtt_subconfig.h"
#include "vda5050++/core/config.h"

class CustomConfig : public vda5050pp::config::SubConfig {
private:
  std::string test_field_;

protected:
  void getFrom(const vda5050pp::config::ConstConfigNode &node) override {
    auto toml_node = vda5050pp::core::config::ConstConfigNode::upcast(node).get();
    this->test_field_ = toml_node["test_field"].value_or<std::string>("none");
  }

  void putTo(vda5050pp::config::ConfigNode &node) const override {
    auto toml_node = vda5050pp::core::config::ConfigNode::upcast(node).get();
    toml_node.as_table()->insert("test_field", this->test_field_);
  }

public:
  void setTestField(std::string_view value) { this->test_field_ = value; }

  std::string_view getTestField() const { return this->test_field_; }
};

TEST_CASE("Config - load/save for sub_configs", "[config][io]") {
  vda5050pp::Config cfg;

  cfg.refMqttSubConfig().refOptions().username = "test user";
  cfg.refMqttSubConfig().refOptions().password = "s3cur3p4$$w0rd";
  cfg.refGlobalConfig().bwListModule("Mqtt");
  cfg.refGlobalConfig().bwListModule("Test");
  cfg.refGlobalConfig().useBlackList();
  cfg.refGlobalConfig().setLogLevel(vda5050pp::config::LogLevel::k_info);
  cfg.refGlobalConfig().setLogFileName("global_log.txt");
  cfg.registerCustomConfig("test_config", std::make_shared<CustomConfig>());
  cfg.lookupModuleConfig("OrderEventHandler")->setLogLevel(vda5050pp::config::LogLevel::k_warn);
  cfg.lookupModuleConfig("StateEventHandler")->setLogLevel(vda5050pp::config::LogLevel::k_debug);
  cfg.lookupCustomConfig("test_config")->as<CustomConfig>().setTestField("test_value");

  std::string serialized;
  cfg.save(serialized);

  WHEN("Restoring the file") {
    vda5050pp::Config cfg2;
    cfg2.registerCustomConfig("test_config", std::make_shared<CustomConfig>());
    cfg2.load(std::string_view(serialized));

    THEN("Config stay the same") {
      REQUIRE(cfg2.refMqttSubConfig().getOptions().username ==
              cfg.refMqttSubConfig().getOptions().username);
      REQUIRE(cfg2.refMqttSubConfig().getOptions().password ==
              cfg.refMqttSubConfig().getOptions().password);
      REQUIRE_FALSE(cfg2.getGlobalConfig().isListedModule("Mqtt"));
      REQUIRE_FALSE(cfg2.getGlobalConfig().isListedModule("Test"));
      REQUIRE(cfg2.getGlobalConfig().isListedModule("Other"));
      REQUIRE(cfg2.lookupModuleConfig("OrderEventHandler")->getLogLevel() ==
              vda5050pp::config::LogLevel::k_warn);
      REQUIRE(cfg2.lookupModuleConfig("StateEventHandler")->getLogLevel() ==
              vda5050pp::config::LogLevel::k_debug);
      REQUIRE(cfg2.refGlobalConfig().getLogLevel() == vda5050pp::config::LogLevel::k_info);
      REQUIRE(cfg2.refGlobalConfig().getLogFileName() == "global_log.txt");
      REQUIRE(cfg2.lookupCustomConfig("test_config")->as<CustomConfig>().getTestField() ==
              "test_value");
    }
  }

  WHEN("An invalid toml string is restored") {
    std::string invalid = "this_is_not_a_toml_string}{}{[]]]]}";

    REQUIRE_THROWS_AS(vda5050pp::Config::loadFrom(std::string_view(invalid)),
                      vda5050pp::VDA5050PPTOMLError);
  }
}

class TestSubConfig : public vda5050pp::config::SubConfig {
private:
  void getFrom(const vda5050pp::config::ConstConfigNode &) override { /* Unused */
  }
  void putTo(vda5050pp::config::ConfigNode &) const override { /* Unused */
  }
  int foo() const { return 0; }
};

class TestSubConfig2 : public vda5050pp::config::SubConfig {
private:
  void getFrom(const vda5050pp::config::ConstConfigNode &) override { /* Unused */
  }
  void putTo(vda5050pp::config::ConfigNode &) const override { /* Unused */
  }
  int foo() const { return 1; }
};

TEST_CASE("Config - SubConfig cast", "[config]") {
  std::shared_ptr<vda5050pp::config::SubConfig> sub_config = std::make_shared<TestSubConfig>();

  SECTION("Nullptr casting") {
    REQUIRE(vda5050pp::config::SubConfig::ptr_as<TestSubConfig>(nullptr) == nullptr);
  }

  SECTION("Correct casting") {
    REQUIRE_NOTHROW(vda5050pp::config::SubConfig::ptr_as<TestSubConfig>(sub_config));
    REQUIRE_NOTHROW(sub_config->as<TestSubConfig>());
  }

  SECTION("Incorrect casting") {
    REQUIRE_THROWS_AS(vda5050pp::config::SubConfig::ptr_as<TestSubConfig2>(sub_config),
                      vda5050pp::VDA5050PPBadCast);
    REQUIRE_THROWS_AS(sub_config->as<TestSubConfig2>(), vda5050pp::VDA5050PPBadCast);
  }
}

TEST_CASE("Config - register", "[config]") {
  vda5050pp::Config cfg;

  SECTION("Registering a config twice, throws") {
    REQUIRE_NOTHROW(cfg.registerCustomConfig("test_config", std::make_shared<CustomConfig>()));
    REQUIRE_THROWS_AS(cfg.registerCustomConfig("test_config", std::make_shared<CustomConfig>()),
                      vda5050pp::VDA5050PPInvalidArgument);
  }

  SECTION("Registering and unregistering works") {
    REQUIRE_NOTHROW(cfg.registerCustomConfig("test_config", std::make_shared<CustomConfig>()));
    REQUIRE_NOTHROW(cfg.removeCustomConfig("test_config"));
    REQUIRE_NOTHROW(cfg.registerCustomConfig("test_config", std::make_shared<CustomConfig>()));
  }

  SECTION("Unregistering an unknown config throws") {
    REQUIRE_THROWS_AS(cfg.removeCustomConfig("test_config"), vda5050pp::VDA5050PPInvalidArgument);
  }
}

TEST_CASE("Config - references", "[config]") {
  vda5050pp::Config cfg;
  REQUIRE_NOTHROW(cfg.refAgvDescription());
  REQUIRE_NOTHROW(cfg.refAgvDescriptionSubConfig());
  REQUIRE_NOTHROW(cfg.refGlobalConfig());
  REQUIRE_NOTHROW(cfg.refMqttSubConfig());
  REQUIRE_NOTHROW(cfg.refNodeReachedSubConfig());
  REQUIRE_NOTHROW(cfg.refQueryEventHandlerSubConfig());
  REQUIRE_NOTHROW(cfg.refStateUpdateTimerSubConfig());
  REQUIRE_NOTHROW(cfg.refVisualizationTimerSubConfig());
  REQUIRE_NOTHROW(cfg.getAgvDescription());
  REQUIRE_NOTHROW(cfg.getGlobalConfig());
  REQUIRE_NOTHROW(cfg.getMqttSubConfig());
  REQUIRE_NOTHROW(cfg.setAgvDescription(vda5050pp::agv_description::AGVDescription{}));
  REQUIRE_NOTHROW(cfg.setGlobalConfig(vda5050pp::config::GlobalConfig()));
}