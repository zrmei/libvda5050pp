//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_CHECKS_ORDER_H_
#define VDA5050_2B_2B_CORE_CHECKS_ORDER_H_

#include <vda5050/Error.h>
#include <vda5050/Order.h>

#include <list>

namespace vda5050pp::core::checks {

std::list<vda5050::Error> checkOrderGraphConsistency(const vda5050::Order &order);

std::list<vda5050::Error> checkOrderId(const vda5050::Order &order);

std::list<vda5050::Error> checkOrderAppend(const vda5050::Order &order);

std::list<vda5050::Error> checkOrderActionIds(const vda5050::Order &order);

}  // namespace vda5050pp::core::checks

#endif  // VDA5050_2B_2B_CORE_CHECKS_ORDER_H_
