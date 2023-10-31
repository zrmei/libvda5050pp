// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_COMMON_FUNCTIONAL_H_
#define VDA5050_2B_2B_CORE_COMMON_FUNCTIONAL_H_

namespace vda5050pp::core::common {

template <auto Function, typename... BindArgs> auto bindLeft(BindArgs &&...args) {
  using FunctionT = decltype(Function);

  if constexpr (std::is_pointer_v<FunctionT> || std::is_member_pointer_v<FunctionT>) {
    static_assert(Function != nullptr);
  }

  return [bound_args = std::forward<BindArgs>(args)](auto &&...fwd_args) {
    return std::invoke(std::forward<FunctionT>(Function), std::forward<BindArgs>(bound_args)...,
                       std::forward<decltype(remainingArgs)>(remainingArgs)...);
  };
}

}  // namespace vda5050pp::core::common

#endif  // VDA5050_2B_2B_CORE_COMMON_FUNCTIONAL_H_
