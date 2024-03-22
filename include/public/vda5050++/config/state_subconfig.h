// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_CONFIG_STATE_SUBCONFIG_H_
#define PUBLIC_VDA5050_2B_2B_CONFIG_STATE_SUBCONFIG_H_

#include <optional>

#include "vda5050++/config/module_subconfig.h"

namespace vda5050pp::config {

class StateSubConfig : public ModuleSubConfig {
private:
  std::optional<std::string> default_agv_position_map_;
  bool use_agv_position_from_order_ = false;

protected:
  ///
  ///\brief Read the values from a ConfigNode.
  ///
  ///\param node the ConfigNode to read from.
  ///
  void getFrom(const ConstConfigNode &node) override;

  ///
  ///\brief Write the values to a ConfigNode.
  ///
  ///\param node the ConfigNode to write to.
  ///
  void putTo(ConfigNode &node) const override;

public:
  ///
  ///\brief Get the default agv position map.
  ///
  ///\return std::optional<std::string>
  ///
  std::optional<std::string> getDefaultAgvPositionMap() const;

  ///
  ///\brief Set the default agv position map.
  ///
  ///\param default_agv_position_map
  ///
  void setDefaultAgvPositionMap(const std::optional<std::string> &default_agv_position_map);

  ///
  ///\brief Check if the agv position should be taken from the order.
  ///
  ///\return bool
  ///
  bool getUseAgvPositionFromOrder() const;

  ///
  ///\brief Set if the agv position should be taken from the order.
  ///
  ///\param use_agv_position_from_order take from order
  ///
  void setUseAgvPositionFromOrder(bool use_agv_position_from_order);
};

}  // namespace vda5050pp::config

#endif  // PUBLIC_VDA5050_2B_2B_CONFIG_STATE_SUBCONFIG_H_
