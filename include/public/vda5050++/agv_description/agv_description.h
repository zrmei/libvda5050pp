// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_AGV_DESCRIPTION_AGV_DESCRIPTION_H_
#define PUBLIC_VDA5050_2B_2B_AGV_DESCRIPTION_AGV_DESCRIPTION_H_

#include <vda5050/AgvGeometry.h>
#include <vda5050/LoadSpecification.h>
#include <vda5050/LocalizationParameters.h>
#include <vda5050/PhysicalParameters.h>
#include <vda5050/TypeSpecification.h>

#include <cstdint>
#include <optional>
#include <set>
#include <string>

#include "vda5050++/agv_description/action_declaration.h"
#include "vda5050++/agv_description/battery.h"
#include "vda5050++/agv_description/simple_protocol_limits.h"

namespace vda5050pp::agv_description {

/// \brief The library configuration
struct AGVDescription {
  /// Use a UUID v5 for unique AGV ID (required for the MQTT topic)
  std::string agv_id;

  /// Manufacturer string (required for the MQTT topic)
  std::string manufacturer;

  /// Serial number (required for the MQTT topic)
  std::string serial_number;

  /// An additional description of the AGV
  std::optional<std::string> description;

  /// Battery Information (unused by the VDA5050)
  Battery battery;

  vda5050::TypeSpecification type_specification;

  vda5050::PhysicalParameters physical_parameters;

  vda5050::AgvGeometry agv_geometry;

  vda5050::LoadSpecification load_specification;

  vda5050::LocalizationParameters localization_parameters;

  SimpleProtocolLimits simple_protocol_limits;
};

}  // namespace vda5050pp::agv_description
#endif  // PUBLIC_VDA5050_2B_2B_AGV_DESCRIPTION_AGV_DESCRIPTION_H_
