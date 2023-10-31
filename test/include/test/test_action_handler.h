//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef TEST_INCLUDE_TEST_TEST_ACTION_HANDLER_H_
#define TEST_INCLUDE_TEST_TEST_ACTION_HANDLER_H_

#include "vda5050++/handler/simple_action_handler.h"

namespace test {

struct ActionHandlerData {
  vda5050::Action action;
};

struct ActionCallbackFutures {
  std::future<void> on_start_called;
  std::future<void> on_pause_called;
  std::future<void> on_resume_called;
  std::future<void> on_cancel_called;
};

class TestActionHandler : public vda5050pp::handler::SimpleActionHandler {
private:
  std::promise<ActionCallbackFutures> prepare_called_;

public:
  TestActionHandler(const vda5050pp::agv_description::ActionDeclaration &decl)
      : vda5050pp::handler::SimpleActionHandler(decl) {}
  virtual ~TestActionHandler() = default;

  vda5050pp::handler::ActionCallbacks prepare(
      std::shared_ptr<vda5050pp::handler::ActionState> action_state,
      std::shared_ptr<vda5050pp::handler::ParametersMap> parameters) noexcept(false) override;

  std::future<ActionCallbackFutures> getPrepareFuture() noexcept(true);
};

}  // namespace test

#endif  // TEST_INCLUDE_TEST_TEST_ACTION_HANDLER_H_
