// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains the SimpleProtocolLimits struct.
//

#ifndef PUBLIC_VDA5050_2B_2B_AGV_DESCRIPTION_SIMPLE_PROTOCOL_LIMITS_H_
#define PUBLIC_VDA5050_2B_2B_AGV_DESCRIPTION_SIMPLE_PROTOCOL_LIMITS_H_

#include <cstdint>
#include <optional>

namespace vda5050pp::agv_description {

///
///\brief This Struct contains a subset of the vda5050::ProtocolLimits.
/// It only contains the fields, which concern the user of this library.
///
struct SimpleProtocolLimits {
  std::optional<uint32_t> max_id_len;
  std::optional<bool> id_numerical_only;
  std::optional<uint32_t> max_load_id_len;
  std::optional<uint32_t> max_loads;
};

}  // namespace vda5050pp::agv_description

#endif  // PUBLIC_VDA5050_2B_2B_AGV_DESCRIPTION_SIMPLE_PROTOCOL_LIMITS_H_
