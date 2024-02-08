//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_CONFIG_STATE_UPDATE_TIMER_SUBCONFIG_H_
#define PUBLIC_VDA5050_2B_2B_CONFIG_STATE_UPDATE_TIMER_SUBCONFIG_H_

#include <chrono>

#include "vda5050++/config/module_subconfig.h"

namespace vda5050pp::config {

///
///\brief This is the StateUpdateTimer's SubConfig. It contains the max update period.
///
class StateUpdateTimerSubConfig : public ModuleSubConfig {
private:
  std::chrono::system_clock::duration state_update_period_ = std::chrono::seconds(30);

protected:
  ///
  ///\brief Read this SubConfig from a ConstConfigNode.
  ///
  ///\param node the ConfigNode to read from.
  ///
  void getFrom(const ConstConfigNode &node) override;

  ///
  ///\brief Write this SubConfig to a ConfigNode.
  ///
  ///\param node the ConfigNode to write to.
  ///
  void putTo(ConfigNode &node) const override;

public:
  ///
  ///\brief Set the maximum period between state updates.
  ///
  ///\param period the new update period
  ///
  void setMaxUpdatePeriod(std::chrono::system_clock::duration period);

  ///
  ///\brief Get the current maximum update period.
  ///
  ///\return std::chrono::system_clock::duration
  ///
  std::chrono::system_clock::duration getMaxUpdatePeriod() const;
};

}  // namespace vda5050pp::config

#endif  // PUBLIC_VDA5050_2B_2B_CONFIG_STATE_UPDATE_TIMER_SUBCONFIG_H_
