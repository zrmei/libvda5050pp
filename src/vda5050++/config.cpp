//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/config.h"

#include <fmt/format.h>
#include <toml++/toml.h>

#include <fstream>

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/config.h"
#include "vda5050++/core/instance.h"

using namespace vda5050pp;

Config::Config() {
  for (auto key : vda5050pp::core::Instance::registeredModules()) {
    if (auto cfg = vda5050pp::core::Instance::generateConfig(key); cfg != nullptr) {
      this->registerModuleConfig(key, cfg);
    }
  }
}

void Config::registerModuleConfig(
    std::string_view key, std::shared_ptr<config::ModuleSubConfig> sub_config) noexcept(false) {
  if (auto [_, ok] = this->module_config_registry_.try_emplace(std::string(key), sub_config); !ok) {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("Module config with key={} already registered", key)));
  }
}

[[nodiscard]] std::shared_ptr<config::ModuleSubConfig> Config::lookupModuleConfig(
    std::string_view key) const noexcept(false) {
  auto it = this->module_config_registry_.find(key);

  if (it == this->module_config_registry_.end()) {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("ModuleConfig with key={} not registered", key)));
  } else {
    return it->second;
  }
}

void Config::registerCustomConfig(std::string_view key,
                                  std::shared_ptr<config::SubConfig> sub_config) noexcept(false) {
  if (auto [_, ok] = this->custom_config_registry_.try_emplace(std::string(key), sub_config); !ok) {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("Custom config with key={} already registered", key)));
  }
}

[[nodiscard]] std::shared_ptr<config::SubConfig> Config::lookupCustomConfig(std::string_view key) {
  auto it = this->custom_config_registry_.find(key);

  if (it == this->custom_config_registry_.end()) {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("CustomConfig with key={} is not registered", key)));
  } else {
    return it->second;
  }
}

std::shared_ptr<config::SubConfig> Config::removeCustomConfig(std::string_view key) noexcept(
    false) {
  auto it = this->custom_config_registry_.find(key);

  if (it == this->custom_config_registry_.end()) {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("SubConfig with key={} is not registered", key)));
  }

  auto node = this->custom_config_registry_.extract(it);
  return node.mapped();
}

const config::GlobalConfig &Config::getGlobalConfig() const { return this->global_config_; }

config::GlobalConfig &Config::refGlobalConfig() { return this->global_config_; }

void Config::setGlobalConfig(const config::GlobalConfig &cfg) { this->global_config_ = cfg; }

vda5050pp::config::MqttSubConfig &Config::refMqttSubConfig() {
  return this->lookupModuleConfig(core::module_keys::k_mqtt_key)
      ->as<vda5050pp::config::MqttSubConfig>();
}

const vda5050pp::config::MqttSubConfig &Config::getMqttSubConfig() const {
  return this->lookupModuleConfig(core::module_keys::k_mqtt_key)
      ->as<vda5050pp::config::MqttSubConfig>();
}

Config Config::loadFrom(const std::filesystem::path &toml_file) noexcept(false) {
  Config cfg;
  cfg.load(toml_file);
  return cfg;
}

Config Config::loadFrom(std::string_view toml_string) noexcept(false) {
  Config cfg;
  cfg.load(toml_string);
  return cfg;
}

void Config::load(const std::filesystem::path &toml_file) {
  try {
    std::ifstream fs(toml_file);
    std::ostringstream ss;
    ss << fs.rdbuf();
    this->load(std::string_view(ss.str()));

  } catch (const toml::parse_error &e) {
    throw vda5050pp::VDA5050PPTOMLError(MK_EX_CONTEXT(e.what()));
  }
}

void Config::load(std::string_view toml_string) {
  try {
    toml::table table = toml::parse(toml_string);

    auto global_table = table["global"];
    auto agv_description_table = table["agv_description"];
    auto custom_table = table["custom"];
    auto module_table = table["module"];

    const core::config::ConfigNode global_wrapped(global_table.node());
    static_cast<config::SubConfig &>(this->global_config_).getFrom(global_wrapped);

    const core::config::ConfigNode agv_description_wrapped(agv_description_table.node());
    static_cast<config::SubConfig &>(this->agv_description_sub_config_)
        .getFrom(agv_description_wrapped);

    for (const auto &[key, sub_config] : this->module_config_registry_) {
      const core::config::ConfigNode node_wrapped(module_table[key].node());
      static_cast<config::SubConfig &>(*sub_config).getFrom(node_wrapped);
    }
    for (const auto &[key, sub_config] : this->custom_config_registry_) {
      const core::config::ConfigNode node_wrapped(custom_table[key].node());
      sub_config->getFrom(node_wrapped);
    }

  } catch (const toml::parse_error &e) {
    throw vda5050pp::VDA5050PPTOMLError(MK_EX_CONTEXT(e.what()));
  }
}

