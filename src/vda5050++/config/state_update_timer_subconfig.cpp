//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/config/state_update_timer_subconfig.h"

#include "vda5050++/core/config.h"

using namespace vda5050pp::config;

void StateUpdateTimerSubConfig::getFrom(const ConfigNode &node) {
  this->ModuleSubConfig::getFrom(node);

  auto node_view = vda5050pp::core::config::ConfigNode::upcast(node).get();
  if (auto maybe_value = node_view["max_update_period_ms"].value<int>(); maybe_value) {
    this->state_update_period_ = std::chrono::milliseconds(*maybe_value);
  }
}

void StateUpdateTimerSubConfig::putTo(ConfigNode &node) const {
  this->ModuleSubConfig::putTo(node);

  auto table = vda5050pp::core::config::ConfigNode::upcast(node).get().as_table();
  table->insert(
      "max_update_period_ms",
      std::chrono::duration_cast<std::chrono::milliseconds>(this->state_update_period_).count());
}

void StateUpdateTimerSubConfig::setMaxUpdatePeriod(std::chrono::system_clock::duration period) {
  this->state_update_period_ = period;
}

std::chrono::system_clock::duration StateUpdateTimerSubConfig::getMaxUpdatePeriod() const {
  return this->state_update_period_;
}