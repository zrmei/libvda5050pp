//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/misc/any_ptr.h"

#include <catch2/catch.hpp>

TEST_CASE("common::AnyPtr behaviour") {
  auto int_ptr = std::make_shared<int>(7);

  WHEN("An AnyPtr to an existing ptr is created") {
    vda5050pp::misc::AnyPtr any_ptr(int_ptr);

    THEN("The use counts of both are 2") {
      REQUIRE(int_ptr.use_count() == 2);
      REQUIRE(any_ptr.useCount() == 2);
    }

    THEN("The type id is correct") { REQUIRE(any_ptr.typeInfo() == typeid(int)); }

    WHEN("The AnyPtr is casted to the correct type") {
      auto casted_ptr = any_ptr.get<int>();

      THEN("The value is correct") { REQUIRE(*casted_ptr == *int_ptr); }

      THEN("The use_counts are 3") {
        REQUIRE(int_ptr.use_count() == 3);
        REQUIRE(any_ptr.useCount() == 3);
        REQUIRE(casted_ptr.use_count() == 3);
      }
    }

    WHEN("The AnyPtr is casted to the incorrect type") {
      THEN("It throws") {
        REQUIRE_THROWS_AS(any_ptr.get<std::string>(), vda5050pp::misc::BadAnyPtrCast);
        REQUIRE_THROWS_AS((std::shared_ptr<std::string>)any_ptr, vda5050pp::misc::BadAnyPtrCast);
      }
    }

    WHEN("The original pointer is destroyed") {
      int_ptr.reset();

      THEN("The use_count is 1") { REQUIRE(any_ptr.useCount() == 1); }
    }
  }

  WHEN("An AnyPtr to a nullptr is created") {
    vda5050pp::misc::AnyPtr any_ptr(nullptr);

    THEN("The use count is 0") { REQUIRE(any_ptr.useCount() == 0); }
    THEN("The type info is nullptr") { REQUIRE(any_ptr.typeInfo() == typeid(std::nullptr_t)); }
  }
}