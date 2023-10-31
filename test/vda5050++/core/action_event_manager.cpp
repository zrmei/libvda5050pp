//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/core/action_event_manager.h"

#include <catch2/catch.hpp>
#include <chrono>

using namespace std::chrono_literals;

template <typename DataT> class TestCallback {
private:
  bool called_ = false;

public:
  bool wasCalled() { return this->called_; }

  void operator()(std::shared_ptr<DataT>) { this->called_ = true; }
};

TEST_CASE("ActionEventManager dispatches callbacks", "[core][ActionEventManager]") {
  vda5050pp::config::EventManagerOptions opts;
  vda5050pp::core::ActionEventManager manager(opts);

  TestCallback<vda5050pp::events::ActionPrepare> action_prepare_callback_1;
  TestCallback<vda5050pp::events::ActionPrepare> action_prepare_callback_2;
  TestCallback<vda5050pp::events::ActionPrepare> action_prepare_callback_3;
  TestCallback<vda5050pp::events::ActionStart> action_start_callback_1;
  TestCallback<vda5050pp::events::ActionStart> action_start_callback_2;
  TestCallback<vda5050pp::events::ActionStart> action_start_callback_3;
  TestCallback<vda5050pp::events::ActionPause> action_pause_callback_1;
  TestCallback<vda5050pp::events::ActionPause> action_pause_callback_2;
  TestCallback<vda5050pp::events::ActionPause> action_pause_callback_3;
  TestCallback<vda5050pp::events::ActionResume> action_resume_callback_1;
  TestCallback<vda5050pp::events::ActionResume> action_resume_callback_2;
  TestCallback<vda5050pp::events::ActionResume> action_resume_callback_3;
  TestCallback<vda5050pp::events::ActionCancel> action_cancel_callback_1;
  TestCallback<vda5050pp::events::ActionCancel> action_cancel_callback_2;
  TestCallback<vda5050pp::events::ActionCancel> action_cancel_callback_3;

  auto callback_manager = manager.getScopedActionEventSubscriber();
  callback_manager.subscribe(std::ref(action_prepare_callback_1));
  callback_manager.subscribe(std::ref(action_prepare_callback_2));
  callback_manager.subscribe(std::ref(action_prepare_callback_3));
  callback_manager.subscribe(std::ref(action_start_callback_1));
  callback_manager.subscribe(std::ref(action_start_callback_2));
  callback_manager.subscribe(std::ref(action_start_callback_3));
  callback_manager.subscribe(std::ref(action_pause_callback_1));
  callback_manager.subscribe(std::ref(action_pause_callback_2));
  callback_manager.subscribe(std::ref(action_pause_callback_3));
  callback_manager.subscribe(std::ref(action_resume_callback_1));
  callback_manager.subscribe(std::ref(action_resume_callback_2));
  callback_manager.subscribe(std::ref(action_resume_callback_3));
  callback_manager.subscribe(std::ref(action_cancel_callback_1));
  callback_manager.subscribe(std::ref(action_cancel_callback_2));
  callback_manager.subscribe(std::ref(action_cancel_callback_3));

  SECTION("Initially no callbacks were called") {
    REQUIRE_FALSE(action_prepare_callback_1.wasCalled());
    REQUIRE_FALSE(action_prepare_callback_2.wasCalled());
    REQUIRE_FALSE(action_prepare_callback_3.wasCalled());
    REQUIRE_FALSE(action_start_callback_1.wasCalled());
    REQUIRE_FALSE(action_start_callback_2.wasCalled());
    REQUIRE_FALSE(action_start_callback_3.wasCalled());
    REQUIRE_FALSE(action_pause_callback_1.wasCalled());
    REQUIRE_FALSE(action_pause_callback_2.wasCalled());
    REQUIRE_FALSE(action_pause_callback_3.wasCalled());
    REQUIRE_FALSE(action_resume_callback_1.wasCalled());
    REQUIRE_FALSE(action_resume_callback_2.wasCalled());
    REQUIRE_FALSE(action_resume_callback_3.wasCalled());
    REQUIRE_FALSE(action_cancel_callback_1.wasCalled());
    REQUIRE_FALSE(action_cancel_callback_2.wasCalled());
    REQUIRE_FALSE(action_cancel_callback_3.wasCalled());
  }

  SECTION("After dispatching a prepare event, all start callbacks were called") {
    manager.dispatch(std::make_shared<vda5050pp::events::ActionPrepare>());
    std::this_thread::sleep_for(200ms);

    REQUIRE(action_prepare_callback_1.wasCalled());
    REQUIRE(action_prepare_callback_2.wasCalled());
    REQUIRE(action_prepare_callback_3.wasCalled());
    REQUIRE_FALSE(action_start_callback_1.wasCalled());
    REQUIRE_FALSE(action_start_callback_2.wasCalled());
    REQUIRE_FALSE(action_start_callback_3.wasCalled());
    REQUIRE_FALSE(action_pause_callback_1.wasCalled());
    REQUIRE_FALSE(action_pause_callback_2.wasCalled());
    REQUIRE_FALSE(action_pause_callback_3.wasCalled());
    REQUIRE_FALSE(action_resume_callback_1.wasCalled());
    REQUIRE_FALSE(action_resume_callback_2.wasCalled());
    REQUIRE_FALSE(action_resume_callback_3.wasCalled());
    REQUIRE_FALSE(action_cancel_callback_1.wasCalled());
    REQUIRE_FALSE(action_cancel_callback_2.wasCalled());
    REQUIRE_FALSE(action_cancel_callback_3.wasCalled());
  }

  SECTION("After dispatching a start event, all start callbacks were called") {
    manager.dispatch(std::make_shared<vda5050pp::events::ActionStart>());
    std::this_thread::sleep_for(200ms);

    REQUIRE_FALSE(action_prepare_callback_1.wasCalled());
    REQUIRE_FALSE(action_prepare_callback_2.wasCalled());
    REQUIRE_FALSE(action_prepare_callback_3.wasCalled());
    REQUIRE(action_start_callback_1.wasCalled());
    REQUIRE(action_start_callback_2.wasCalled());
    REQUIRE(action_start_callback_3.wasCalled());
    REQUIRE_FALSE(action_pause_callback_1.wasCalled());
    REQUIRE_FALSE(action_pause_callback_2.wasCalled());
    REQUIRE_FALSE(action_pause_callback_3.wasCalled());
    REQUIRE_FALSE(action_resume_callback_1.wasCalled());
    REQUIRE_FALSE(action_resume_callback_2.wasCalled());
    REQUIRE_FALSE(action_resume_callback_3.wasCalled());
    REQUIRE_FALSE(action_cancel_callback_1.wasCalled());
    REQUIRE_FALSE(action_cancel_callback_2.wasCalled());
    REQUIRE_FALSE(action_cancel_callback_3.wasCalled());
  }

  SECTION("After dispatching a pause event, all pause callbacks were called") {
    manager.dispatch(std::make_shared<vda5050pp::events::ActionPause>());
    std::this_thread::sleep_for(200ms);

    REQUIRE_FALSE(action_prepare_callback_1.wasCalled());
    REQUIRE_FALSE(action_prepare_callback_2.wasCalled());
    REQUIRE_FALSE(action_prepare_callback_3.wasCalled());
    REQUIRE_FALSE(action_start_callback_1.wasCalled());
    REQUIRE_FALSE(action_start_callback_2.wasCalled());
    REQUIRE_FALSE(action_start_callback_3.wasCalled());
    REQUIRE(action_pause_callback_1.wasCalled());
    REQUIRE(action_pause_callback_2.wasCalled());
    REQUIRE(action_pause_callback_3.wasCalled());
    REQUIRE_FALSE(action_resume_callback_1.wasCalled());
    REQUIRE_FALSE(action_resume_callback_2.wasCalled());
    REQUIRE_FALSE(action_resume_callback_3.wasCalled());
    REQUIRE_FALSE(action_cancel_callback_1.wasCalled());
    REQUIRE_FALSE(action_cancel_callback_2.wasCalled());
    REQUIRE_FALSE(action_cancel_callback_3.wasCalled());
  }

  SECTION("After dispatching a resume event, all resume callbacks were called") {
    manager.dispatch(std::make_shared<vda5050pp::events::ActionResume>());
    std::this_thread::sleep_for(200ms);

    REQUIRE_FALSE(action_prepare_callback_1.wasCalled());
    REQUIRE_FALSE(action_prepare_callback_2.wasCalled());
    REQUIRE_FALSE(action_prepare_callback_3.wasCalled());
    REQUIRE_FALSE(action_start_callback_1.wasCalled());
    REQUIRE_FALSE(action_start_callback_2.wasCalled());
    REQUIRE_FALSE(action_start_callback_3.wasCalled());
    REQUIRE_FALSE(action_pause_callback_1.wasCalled());
    REQUIRE_FALSE(action_pause_callback_2.wasCalled());
    REQUIRE_FALSE(action_pause_callback_3.wasCalled());
    REQUIRE(action_resume_callback_1.wasCalled());
    REQUIRE(action_resume_callback_2.wasCalled());
    REQUIRE(action_resume_callback_3.wasCalled());
    REQUIRE_FALSE(action_cancel_callback_1.wasCalled());
    REQUIRE_FALSE(action_cancel_callback_2.wasCalled());
    REQUIRE_FALSE(action_cancel_callback_3.wasCalled());
  }

  SECTION("After dispatching a action cancel, all cancel callbacks were called") {
    manager.dispatch(std::make_shared<vda5050pp::events::ActionCancel>());
    std::this_thread::sleep_for(200ms);

    REQUIRE_FALSE(action_prepare_callback_1.wasCalled());
    REQUIRE_FALSE(action_prepare_callback_2.wasCalled());
    REQUIRE_FALSE(action_prepare_callback_3.wasCalled());
    REQUIRE_FALSE(action_start_callback_1.wasCalled());
    REQUIRE_FALSE(action_start_callback_2.wasCalled());
    REQUIRE_FALSE(action_start_callback_3.wasCalled());
    REQUIRE_FALSE(action_pause_callback_1.wasCalled());
    REQUIRE_FALSE(action_pause_callback_2.wasCalled());
    REQUIRE_FALSE(action_pause_callback_3.wasCalled());
    REQUIRE_FALSE(action_resume_callback_1.wasCalled());
    REQUIRE_FALSE(action_resume_callback_2.wasCalled());
    REQUIRE_FALSE(action_resume_callback_3.wasCalled());
    REQUIRE(action_cancel_callback_1.wasCalled());
    REQUIRE(action_cancel_callback_2.wasCalled());
    REQUIRE(action_cancel_callback_3.wasCalled());
  }
}

