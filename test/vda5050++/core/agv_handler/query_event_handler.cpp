// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
#include <catch2/catch.hpp>

#include "vda5050++/core/instance.h"

using namespace std::chrono_literals;

class TestQueryNoHandle : public vda5050pp::handler::BaseQueryHandler {};

class TestQueryPauseableHandle : public vda5050pp::handler::BaseQueryHandler {
private:
  bool pauseable_called_ = false;
  QueryPauseResumeResult result_;

public:
  TestQueryPauseableHandle() {
    vda5050::Error err;
    err.errorType = "test";
    result_.errors.emplace_back(err);
  }

  bool pauseableCalled() const { return pauseable_called_; }

  vda5050pp::handler::BaseQueryHandler::QueryPauseResumeResult getResult() const { return result_; }

  void reset() { pauseable_called_ = false; }

  QueryPauseResumeResult queryPauseable() override {
    this->pauseable_called_ = true;
    return result_;
  }
};

class TestQueryResumeHandle : public vda5050pp::handler::BaseQueryHandler {
private:
  bool resume_called_ = false;
  QueryPauseResumeResult result_;

public:
  TestQueryResumeHandle() {
    vda5050::Error err;
    err.errorType = "test";
    result_.errors.emplace_back(err);
  }

  bool resumeCalled() const { return resume_called_; }

  vda5050pp::handler::BaseQueryHandler::QueryPauseResumeResult getResult() const { return result_; }

  void reset() { resume_called_ = false; }

  QueryPauseResumeResult queryResumable() override {
    this->resume_called_ = true;
    return result_;
  }
};

class TestQueryAcceptHandle : public vda5050pp::handler::BaseQueryHandler {
private:
  bool accept_called_ = false;
  std::list<vda5050::Error> result_;

public:
  TestQueryAcceptHandle() {
    vda5050::Error err;
    err.errorType = "test";
    result_.emplace_back(err);
  }

  bool acceptCalled() const { return accept_called_; }

  std::list<vda5050::Error> getResult() const { return result_; }

  void reset() { accept_called_ = false; }

  std::list<vda5050::Error> queryAcceptZoneSet(std::string_view) override {
    this->accept_called_ = true;
    return result_;
  }
};

TEST_CASE("core::agv_handler:QueryEventHandler", "[core][agv_handler]") {
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().refEventManagerOptions().synchronous_event_dispatch = true;
  cfg.refGlobalConfig().useWhiteList();
  cfg.refGlobalConfig().bwListModule(vda5050pp::core::module_keys::k_query_event_handler_key);

  auto query_pauseable = std::make_shared<vda5050pp::events::QueryPauseable>();
  auto query_resumable = std::make_shared<vda5050pp::events::QueryResumable>();
  auto query_accept1 = std::make_shared<vda5050pp::events::QueryAcceptZoneSet>();
  query_accept1->zone_set_id = "zone1";
  auto query_accept3 = std::make_shared<vda5050pp::events::QueryAcceptZoneSet>();
  query_accept3->zone_set_id = "zone3";

  WHEN("The defaults are set a nop handler is set") {
    cfg.refQueryEventHandlerSubConfig().setDefaultAcceptZoneSets({"zone1", "zone2"});
    cfg.refQueryEventHandlerSubConfig().setDefaultPauseableSuccess(true);
    cfg.refQueryEventHandlerSubConfig().setDefaultResumableSuccess(false);
    vda5050pp::core::Instance::reset();
    auto instance = vda5050pp::core::Instance::init(cfg).lock();
    auto handler = std::make_shared<TestQueryNoHandle>();
    instance->setQueryHandler(handler);

    THEN("Accept Events are checked against the default") {
      instance->getQueryEventManager().dispatch(query_accept1);
      instance->getQueryEventManager().dispatch(query_accept3);
      auto future1 = query_accept1->getFuture();
      auto future3 = query_accept3->getFuture();
      REQUIRE(future1.wait_for(0s) == std::future_status::ready);
      REQUIRE(future1.get().empty());
      REQUIRE(future3.wait_for(0s) == std::future_status::ready);
      REQUIRE_FALSE(future3.get().empty());
    }

    THEN("Pause/Resume events are checked against the default") {
      instance->getQueryEventManager().dispatch(query_pauseable);
      instance->getQueryEventManager().dispatch(query_resumable);
      auto future1 = query_pauseable->getFuture();
      auto future2 = query_resumable->getFuture();
      REQUIRE(future1.wait_for(0s) == std::future_status::ready);
      REQUIRE(future2.wait_for(0s) == std::future_status::ready);
      REQUIRE(future1.get().errors.empty());
      REQUIRE_FALSE(future2.get().errors.empty());
    }
  }

  WHEN("A pauseable handler is set") {
    vda5050pp::core::Instance::reset();
    auto instance = vda5050pp::core::Instance::init(cfg).lock();
    auto handler = std::make_shared<TestQueryPauseableHandle>();
    instance->setQueryHandler(handler);

    THEN("Pause events are checked with the handler") {
      instance->getQueryEventManager().dispatch(query_pauseable);
      auto future = query_pauseable->getFuture();
      REQUIRE(future.wait_for(0s) == std::future_status::ready);
      REQUIRE(future.get().errors == handler->getResult().errors);
      REQUIRE(handler->pauseableCalled());
    }
  }

  WHEN("A resumable handler is set") {
    vda5050pp::core::Instance::reset();
    auto instance = vda5050pp::core::Instance::init(cfg).lock();
    auto handler = std::make_shared<TestQueryResumeHandle>();
    instance->setQueryHandler(handler);

    THEN("Resume events are checked with the handler") {
      instance->getQueryEventManager().dispatch(query_resumable);
      auto future = query_resumable->getFuture();
      REQUIRE(future.wait_for(0s) == std::future_status::ready);
      REQUIRE(future.get().errors == handler->getResult().errors);
      REQUIRE(handler->resumeCalled());
    }
  }

  WHEN("A accept handler is set") {
    vda5050pp::core::Instance::reset();
    auto instance = vda5050pp::core::Instance::init(cfg).lock();
    auto handler = std::make_shared<TestQueryAcceptHandle>();
    instance->setQueryHandler(handler);

    THEN("Accept events are checked with the handler") {
      instance->getQueryEventManager().dispatch(query_accept1);
      auto future = query_accept1->getFuture();
      REQUIRE(future.wait_for(0s) == std::future_status::ready);
      REQUIRE(future.get() == handler->getResult());
      REQUIRE(handler->acceptCalled());
    }
  }
}