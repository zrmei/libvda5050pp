// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains the VisualizationTimerSubconfig declaration
//
//

#ifndef PUBLIC_VDA5050_2B_2B_CONFIG_VISUALIZATION_TIMER_SUBCONFIG_H_
#define PUBLIC_VDA5050_2B_2B_CONFIG_VISUALIZATION_TIMER_SUBCONFIG_H_

#include <chrono>

#include "vda5050++/config/module_subconfig.h"

namespace vda5050pp::config {

///
///\brief The VisualizationTimer's SubConfig. It contains the visualization update period.
///
class VisualizationTimerSubConfig : public vda5050pp::config::ModuleSubConfig {
private:
  std::chrono::system_clock::duration visualization_period_ = std::chrono::seconds(1);

public:
  ///
  ///\brief Read this SubConfig from a ConstConfigNode.
  ///
  ///\param node The ConfigNode to read from.
  ///
  void getFrom(const ConstConfigNode &node) override;

  ///
  ///\brief Write this SubConfig to a ConfigNode.
  ///
  ///\param node The ConfigNode to write to.
  ///
  void putTo(ConfigNode &node) const override;

  ///
  ///\brief Get the current visualization period.
  ///
  ///\return std::chrono::system_clock::duration
  ///
  std::chrono::system_clock::duration getVisualizationPeriod() const;

  ///
  ///\brief Set the new visualization period. Determines the rate of visualization messages.
  ///
  ///\param new_value
  ///
  void setVisualizationPeriod(std::chrono::system_clock::duration new_value);
};

}  // namespace vda5050pp::config

#endif  // PUBLIC_VDA5050_2B_2B_CONFIG_VISUALIZATION_TIMER_SUBCONFIG_H_
