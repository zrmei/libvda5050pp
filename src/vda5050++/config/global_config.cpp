//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/config/global_config.h"

#include "vda5050++/core/config.h"

using namespace vda5050pp::config;

void GlobalConfig::getFrom(const ConfigNode &node) {
  auto node_view = core::config::ConfigNode::upcast(node).get();

  this->LoggingSubConfig::getFrom(node);
  this->event_manager_options_.synchronous_event_dispatch =
      node_view["event_manager_options.synchronous_event_dispatch"].value_or(false);

  auto bl = node_view["module_black_list"];
  auto wl = node_view["module_white_list"];

  if (bl && wl) {
    throw vda5050pp::VDA5050PPInvalidConfiguration(MK_EX_CONTEXT(
        "Cannot have global.module_white_list and global.module_black_list at the same time."));
  }

  if (auto list = bl ? bl : wl; list.is_array()) {
    this->is_black_list_ = true;
    for (const auto &v : *list.as_array()) {
      if (v.is_string()) {
        this->module_bw_list_.insert(v.as_string()->get());
      }
    }
  }
}

void GlobalConfig::putTo(ConfigNode &node) const {
  auto table = core::config::ConfigNode::upcast(node).get().as_table();

  this->LoggingSubConfig::putTo(node);
  table->insert("event_manager_options",
                toml::table{{"synchronous_event_dispatch",
                             this->event_manager_options_.synchronous_event_dispatch}});

  if (!this->module_bw_list_.empty()) {
    toml::array module_list;
    module_list.insert(module_list.begin(), this->module_bw_list_.begin(),
                       this->module_bw_list_.end());

    table->insert(this->is_black_list_ ? "module_black_list" : "module_white_list", module_list);
  }
}

bool GlobalConfig::isListedModule(std::string_view key) const {
  auto it = this->module_bw_list_.find(key);
  auto found = it != this->module_bw_list_.end();

  return this->is_black_list_ != found;
}
void GlobalConfig::useBlackList() { this->is_black_list_ = true; }

void GlobalConfig::useWhiteList() { this->is_black_list_ = false; }

void GlobalConfig::bwListModule(std::string_view key) { this->module_bw_list_.emplace(key); }

void GlobalConfig::bwUnListModule(std::string_view key) {
  if (auto it = this->module_bw_list_.find(key); it != this->module_bw_list_.end()) {
    this->module_bw_list_.extract(it);
  }
}

const vda5050pp::config::EventManagerOptions &GlobalConfig::getEventManagerOptions() const {
  return this->event_manager_options_;
}
vda5050pp::config::EventManagerOptions &GlobalConfig::refEventManagerOptions() {
  return this->event_manager_options_;
}
void GlobalConfig::setEventManagerOptions(const vda5050pp::config::EventManagerOptions &opts) {
  this->event_manager_options_ = opts;
}