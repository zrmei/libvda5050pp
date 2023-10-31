//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef TEST_INCLUDE_TEST_EXCEPTION_H_
#define TEST_INCLUDE_TEST_EXCEPTION_H_

#include <exception>
#include <string>
#include <type_traits>
#include <variant>

namespace test::exception {

class VDA5050TestException : public std::logic_error {
private:
  std::variant<std::string, const char *> msg_;

public:
  template <typename StringT>
  VDA5050TestException(StringT &&msg)
      : msg_(std::forward<StringT>(msg)), std::logic_error(what()) {}

  const char *what() const noexcept(true) override {
    return std::visit(
        [](auto &&str) {
          using T = std::decay_t<decltype(str)>;

          if constexpr (std::is_same_v<T, std::string>) {
            return str.c_str();
          } else {
            return str;
          }
        },
        this->msg_);
  }
};

}  // namespace test::exception

#endif  // TEST_INCLUDE_TEST_EXCEPTION_H_
