//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_CONFIG_NODE_REACHED_SUBCONFIG_H_
#define PUBLIC_VDA5050_2B_2B_CONFIG_NODE_REACHED_SUBCONFIG_H_

#include <optional>

#include "vda5050++/config/module_subconfig.h"

namespace vda5050pp::config {

///
///\brief The NodeReachedHandler's SubConfig.
///
/// Contains:
///   - Default node deviations to use, if they were not given by the order.
///   - Overwrite node deviations, which will always be used.
///
class NodeReachedSubConfig : public ModuleSubConfig {
private:
  double default_node_deviation_xy_ = 0.5;
  double default_node_deviation_theta_ = 0.1;
  std::optional<double> overwrite_node_deviation_xy_;
  std::optional<double> overwrite_node_deviation_theta_;

protected:
  ///
  ///\brief Read the deviation values from a ConfigNode.
  ///
  ///\param node the ConfigNode to read from.
  ///
  void getFrom(const ConfigNode &node) override;

  ///
  ///\brief Write the deviation values to a ConfigNode.
  ///
  ///\param node the ConfigNode to write to.
  ///
  void putTo(ConfigNode &node) const override;

public:
  ///
  ///\brief Set the default node XY deviation.
  ///
  ///\param new_value new xy deviation.
  ///
  void setDefaultNodeDeviationXY(double new_value);

  ///
  ///\brief Set the default node theta deviation.
  ///
  ///\param new_value new theta deviation [rad].
  ///
  void setDefaultNodeDeviationTheta(double new_value);

  ///
  ///\brief Set the node XY deviation to use for ALL nodes.
  ///
  ///\param new_value new xy deviation (std::nullopt for no overwrite).
  ///
  void setOverwriteNodeDeviationXY(std::optional<double> new_value);

  ///
  ///\brief Set the node theta deviation to use for ALL nodes.
  ///
  ///\param new_value new theta deviation (std::nullopt for no overwrite).
  ///
  void setOverwriteNodeDeviationTheta(std::optional<double> new_value);

  ///
  ///\brief Get the default node XY deviation.
  ///
  ///\return double
  ///
  double getDefaultNodeDeviationXY() const;

  ///
  ///\brief Get the default node theta deviation.
  ///
  ///\return double
  ///
  double getDefaultNodeDeviationTheta() const;

  ///
  ///\brief Get the overwrite node xy deviation.
  ///
  ///\return std::optional<double>
  ///
  std::optional<double> getOverwriteNodeDeviationXY() const;

  ///
  ///\brief Get the overwrite node theta deviation.
  ///
  ///\return std::optional<double>
  ///
  std::optional<double> getOverwriteNodeDeviationTheta() const;
};

}  // namespace vda5050pp::config

#endif  // PUBLIC_VDA5050_2B_2B_CONFIG_NODE_REACHED_SUBCONFIG_H_
