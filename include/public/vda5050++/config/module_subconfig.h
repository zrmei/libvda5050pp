//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_CONFIG_MODULE_SUBCONFIG_H_
#define PUBLIC_VDA5050_2B_2B_CONFIG_MODULE_SUBCONFIG_H_

#include "vda5050++/config/logging_subconfig.h"
#include "vda5050++/config/types.h"

namespace vda5050pp::config {

///\brief a specialized SubConfig for modules. Currently it only wraps LoggingSubConfig.
/// Further common traits might be added later.
class ModuleSubConfig : public vda5050pp::config::LoggingSubConfig {
protected:
  ///\brief Read all common module settings from a config node.
  /// NOTE: If you overwrite this function, make sure to call it from within that function.
  ///\param node  the config node
  void getFrom(const ConfigNode &node) override;

  ///\brief Write all common module settings to a config node.
  /// NOTE: If you overwrite this function, make sure to call it from within that function.
  ///\param node the config node
  void putTo(ConfigNode &node) const override;
};

}  // namespace vda5050pp::config

#endif  // PUBLIC_VDA5050_2B_2B_CONFIG_MODULE_SUBCONFIG_H_
