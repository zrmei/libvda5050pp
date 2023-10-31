// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains the implementation of the VisualizationTimerSubconfig
//

#include "vda5050++/config/visualization_timer_subconfig.h"

#include "vda5050++/core/config.h"

using namespace vda5050pp::config;

void VisualizationTimerSubConfig::getFrom(const ConfigNode &node) {
  this->ModuleSubConfig::getFrom(node);

  auto node_view = vda5050pp::core::config::ConfigNode::upcast(node).get();
  if (auto maybe_value = node_view["visualization_period_ms"].value<int>(); maybe_value) {
    this->visualization_period_ = std::chrono::milliseconds(*maybe_value);
  }
}

void VisualizationTimerSubConfig::putTo(ConfigNode &node) const {
  this->ModuleSubConfig::putTo(node);

  auto table = vda5050pp::core::config::ConfigNode::upcast(node).get().as_table();
  table->insert(
      "visualization_period_ms",
      std::chrono::duration_cast<std::chrono::milliseconds>(this->visualization_period_).count());
}

std::chrono::system_clock::duration VisualizationTimerSubConfig::getVisualizationPeriod() const {
  return this->visualization_period_;
}

void VisualizationTimerSubConfig::setVisualizationPeriod(
    std::chrono::system_clock::duration new_value) {
  this->visualization_period_ = new_value;
}