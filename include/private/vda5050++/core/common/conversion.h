// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains declarations of various conversion functions
//
//

#ifndef VDA5050_2B_2B_CORE_COMMON_CONVERSION_H_
#define VDA5050_2B_2B_CORE_COMMON_CONVERSION_H_

#include <vda5050/AgvAction.h>

#include "vda5050++/agv_description/agv_description.h"

namespace vda5050pp::core::common {

vda5050::AgvAction fromActionDeclaration(
    const vda5050pp::agv_description::ActionDeclaration &decl) noexcept(false);

}

#endif  // VDA5050_2B_2B_CORE_COMMON_CONVERSION_H_
