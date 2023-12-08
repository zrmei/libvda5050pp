//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef TEST_INCLUDE_TEST_EXCEPTION_H_
#define TEST_INCLUDE_TEST_EXCEPTION_H_

#include <exception>
#include <stdexcept>
#include <string>

namespace test::exception {

class VDA5050TestException : public std::logic_error {
private:
  std::string msg_;

public:
  template <typename StringT>
  VDA5050TestException(const StringT &msg) : std::logic_error(""), msg_(msg) {}

  const char *what() const noexcept(true) override {
    return this->msg_.c_str();
  }
};

}  // namespace test::exception

#endif  // TEST_INCLUDE_TEST_EXCEPTION_H_
