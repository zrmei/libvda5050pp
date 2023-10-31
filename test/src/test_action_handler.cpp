//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "test/test_action_handler.h"

using namespace test;

vda5050pp::handler::ActionCallbacks TestActionHandler::prepare(
    std::shared_ptr<vda5050pp::handler::ActionState>,
    std::shared_ptr<vda5050pp::handler::ParametersMap>) noexcept(false) {
  auto on_start = std::make_shared<std::packaged_task<void()>>([] {});
  auto on_pause = std::make_shared<std::packaged_task<void()>>([] {});
  auto on_resume = std::make_shared<std::packaged_task<void()>>([] {});
  auto on_cancel = std::make_shared<std::packaged_task<void()>>([] {});

  ActionCallbackFutures futures;
  futures.on_start_called = on_start->get_future();
  futures.on_pause_called = on_pause->get_future();
  futures.on_resume_called = on_resume->get_future();
  futures.on_cancel_called = on_cancel->get_future();

  this->prepare_called_.set_value(std::move(futures));

  return vda5050pp::handler::ActionCallbacks([on_start](auto &) { on_start->operator()(); },
                                             [on_pause](auto &) { on_pause->operator()(); },
                                             [on_resume](auto &) { on_resume->operator()(); },
                                             [on_cancel](auto &) { on_cancel->operator()(); });
}

std::future<ActionCallbackFutures> TestActionHandler::getPrepareFuture() noexcept(true) {
  return this->prepare_called_.get_future();
}
