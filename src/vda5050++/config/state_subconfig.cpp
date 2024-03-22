// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/config/state_subconfig.h"

#include "vda5050++/core/config.h"

using namespace vda5050pp::config;

void StateSubConfig::getFrom(const ConstConfigNode &node) {
  auto toml_node = vda5050pp::core::config::ConstConfigNode::upcast(node).get();

  this->ModuleSubConfig::getFrom(node);

  this->default_agv_position_map_ = toml_node["defaut_agv_position_map"].value<std::string>();
  this->use_agv_position_from_order_ =
      toml_node["use_agv_position_from_order"].value_or<bool>(false);
}

void StateSubConfig::putTo(ConfigNode &node) const {
  auto toml_node = vda5050pp::core::config::ConfigNode::upcast(node).get();

  this->ModuleSubConfig::putTo(node);

  if (this->default_agv_position_map_.has_value()) {
    toml_node.as_table()->insert("default_agv_position_map", *this->default_agv_position_map_);
  }
  toml_node.as_table()->insert("use_agv_position_from_order", this->use_agv_position_from_order_);
}

std::optional<std::string> StateSubConfig::getDefaultAgvPositionMap() const {
  return this->default_agv_position_map_;
}

void StateSubConfig::setDefaultAgvPositionMap(
    const std::optional<std::string> &default_agv_position_map) {
  this->default_agv_position_map_ = default_agv_position_map;
}

bool StateSubConfig::getUseAgvPositionFromOrder() const {
  return this->use_agv_position_from_order_;
}

void StateSubConfig::setUseAgvPositionFromOrder(bool use_agv_position_from_order) {
  this->use_agv_position_from_order_ = use_agv_position_from_order;
}