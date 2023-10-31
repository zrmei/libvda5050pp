//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/config/node_reached_subconfig.h"

#include "vda5050++/core/config.h"

using namespace vda5050pp::config;

void NodeReachedSubConfig::getFrom(const ConfigNode &node) {
  this->ModuleSubConfig::getFrom(node);

  auto node_view = vda5050pp::core::config::ConfigNode::upcast(node).get();
  if (auto maybe_value = node_view["default_node_deviation_xy"].value<double>(); maybe_value) {
    this->default_node_deviation_xy_ = *maybe_value;
  }
  if (auto maybe_value = node_view["default_node_deviation_theta"].value<double>(); maybe_value) {
    this->default_node_deviation_theta_ = *maybe_value;
  }

  this->overwrite_node_deviation_xy_ = node_view["overwrite_node_deviation_xy"].value<double>();
  this->overwrite_node_deviation_theta_ =
      node_view["overwrite_node_deviation_theta"].value<double>();
}

void NodeReachedSubConfig::putTo(ConfigNode &node) const {
  this->ModuleSubConfig::putTo(node);

  auto table = vda5050pp::core::config::ConfigNode::upcast(node).get().as_table();
  table->insert("default_node_deviation_xy", this->default_node_deviation_xy_);
  table->insert("default_node_deviation_theta", this->default_node_deviation_theta_);
  if (this->overwrite_node_deviation_xy_) {
    table->insert("overwrite_node_deviation_xy", *this->overwrite_node_deviation_xy_);
  }
  if (this->overwrite_node_deviation_theta_) {
    table->insert("overwrite_node_deviation_theta", *this->overwrite_node_deviation_theta_);
  }
}

void NodeReachedSubConfig::setDefaultNodeDeviationXY(double new_value) {
  this->default_node_deviation_xy_ = new_value;
}
void NodeReachedSubConfig::setDefaultNodeDeviationTheta(double new_value) {
  this->default_node_deviation_theta_ = new_value;
}

void NodeReachedSubConfig::setOverwriteNodeDeviationXY(std::optional<double> new_value) {
  this->overwrite_node_deviation_xy_ = new_value;
}
void NodeReachedSubConfig::setOverwriteNodeDeviationTheta(std::optional<double> new_value) {
  this->overwrite_node_deviation_theta_ = new_value;
}

double NodeReachedSubConfig::getDefaultNodeDeviationXY() const {
  return this->default_node_deviation_xy_;
}
double NodeReachedSubConfig::getDefaultNodeDeviationTheta() const {
  return this->default_node_deviation_theta_;
}

std::optional<double> NodeReachedSubConfig::getOverwriteNodeDeviationXY() const {
  return this->overwrite_node_deviation_xy_;
}

std::optional<double> NodeReachedSubConfig::getOverwriteNodeDeviationTheta() const {
  return this->overwrite_node_deviation_theta_;
}