void Config::save(std::string &to_toml_string) const {
  toml::table table;

  toml::table global_table;
  core::config::ConfigNode global_wrapped(global_table);
  static_cast<const config::SubConfig &>(this->global_config_).putTo(global_wrapped);

  toml::table agv_description_table;
  core::config::ConfigNode agv_description_wrapped(agv_description_table);
  static_cast<const config::SubConfig &>(this->agv_description_sub_config_)
      .putTo(agv_description_wrapped);

  toml::table module_table;
  for (const auto &[key, sub_config] : this->module_config_registry_) {
    toml::table sub_table;
    core::config::ConfigNode node_wrapped(sub_table);
    static_cast<config::SubConfig &>(*sub_config).putTo(node_wrapped);
    if (!sub_table.empty()) {
      module_table.insert(key, sub_table);
    }
  }

  toml::table custom_table;
  for (const auto &[key, sub_config] : this->custom_config_registry_) {
    toml::table sub_table;
    core::config::ConfigNode node_wrapped(sub_table);
    sub_config->putTo(node_wrapped);
    custom_table.insert(key, sub_table);
    if (!sub_table.empty()) {
      custom_table.insert(key, sub_table);
    }
  }

  if (!global_table.empty()) {
    table.insert("global", global_table);
  }
  if (!global_table.empty()) {
    table.insert("agv_description", agv_description_table);
  }
  if (!module_table.empty()) {
    table.insert("module", module_table);
  }
  if (!custom_table.empty()) {
    table.insert("custom", custom_table);
  }

  std::stringstream ss;
  ss << toml::toml_formatter(table, toml::format_flags::indent_array_elements |
                                        toml::format_flags::allow_multi_line_strings |
                                        toml::format_flags::allow_literal_strings);
  to_toml_string = ss.str();
}

void Config::save(const std::filesystem::path &toml_file) const {
  std::ofstream ofs(toml_file);
  ofs.exceptions(std::ios_base::failbit);

  std::string contents;
  this->save(contents);
  try {
    ofs << contents;
  } catch (const std::ios_base::failure &) {
    // Format errno, set by ofs
    std::system_error sys_error{errno, std::iostream_category(), toml_file.c_str()};
    throw vda5050pp::VDA5050PPTOMLError(MK_EX_CONTEXT(sys_error.what()));
  }
}

void Config::setAgvDescription(const vda5050pp::agv_description::AGVDescription &desc) {
  this->agv_description_sub_config_.setAGVDescription(desc);
}

const vda5050pp::agv_description::AGVDescription &Config::getAgvDescription() const {
  return this->agv_description_sub_config_.getAGVDescription();
}

vda5050pp::agv_description::AGVDescription &Config::refAgvDescription() {
  return this->agv_description_sub_config_.refAGVDescription();
}

vda5050pp::config::AGVDescriptionSubConfig &Config::refAgvDescriptionSubConfig() {
  return this->agv_description_sub_config_;
}

vda5050pp::config::NodeReachedSubConfig &Config::refNodeReachedSubConfig() {
  return this->lookupModuleConfig(core::module_keys::k_node_reached_handler_key)
      ->as<vda5050pp::config::NodeReachedSubConfig>();
}

vda5050pp::config::QueryEventHandlerSubConfig &Config::refQueryEventHandlerSubConfig() {
  return this->lookupModuleConfig(core::module_keys::k_query_event_handler_key)
      ->as<vda5050pp::config::QueryEventHandlerSubConfig>();
}

vda5050pp::config::StateUpdateTimerSubConfig &Config::refStateUpdateTimerSubConfig() {
  return this->lookupModuleConfig(core::module_keys::k_state_update_timer_key)
      ->as<vda5050pp::config::StateUpdateTimerSubConfig>();
}

vda5050pp::config::VisualizationTimerSubConfig &Config::refVisualizationTimerSubConfig() {
  return this->lookupModuleConfig(core::module_keys::k_visualization_timer_key)
      ->as<vda5050pp::config::VisualizationTimerSubConfig>();
}