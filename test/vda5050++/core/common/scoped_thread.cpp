//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/core/common/scoped_thread.h"

#include <catch2/catch.hpp>
#include <chrono>
#include <memory>

using namespace vda5050pp::core::common;
using namespace std::chrono_literals;

TEST_CASE("core::common::ScopedThread - RAII check", "[core][common]") {
  WHEN("A scoped thread with autostart=true is created") {
    bool called = false;
    ScopedThread<void()> thread([&called](StopToken) { called = true; });

    WHEN("It is started a twice") {
      THEN("It throws") { REQUIRE_THROWS_AS(thread.start(), ScopedThreadAlreadyStarted); }
    }

    THEN("The thread executes") {
      thread.join();
      REQUIRE(called);
    }
  }

  WHEN("A blocking thread runs in its own scope") {
    bool returned = false;
    bool tkn_stop_called = false;
    auto fn = [&returned, &tkn_stop_called](StopToken tkn) {
      tkn.onStopRequested([&tkn_stop_called] { tkn_stop_called = true; });
      while (!tkn.stopRequested()) {
        std::this_thread::sleep_for(50ms);
      }
      returned = true;
    };

    auto t_ptr = std::make_unique<ScopedThread<void()>>(fn);

    THEN("It has not returned yet") {
      std::this_thread::sleep_for(200ms);
      REQUIRE_FALSE(returned);
    }

    WHEN("It runs out of scope") {
      std::this_thread::sleep_for(200ms);
      t_ptr.reset();

      THEN("The StopToken makes the thread finish") { REQUIRE(returned); }
      THEN("The StopToken's on_stop was called") { REQUIRE(tkn_stop_called); }
    }
  }
}

TEST_CASE("core::common::ScopedThread - Future check", "[core][common]") {
  WHEN("A scoped thread with a returning task is executed") {
    auto task = [](StopToken tkn, auto p1, auto p2) -> float {
      while (!tkn.stopRequested()) {
        std::this_thread::sleep_for(50ms);
      }

      return float(p1) + float(p2);
    };

    ScopedThread<float(int, float)> thread(task, 20, 25.5);
    auto future_result = thread.getFuture();

    WHEN("The thread finishes execution") {
      thread.stop();
      thread.join();

      THEN("The future result is available") { REQUIRE(future_result.get() == 45.5); }
    }
  }
}