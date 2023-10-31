// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/core/agv_handler/navigation_event_handler.h"

#include <catch2/catch.hpp>

#include "vda5050++/core/instance.h"

class TestNavigationEventHandler : public vda5050pp::handler::BaseNavigationHandler {
private:
  bool horz_called_ = false;
  bool base_called_ = false;
  bool next_called_ = false;
  bool seg_called_ = false;
  bool cancel_called_ = false;
  bool pause_called_ = false;
  bool resume_called_ = false;

public:
  bool isHorizonCalled() const { return horz_called_; }
  bool isBaseCalled() const { return base_called_; }
  bool isNextCalled() const { return next_called_; }
  bool isSegmentCalled() const { return seg_called_; }
  bool isCancelCalled() const { return cancel_called_; }
  bool isPauseCalled() const { return pause_called_; }
  bool isResumeCalled() const { return resume_called_; }

  void reset() {
    horz_called_ = false;
    base_called_ = false;
    next_called_ = false;
    seg_called_ = false;
    cancel_called_ = false;
    pause_called_ = false;
    resume_called_ = false;
  }

  void horizonUpdated(const std::list<std::shared_ptr<const vda5050::Node>> &,
                      const std::list<std::shared_ptr<const vda5050::Edge>> &) override {
    horz_called_ = true;
  }

  void baseIncreased(const std::list<std::shared_ptr<const vda5050::Node>> &,
                     const std::list<std::shared_ptr<const vda5050::Edge>> &) override {
    base_called_ = true;
  }

  void navigateToNextNode(std::shared_ptr<const vda5050::Node>,
                          std::shared_ptr<const vda5050::Edge>) override {
    next_called_ = true;
  }

  void upcomingSegment(decltype(vda5050::Node::sequenceId),
                       decltype(vda5050::Node::sequenceId)) override {
    seg_called_ = true;
  }

  void cancel() override { cancel_called_ = true; }

  void pause() override { pause_called_ = true; }

  void resume() override { resume_called_ = true; }
};

TEST_CASE("core::agv_handler::NavigationEventHandler", "[core][agv_handler]") {
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().refEventManagerOptions().synchronous_event_dispatch = true;
  cfg.refGlobalConfig().useWhiteList();
  cfg.refGlobalConfig().bwListModule(vda5050pp::core::module_keys::k_navigation_event_handler_key);
  vda5050pp::core::Instance::reset();
  auto instance = vda5050pp::core::Instance::init(cfg).lock();

  auto base_evt = std::make_shared<vda5050pp::events::NavigationBaseIncreased>();
  auto pause_evt = std::make_shared<vda5050pp::events::NavigationControl>();
  auto resume_evt = std::make_shared<vda5050pp::events::NavigationControl>();
  auto cancel_evt = std::make_shared<vda5050pp::events::NavigationControl>();
  auto next_evt = std::make_shared<vda5050pp::events::NavigationNextNode>();
  auto horz_evt = std::make_shared<vda5050pp::events::NavigationHorizonUpdate>();
  auto seg_evt = std::make_shared<vda5050pp::events::NavigationUpcomingSegment>();

  pause_evt->type = vda5050pp::events::NavigationControlType::k_pause;
  resume_evt->type = vda5050pp::events::NavigationControlType::k_resume;
  cancel_evt->type = vda5050pp::events::NavigationControlType::k_cancel;

  auto handler = std::make_shared<TestNavigationEventHandler>();

  WHEN("There is no handler set") {
    THEN("Each event is unhandled") {
      REQUIRE_NOTHROW(instance->getNavigationEventManager().dispatch(base_evt));
      REQUIRE_NOTHROW(instance->getNavigationEventManager().dispatch(pause_evt));
      REQUIRE_NOTHROW(instance->getNavigationEventManager().dispatch(next_evt));
      REQUIRE_NOTHROW(instance->getNavigationEventManager().dispatch(horz_evt));
      REQUIRE_NOTHROW(instance->getNavigationEventManager().dispatch(seg_evt));
    }
  }

  WHEN("A handler is set") {
    instance->setNavigationHandler(handler);

    WHEN("Base Evt is dispatched") {
      handler->reset();
      instance->getNavigationEventManager().dispatch(base_evt);
      THEN("The handler is called") { REQUIRE(handler->isBaseCalled()); }
    }
    WHEN("Pause Evt is dispatched") {
      handler->reset();
      instance->getNavigationEventManager().dispatch(pause_evt);
      THEN("The handler is called") { REQUIRE(handler->isPauseCalled()); }
    }
    WHEN("Resume Evt is dispatched") {
      handler->reset();
      instance->getNavigationEventManager().dispatch(resume_evt);
      THEN("The handler is called") { REQUIRE(handler->isResumeCalled()); }
    }
    WHEN("Cancel Evt is dispatched") {
      handler->reset();
      instance->getNavigationEventManager().dispatch(cancel_evt);
      THEN("The handler is called") { REQUIRE(handler->isCancelCalled()); }
    }
    WHEN("Next Evt is dispatched") {
      handler->reset();
      instance->getNavigationEventManager().dispatch(next_evt);
      THEN("The handler is called") { REQUIRE(handler->isNextCalled()); }
    }
    WHEN("Horizon Evt is dispatched") {
      handler->reset();
      instance->getNavigationEventManager().dispatch(horz_evt);
      THEN("The handler is called") { REQUIRE(handler->isHorizonCalled()); }
    }
  }
}