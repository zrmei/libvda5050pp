//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef TEST_INCLUDE_TEST_DATA_H_
#define TEST_INCLUDE_TEST_DATA_H_

#include <vda5050/Order.h>

#include <string_view>

namespace test::data {

template <typename T> inline auto wrap_shared(T &&value) {
  return std::make_shared<std::decay_t<T>>(std::forward<T>(value));
}

struct TemplateElement {
  std::string_view id;
  uint32_t seq_id;
  bool released;
  std::vector<vda5050::Action> actions;
};

vda5050::Action mkAction(std::string_view id, std::string_view type, vda5050::BlockingType b_type);
vda5050::Edge mkEdge(std::string_view id, uint32_t seq_id, bool released,
                     std::vector<vda5050::Action> &&actions);
vda5050::Node mkNode(std::string_view id, uint32_t seq_id, bool released,
                     std::vector<vda5050::Action> &&actions);
vda5050::Order mkTemplateOrder(std::vector<TemplateElement> &&elements) noexcept(false);

}  // namespace test::data

#endif  // TEST_INCLUDE_TEST_DATA_H_
