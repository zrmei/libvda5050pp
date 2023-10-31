//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_COMMON_CONTAINER_H_
#define VDA5050_2B_2B_CORE_COMMON_CONTAINER_H_

#include <algorithm>

namespace vda5050pp::core::common {

template <typename ContainerT, typename ValueT>
constexpr bool contains(const ContainerT &container, const ValueT &value) noexcept(true) {
  return std::find(cbegin(container), cend(container), value) != cend(container);
}

}  // namespace vda5050pp::core::common

#endif  // VDA5050_2B_2B_CORE_COMMON_CONTAINER_H_
