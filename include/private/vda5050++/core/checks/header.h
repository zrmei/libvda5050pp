//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_CHECKS_HEADER_H_
#define VDA5050_2B_2B_CORE_CHECKS_HEADER_H_

#include <vda5050/Error.h>
#include <vda5050/Header_vda5050.h>

#include <list>

namespace vda5050pp::core::checks {

std::list<vda5050::Error> checkHeader(const vda5050::HeaderVDA5050 &header);

}

#endif  // VDA5050_2B_2B_CORE_CHECKS_HEADER_H_
