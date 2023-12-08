//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PRIVATE_VDA5050_2B_2B_CORE_CONFIG_H_
#define PRIVATE_VDA5050_2B_2B_CORE_CONFIG_H_

#include <spdlog/fmt/fmt.h>
#include <toml++/toml.h>

#include "vda5050++/config.h"
#include "vda5050++/core/common/exception.h"

namespace vda5050pp::core::config {

class ConfigNode : public vda5050pp::config::ConfigNode {
private:
  toml::node *node_;

public:
  static inline ConfigNode &upcast(vda5050pp::config::ConfigNode &node) {
    return static_cast<ConfigNode &>(node);
  }
  static inline const ConfigNode &upcast(const vda5050pp::config::ConfigNode &node) {
    return static_cast<const ConfigNode &>(node);
  }

  explicit ConfigNode(toml::node &node) : node_(&node) {}
  explicit ConfigNode(toml::node *node) : node_(node) {}

  toml::node_view<toml::node> get();
  toml::node_view<const toml::node> get() const;
};

}  // namespace vda5050pp::core::config

#endif  // PRIVATE_VDA5050_2B_2B_CORE_CONFIG_H_