TEST_CASE("ScopedActionEventCallbackManager removes callbacks", "[core][ActionEventManager]") {
  vda5050pp::config::EventManagerOptions opts;
  vda5050pp::core::ActionEventManager manager(opts);

  TestCallback<vda5050pp::events::ActionPrepare> prepare_callback_1;
  TestCallback<vda5050pp::events::ActionStart> start_callback_1;
  TestCallback<vda5050pp::events::ActionPause> pause_callback_1;
  TestCallback<vda5050pp::events::ActionResume> resume_callback_1;
  TestCallback<vda5050pp::events::ActionCancel> cancel_callback_1;

  auto callback_manager = std::make_shared<vda5050pp::core::ScopedActionEventSubscriber>(
      manager.getScopedActionEventSubscriber());
  callback_manager->subscribe(std::ref(prepare_callback_1));
  callback_manager->subscribe(std::ref(start_callback_1));
  callback_manager->subscribe(std::ref(pause_callback_1));
  callback_manager->subscribe(std::ref(resume_callback_1));
  callback_manager->subscribe(std::ref(cancel_callback_1));

  SECTION("When dispatching event, all callbacks are called") {
    manager.dispatch(std::make_shared<vda5050pp::events::ActionPrepare>());
    manager.dispatch(std::make_shared<vda5050pp::events::ActionStart>());
    manager.dispatch(std::make_shared<vda5050pp::events::ActionPause>());
    manager.dispatch(std::make_shared<vda5050pp::events::ActionResume>());
    manager.dispatch(std::make_shared<vda5050pp::events::ActionCancel>());
    std::this_thread::sleep_for(200ms);
    REQUIRE(prepare_callback_1.wasCalled());
    REQUIRE(start_callback_1.wasCalled());
    REQUIRE(pause_callback_1.wasCalled());
    REQUIRE(resume_callback_1.wasCalled());
    REQUIRE(cancel_callback_1.wasCalled());
  }

  SECTION("When the scoped manager was destroyed, no callbacks are called") {
    callback_manager.reset();
    manager.dispatch(std::make_shared<vda5050pp::events::ActionPrepare>());
    manager.dispatch(std::make_shared<vda5050pp::events::ActionStart>());
    manager.dispatch(std::make_shared<vda5050pp::events::ActionPause>());
    manager.dispatch(std::make_shared<vda5050pp::events::ActionResume>());
    manager.dispatch(std::make_shared<vda5050pp::events::ActionCancel>());
    std::this_thread::sleep_for(200ms);
    REQUIRE_FALSE(prepare_callback_1.wasCalled());
    REQUIRE_FALSE(start_callback_1.wasCalled());
    REQUIRE_FALSE(pause_callback_1.wasCalled());
    REQUIRE_FALSE(resume_callback_1.wasCalled());
    REQUIRE_FALSE(cancel_callback_1.wasCalled());
  }
}