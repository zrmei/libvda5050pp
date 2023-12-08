//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_COMMON_FORMATTERS_H_
#define VDA5050_2B_2B_CORE_COMMON_FORMATTERS_H_

#include <spdlog/fmt/fmt.h>
#include <vda5050/Action.h>
#include <vda5050/Error.h>

#include <sstream>

#include "vda5050++/exception.h"
#include "vda5050++/misc/order_status.h"

namespace fmt {

template <> struct formatter<vda5050::BlockingType> : formatter<string_view> {
  template <typename FormatContext> auto format(vda5050::BlockingType b, FormatContext &ctx) const {
    string_view name;
    switch (b) {
      case vda5050::BlockingType::HARD:
        name = "HARD";
        break;
      case vda5050::BlockingType::SOFT:
        name = "SOFT";
        break;
      case vda5050::BlockingType::NONE:
        name = "NONE";
        break;
      default:
        name = "unknown";
        break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};

template <> struct formatter<vda5050pp::misc::OrderStatus> : formatter<string_view> {
  template <typename FormatContext>
  auto format(vda5050pp::misc::OrderStatus s, FormatContext &ctx) const {
    string_view name;
    switch (s) {
      case vda5050pp::misc::OrderStatus::k_order_active:
        name = "Active";
        break;
      case vda5050pp::misc::OrderStatus::k_order_canceling:
        name = "Canceling";
        break;
      case vda5050pp::misc::OrderStatus::k_order_failed:
        name = "Failed";
        break;
      case vda5050pp::misc::OrderStatus::k_order_idle:
        name = "Idle";
        break;
      case vda5050pp::misc::OrderStatus::k_order_idle_paused:
        name = "Idle-Paused";
        break;
      case vda5050pp::misc::OrderStatus::k_order_interrupting:
        name = "Interrupting";
        break;
      case vda5050pp::misc::OrderStatus::k_order_paused:
        name = "Paused";
        break;
      case vda5050pp::misc::OrderStatus::k_order_pausing:
        name = "Pausing";
        break;
      case vda5050pp::misc::OrderStatus::k_order_resuming:
        name = "Resuming";
        break;
      default:
        name = "unknown";
        break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};

template <> struct formatter<vda5050pp::VDA5050PPError> : formatter<string_view> {
  template <typename FormatContext>
  auto format(const vda5050pp::VDA5050PPError &err, FormatContext &ctx) const {
    return formatter<string_view>::format(err.dump(), ctx);
  }
};

template <typename T, size_t n> struct formatter<std::array<T, n>> : formatter<string_view> {
  template <typename FormatContext>
  auto format(const std::array<T, n> &arr, FormatContext &ctx) const {
    if (arr.size() == 0) {
      return formatter<string_view>::format("[]", ctx);
    }

    std::stringstream ss;
    ss << fmt::format("[{}", arr[0]);
    for (size_t i = 1; i < n; ++i) {
      ss << fmt::format(", {}", arr[0]);
    }
    ss << "]";
    return formatter<string_view>::format(ss.str(), ctx);
  }
};

template <> struct formatter<vda5050::Error> : formatter<string_view> {
  template <typename FormatContext>
  auto format(const vda5050::Error &err, FormatContext &ctx) const {
    return formatter<string_view>::format(vda5050::json(err).dump(2), ctx);
  }
};

}  // namespace fmt

#endif  // VDA5050_2B_2B_CORE_COMMON_FORMATTERS_H_
