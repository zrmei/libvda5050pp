// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains factsheet gather functions.
//
//

#ifndef VDA5050_2B_2B_CORE_FACTSHEET_GATHER_H_
#define VDA5050_2B_2B_CORE_FACTSHEET_GATHER_H_

#include "vda5050/AgvFactsheet.h"

namespace vda5050pp::core::factsheet {

vda5050::AgvGeometry gatherGeometry();
vda5050::LoadSpecification gatherLoadSpecification();
vda5050::LocalizationParameters gatherLocalizationParameters();
vda5050::PhysicalParameters gatherPhysicalParameters();
vda5050::ProtocolFeatures gatherProtocolFeatures();
vda5050::ProtocolLimits gatherProtocolLimits();
vda5050::TypeSpecification gatherTypeSpecification();

}  // namespace vda5050pp::core::factsheet

#endif  // VDA5050_2B_2B_CORE_FACTSHEET_GATHER_H_
