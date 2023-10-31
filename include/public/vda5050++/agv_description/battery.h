// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
//
// This file contains the Battery data structure
//

#ifndef PUBLIC_VDA5050_2B_2B_AGV_DESCRIPTION_BATTERY_H_
#define PUBLIC_VDA5050_2B_2B_AGV_DESCRIPTION_BATTERY_H_

#include <cstdint>
#include <optional>
#include <string>

namespace vda5050pp::agv_description {

///
///\brief A battery description of an AGV
///
struct Battery {
  ///\brief [m] Maximum reach with full Battery
  std::optional<uint32_t> max_reach;
  ///\brief [Ah] Maximum Battery Charge
  double max_charge;
  ///\brief [V] Maximum Battery Voltage
  double max_voltage;
  ///
  ///\brief The charge type of the battery
  ///
  /// I.e. what kind of charge station can be used.
  ///
  std::string charge_type;
};
}  // namespace vda5050pp::agv_description

#endif  // PUBLIC_VDA5050_2B_2B_AGV_DESCRIPTION_BATTERY_H_
