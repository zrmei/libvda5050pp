//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_CONFIG_MQTT_SUBCONFIG_H_
#define PUBLIC_VDA5050_2B_2B_CONFIG_MQTT_SUBCONFIG_H_

#include "vda5050++/config/module_subconfig.h"
#include "vda5050++/config/mqtt_options.h"

namespace vda5050pp::config {

///
///\brief The MqttModule's SubConfig. Contains all options for the mqtt connection.
///
class MqttSubConfig : public vda5050pp::config::ModuleSubConfig {
private:
  MqttOptions options_;

protected:
  ///
  ///\brief Read the MqttOptions object from a ConfigNode.
  ///
  ///\param node the ConfigNode to read from.
  ///
  void getFrom(const ConstConfigNode &node) override;

  ///
  ///\brief Write the MqttOptions object to a ConfigNode.
  ///
  ///\param node the ConfigNode to write to.
  ///
  void putTo(ConfigNode &node) const override;

public:
  ///
  ///\brief Set new MqttOptions.
  ///
  ///\param options the new MqttOptions.
  ///
  void setOptions(const MqttOptions &options);

  ///
  ///\brief Get the current MqttOptions.
  ///
  ///\return const MqttOptions&
  ///
  const MqttOptions &getOptions() const;

  ///
  ///\brief Get a writeable reference to the current MqttOptions.
  ///
  ///\return MqttOptions&
  ///
  MqttOptions &refOptions();
};

}  // namespace vda5050pp::config

#endif  // PUBLIC_VDA5050_2B_2B_CONFIG_MQTT_SUBCONFIG_H_
