// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains the QueryEventHandlerSubConfig implementation.
//
//

#include "vda5050++/config/query_event_handler_subconfig.h"

#include "vda5050++/core/config.h"

using namespace vda5050pp::config;

void QueryEventHandlerSubConfig::getFrom(const ConfigNode &node) {
  auto toml_node = vda5050pp::core::config::ConfigNode::upcast(node).get();

  this->ModuleSubConfig::getFrom(node);

  this->default_pauseable_success_ = toml_node["default_pauseable_success"].value<bool>();
  this->default_resumable_success_ = toml_node["default_resumable_success"].value<bool>();
  this->default_accept_zone_set_success_ = toml_node["default_zone_set_success"].value<bool>();
  if (auto sets = toml_node["default_zone_sets"].as_array(); sets) {
    this->default_accept_zone_sets_ = std::set<std::string, std::less<>>();
    for (const auto &set : *sets) {
      if (auto set_str = set.value<std::string>(); set_str) {
        this->default_accept_zone_sets_->insert(*set_str);
      } else {
        throw vda5050pp::VDA5050PPTOMLError(
            MK_EX_CONTEXT(fmt::format("Could not parse default_zone_sets entry")));
      }
    }
  } else {
    this->default_accept_zone_sets_ = std::nullopt;
  }
}

void QueryEventHandlerSubConfig::putTo(ConfigNode &node) const {
  auto toml_node = vda5050pp::core::config::ConfigNode::upcast(node).get();

  this->ModuleSubConfig::putTo(node);

  if (this->default_pauseable_success_.has_value()) {
    toml_node.as_table()->insert("default_pauseable_success", *this->default_pauseable_success_);
  }
  if (this->default_resumable_success_.has_value()) {
    toml_node.as_table()->insert("default_resumable_success", *this->default_resumable_success_);
  }
  if (this->default_accept_zone_set_success_.has_value()) {
    toml_node.as_table()->insert("default_accept_zone_set_success",
                                 *this->default_accept_zone_set_success_);
  }
  if (this->default_accept_zone_sets_.has_value()) {
    toml::array array;
    for (const auto &set : *this->default_accept_zone_sets_) {
      array.push_back(set);
    }
    toml_node.as_table()->insert("default_zone_sets", array);
  }
}

void QueryEventHandlerSubConfig::setDefaultPauseableSuccess(std::optional<bool> value) {
  this->default_pauseable_success_ = value;
}

std::optional<bool> QueryEventHandlerSubConfig::getDefaultPauseableSuccess() const {
  return this->default_pauseable_success_;
}

void QueryEventHandlerSubConfig::setDefaultResumableSuccess(std::optional<bool> value) {
  this->default_resumable_success_ = value;
}

std::optional<bool> QueryEventHandlerSubConfig::getDefaultResumableSuccess() const {
  return this->default_resumable_success_;
}

void QueryEventHandlerSubConfig::setDefaultAcceptZoneSetSuccess(std::optional<bool> value) {
  this->default_accept_zone_set_success_ = value;
}

std::optional<bool> QueryEventHandlerSubConfig::getDefaultAcceptZoneSetSuccess() const {
  return default_accept_zone_set_success_;
}

void QueryEventHandlerSubConfig::setDefaultAcceptZoneSets(
    const std::set<std::string, std::less<>> &value) {
  this->default_accept_zone_sets_ = value;
}

const std::optional<std::set<std::string, std::less<>>>
    &QueryEventHandlerSubConfig::getDefaultAcceptZoneSets() const {
  return this->default_accept_zone_sets_;
}