//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/events/synchronized_event.h"

#include <catch2/catch_all.hpp>

using namespace std::chrono_literals;

TEST_CASE("SynchronizedEvent behaviour", "[events]") {
  std::string_view str1 = "4leRTxMmUUSs";

  vda5050pp::events::SynchronizedEvent<std::string_view> event1;
  vda5050pp::events::SynchronizedEvent<void> event2;

  // Empty acquire does not cause problems
  { event1.acquireResultToken(); }

  WHEN("The Resul is set") {
    event1.acquireResultToken().setValue(str1);
    event2.acquireResultToken().setValue();

    THEN("Setting the result a second time fails") {
      REQUIRE_THROWS_AS(event1.acquireResultToken().setException(nullptr),
                        vda5050pp::VDA5050PPSynchronizedEventNotAcquired);
      REQUIRE_THROWS_AS(event1.acquireResultToken().setValue(str1),
                        vda5050pp::VDA5050PPSynchronizedEventNotAcquired);
      REQUIRE_THROWS_AS(event2.acquireResultToken().setException(nullptr),
                        vda5050pp::VDA5050PPSynchronizedEventNotAcquired);
      REQUIRE_THROWS_AS(event2.acquireResultToken().setValue(),
                        vda5050pp::VDA5050PPSynchronizedEventNotAcquired);
    }
  }

  WHEN("The event was acquired") {
    auto result1 = event1.acquireResultToken();
    auto result2 = event2.acquireResultToken();

    THEN("The Token returns acquired") {
      REQUIRE(result1.isAcquired());
      REQUIRE(result2.isAcquired());
    }

    WHEN("The event1 is tried to acquired a second time") {
      auto result12 = event1.acquireResultToken();
      auto result22 = event2.acquireResultToken();
      THEN("The Tokens does not return acquired") {
        REQUIRE_FALSE(result12.isAcquired());
        REQUIRE_FALSE(result22.isAcquired());
      }
      THEN("All operations throw") {
        REQUIRE_THROWS_AS(result12.setException(nullptr),
                          vda5050pp::VDA5050PPSynchronizedEventNotAcquired);
        REQUIRE_THROWS_AS(result12.setValue(str1),
                          vda5050pp::VDA5050PPSynchronizedEventNotAcquired);
        REQUIRE_THROWS_AS(result22.setException(nullptr),
                          vda5050pp::VDA5050PPSynchronizedEventNotAcquired);
        REQUIRE_THROWS_AS(result22.setValue(), vda5050pp::VDA5050PPSynchronizedEventNotAcquired);
      }
    }

    WHEN("The result is set") {
      result1.setValue(str1);
      result2.setValue();

      THEN("The future has the result") {
        auto future1 = event1.getFuture();
        auto future2 = event2.getFuture();
        REQUIRE(future1.wait_for(100ms) == std::future_status::ready);
        REQUIRE(future1.get() == str1);
        REQUIRE(future2.wait_for(100ms) == std::future_status::ready);
      }
    }

    WHEN("An exception is set") {
      auto exception = std::make_exception_ptr(std::logic_error("test"));
      result1.setException(exception);
      result2.setException(exception);

      THEN("The future has the exception") {
        auto future1 = event1.getFuture();
        auto future2 = event2.getFuture();
        REQUIRE(future1.wait_for(100ms) == std::future_status::ready);
        REQUIRE_THROWS_AS(future1.get(), std::logic_error);
        REQUIRE(future2.wait_for(100ms) == std::future_status::ready);
        REQUIRE_THROWS_AS(future2.get(), std::logic_error);
      }
    }

    WHEN("The token is released") {
      result1.release();
      result2.release();

      THEN("All operations throw") {
        REQUIRE_THROWS_AS(result1.setException(nullptr),
                          vda5050pp::VDA5050PPSynchronizedEventNotAcquired);
        REQUIRE_THROWS_AS(result1.setValue(str1), vda5050pp::VDA5050PPSynchronizedEventNotAcquired);
        REQUIRE_THROWS_AS(result2.setException(nullptr),
                          vda5050pp::VDA5050PPSynchronizedEventNotAcquired);
        REQUIRE_THROWS_AS(result2.setValue(), vda5050pp::VDA5050PPSynchronizedEventNotAcquired);
      }
      WHEN("The event is tried to acquired a second time") {
        auto result12 = event1.acquireResultToken();
        auto result22 = event2.acquireResultToken();
        THEN("The Tokens return acquired") {
          REQUIRE(result12.isAcquired());
          REQUIRE(result22.isAcquired());
        }
        WHEN("The result is set") {
          result12.setValue(str1);
          result22.setValue();

          THEN("The future has the result") {
            auto future1 = event1.getFuture();
            auto future2 = event2.getFuture();
            REQUIRE(future1.wait_for(100ms) == std::future_status::ready);
            REQUIRE(future1.get() == str1);
            REQUIRE(future2.wait_for(100ms) == std::future_status::ready);
          }
        }
      }
    }
  }
}
