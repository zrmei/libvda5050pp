//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/config.h"
using namespace vda5050pp::core::config;

toml::node_view<toml::node> ConfigNode::get() { return toml::node_view(this->node_); }
toml::node_view<const toml::node> ConfigNode::get() const {
  return toml::node_view<const toml::node>(this->node_);
}