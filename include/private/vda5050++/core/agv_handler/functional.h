//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_AGV_HANDLER_FUNCTIONAL_H_
#define VDA5050_2B_2B_CORE_AGV_HANDLER_FUNCTIONAL_H_

#include <vda5050/AGVPosition.h>
#include <vda5050/NodePosition.h>

#include <optional>

namespace vda5050pp::core::agv_handler {

bool isOnNode(const vda5050::AGVPosition &agv, const vda5050::NodePosition &node,
              double default_deviation_xy, double default_deviation_theta,
              std::optional<double> override_deviation_xy = std::nullopt,
              std::optional<double> override_deviation_theta = std::nullopt);
}

#endif  // VDA5050_2B_2B_CORE_AGV_HANDLER_FUNCTIONAL_H_
