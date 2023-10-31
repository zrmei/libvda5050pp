//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/config/mqtt_subconfig.h"

#include "vda5050++/core/config.h"

using namespace vda5050pp::config;
using namespace std::string_view_literals;

void MqttSubConfig::getFrom(const ConfigNode &node) {
  auto toml_node = vda5050pp::core::config::ConfigNode::upcast(node).get();

  this->ModuleSubConfig::getFrom(node);

  this->options_.username = toml_node["username"].value<std::string>();
  this->options_.password = toml_node["password"].value<std::string>();
  this->options_.version_overwrite = toml_node["version_overwrite"].value<std::string>();
  this->options_.server = toml_node["server"].value_or("tcp://localhost:1883"sv);
  this->options_.interface = toml_node["interface"].value_or("uagv"sv);
  this->options_.enable_cert_check = toml_node["enable_cert_check"].value_or(false);
  this->options_.use_ssl = toml_node["use_ssl"].value_or(false);
}

void MqttSubConfig::putTo(ConfigNode &node) const {
  auto toml_node = vda5050pp::core::config::ConfigNode::upcast(node).get();

  this->ModuleSubConfig::putTo(node);

  if (this->options_.username.has_value()) {
    toml_node.as_table()->insert("username", this->options_.username.value());
  }
  if (this->options_.password.has_value()) {
    toml_node.as_table()->insert("password", this->options_.password.value());
  }
  if (this->options_.version_overwrite.has_value()) {
    toml_node.as_table()->insert("version_overwrite", this->options_.version_overwrite.value());
  }
  toml_node.as_table()->insert("server", this->options_.server);
  toml_node.as_table()->insert("interface", this->options_.interface);
  toml_node.as_table()->insert("enable_cert_check", this->options_.enable_cert_check);
  toml_node.as_table()->insert("use_ssl", this->options_.use_ssl);
}

void MqttSubConfig::setOptions(const MqttOptions &options) { this->options_ = options; }

const MqttOptions &MqttSubConfig::getOptions() const { return this->options_; }

MqttOptions &MqttSubConfig::refOptions() { return this->options_; }
