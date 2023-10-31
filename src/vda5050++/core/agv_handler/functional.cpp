//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/agv_handler/functional.h"

#include "vda5050++/core/common/math/geometry.h"

namespace math = vda5050pp::core::common::math;

bool vda5050pp::core::agv_handler::isOnNode(const vda5050::AGVPosition &agv,
                                            const vda5050::NodePosition &node,
                                            double default_deviation_xy,
                                            double default_deviation_theta,
                                            std::optional<double> override_deviation_xy,
                                            std::optional<double> override_deviation_theta) {
  if (agv.mapId != node.mapId) {
    return false;
  }

  math::Vector2<double> a_pos{agv.x, agv.y};
  math::Circle<double> a_range{a_pos, agv.deviationRange.value_or(0)};

  math::Vector2<double> n_pos{node.x, node.y};
  math::Circle<double> n_range{n_pos, override_deviation_xy.value_or(
                                          node.allowedDeviationXY.value_or(default_deviation_xy))};

  if (!math::circleEnclosureOf(n_range, a_range)) {
    return false;
  }

  if (node.theta.has_value()) {
    auto theta_diff = math::angleDifference(agv.theta, *node.theta);
    if (std::abs(theta_diff) > override_deviation_theta.value_or(
                                   node.allowedDeviationTheta.value_or(default_deviation_theta))) {
      return false;
    }
  }

  return true;
}