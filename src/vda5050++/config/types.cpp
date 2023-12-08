//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/config/types.h"

#include <spdlog/fmt/fmt.h>

#include "vda5050++/core/common/exception.h"

vda5050pp::VDA5050PPBadCast vda5050pp::config::SubConfig::badCast(const std::type_info &is,
                                                                  const std::type_info &got) {
  return vda5050pp::VDA5050PPBadCast(
      MK_FN_EX_CONTEXT(fmt::format("Could not cast from {} to {}", is.name(), got.name())));
}