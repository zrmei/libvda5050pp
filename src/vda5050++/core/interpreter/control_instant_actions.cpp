// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/interpreter/control_instant_actions.h"

#include <fmt/format.h>

#include <set>

#include "vda5050++/core/checks/action.h"
#include "vda5050++/core/common/conversion.h"
#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/events/factsheet_event.h"
#include "vda5050++/core/instance.h"

using namespace std::chrono_literals;

static const vda5050pp::agv_description::ActionDeclaration k_cancelOrderDeclaration{
    "cancelOrder",
    "Request the AGV to cancel the Order.",
    std::nullopt,
    {},
    {},
    {vda5050::BlockingType::HARD},
    true,
    false,
    false,
};
static const vda5050pp::agv_description::ActionDeclaration k_startPauseDeclaration{
    "startPause", "Request the AGV to pause the Order.", std::nullopt, {},
    {},           {vda5050::BlockingType::HARD},         true,         false,
    false,
};
static const vda5050pp::agv_description::ActionDeclaration k_stopPauseDeclaration{
    "stopPause", "Request the AGV to resume the Order.", std::nullopt, {},
    {},          {vda5050::BlockingType::HARD},          true,         false,
    false,
};
static const vda5050pp::agv_description::ActionDeclaration k_requestFactsheetDeclaration{
    "requestFactsheet",
    "Request the factsheet from the AGV.",
    std::nullopt,
    {},
    {},
    {vda5050::BlockingType::NONE},
    true,
    false,
    false,
};
static const vda5050pp::agv_description::ActionDeclaration k_stateRequestDeclaration{
    "stateRequest",
    "Request the AGV to send a new state report.",
    std::nullopt,
    {},
    {},
    {vda5050::BlockingType::NONE},
    true,
    false,
    false,
};

static const std::set<vda5050pp::agv_description::ActionDeclaration> k_declarations{
    k_cancelOrderDeclaration,      k_startPauseDeclaration,   k_stopPauseDeclaration,
    k_requestFactsheetDeclaration, k_stateRequestDeclaration,
};

bool vda5050pp::core::interpreter::isControlInstantAction(const vda5050::Action &instant_action) {
  return std::any_of(k_declarations.begin(), k_declarations.end(),
                     [&instant_action](const auto &decl) {
                       return vda5050pp::core::checks::matchActionType(decl, instant_action);
                     });
}

std::list<vda5050::Error> vda5050pp::core::interpreter::validateControlInstantAction(
    const vda5050::Action &instant_action, vda5050pp::misc::ActionContext ctxt) {
  for (const auto &decl : k_declarations) {
    if (vda5050pp::core::checks::matchActionType(decl, instant_action)) {
      auto e = vda5050pp::core::checks::validateActionWithDeclaration(instant_action, ctxt, decl);
      e.splice(e.end(), vda5050pp::core::checks::controlActionFeasible(instant_action));
      return e;
    }
  }

  throw vda5050pp::VDA5050PPInvalidArgument(
      MK_FN_EX_CONTEXT(fmt::format("Trying to validate non-control action(id={}, type={})",
                                   instant_action.actionId, instant_action.actionType)));
}

std::shared_ptr<std::list<vda5050::AgvAction>>
vda5050pp::core::interpreter::listControlActions() noexcept(false) {
  auto actions = std::make_shared<std::list<vda5050::AgvAction>>();

  std::transform(k_declarations.begin(), k_declarations.end(), std::back_inserter(*actions),
                 &vda5050pp::core::common::fromActionDeclaration);

  return actions;
}

template <vda5050pp::misc::OrderStatus status>
static bool isOrderStatus(std::shared_ptr<vda5050pp::core::events::OrderStatus> evt) {
  return evt->status == status;
}

std::shared_ptr<vda5050pp::core::events::EventControlBlock>
vda5050pp::core::interpreter::makeCancelControlBlock(
    std::shared_ptr<const vda5050::Action> action) {
  auto done_run = [action]() {
    auto running_evt = std::make_shared<vda5050pp::core::events::OrderActionStatusChanged>();
    running_evt->action_id = action->actionId;
    running_evt->action_status = vda5050::ActionStatus::RUNNING;
    vda5050pp::core::Instance::ref().getOrderEventManager().synchronousDispatch(running_evt);
  };

  auto done_fin = [action]() {
    auto clear_evt = std::make_shared<vda5050pp::core::events::OrderClearAfterCancel>();
    clear_evt->cancel_action = action;
    vda5050pp::core::Instance::ref().getOrderEventManager().synchronousDispatch(clear_evt);

    auto finished_evt = std::make_shared<vda5050pp::core::events::OrderActionStatusChanged>();
    finished_evt->action_id = action->actionId;
    finished_evt->action_status = vda5050::ActionStatus::FINISHED;
    vda5050pp::core::Instance::ref().getOrderEventManager().synchronousDispatch(finished_evt);
  };

  using Status = vda5050pp::misc::OrderStatus;

  auto fn_cancel = std::make_shared<vda5050pp::core::events::FunctionBlock>();
  fn_cancel->setFunction([action] {
    auto cancel = std::make_shared<vda5050pp::core::events::InterpreterOrderControl>();
    cancel->status = vda5050pp::core::events::InterpreterOrderControl::Status::k_cancel;
    cancel->associated_action = action;
    Instance::ref().getInterpreterEventManager().dispatch(cancel);
  });

  auto latch_run = std::make_shared<
      vda5050pp::core::events::LambdaEventLatch<vda5050pp::core::events::OrderStatus>>(
      Instance::ref().getOrderEventManager().getScopedSubscriber(),
      isOrderStatus<Status::k_order_canceling>, std::move(done_run));

  auto latch_fin = std::make_shared<
      vda5050pp::core::events::LambdaEventLatch<vda5050pp::core::events::OrderStatus>>(
      Instance::ref().getOrderEventManager().getScopedSubscriber(),
      isOrderStatus<Status::k_order_idle>, std::move(done_fin));

  auto chain = std::make_shared<vda5050pp::core::events::EventControlChain>();
  chain->add(fn_cancel);
  chain->add(latch_run);
  chain->add(latch_fin);

  return chain;
}

std::shared_ptr<vda5050pp::core::events::EventControlBlock>
vda5050pp::core::interpreter::makePauseControlBlock(std::shared_ptr<const vda5050::Action> action) {
  auto done_run = [action]() {
    auto running_evt = std::make_shared<vda5050pp::core::events::OrderActionStatusChanged>();
    running_evt->action_id = action->actionId;
    running_evt->action_status = vda5050::ActionStatus::RUNNING;
    vda5050pp::core::Instance::ref().getOrderEventManager().synchronousDispatch(running_evt);
  };

  auto done_fin = [action]() {
    auto finished_evt = std::make_shared<vda5050pp::core::events::OrderActionStatusChanged>();
    finished_evt->action_id = action->actionId;
    finished_evt->action_status = vda5050::ActionStatus::FINISHED;
    vda5050pp::core::Instance::ref().getOrderEventManager().synchronousDispatch(finished_evt);
  };

  using Status = vda5050pp::misc::OrderStatus;

  auto fn_pause = std::make_shared<vda5050pp::core::events::FunctionBlock>();
  fn_pause->setFunction([action] {
    auto pause = std::make_shared<vda5050pp::core::events::InterpreterOrderControl>();
    pause->status = vda5050pp::core::events::InterpreterOrderControl::Status::k_pause;
    pause->associated_action = action;
    Instance::ref().getInterpreterEventManager().dispatch(pause);
  });

  auto latch_run = std::make_shared<
      vda5050pp::core::events::LambdaEventLatch<vda5050pp::core::events::OrderStatus>>(
      Instance::ref().getOrderEventManager().getScopedSubscriber(),
      isOrderStatus<Status::k_order_pausing>, std::move(done_run));

  auto latch_fin = std::make_shared<
      vda5050pp::core::events::LambdaEventLatch<vda5050pp::core::events::OrderStatus>>(
      Instance::ref().getOrderEventManager().getScopedSubscriber(),
      isOrderStatus<Status::k_order_paused>, done_fin);

  auto latch_idle_fin = std::make_shared<
      vda5050pp::core::events::LambdaEventLatch<vda5050pp::core::events::OrderStatus>>(
      Instance::ref().getOrderEventManager().getScopedSubscriber(),
      isOrderStatus<Status::k_order_idle_paused>, done_fin);

  auto chain_pause = std::make_shared<vda5050pp::core::events::EventControlChain>();
  chain_pause->add(latch_run);
  chain_pause->add(latch_fin);

  auto pause_alternative = std::make_shared<vda5050pp::core::events::EventControlAlternative>();
  pause_alternative->add(latch_idle_fin);  // Finish idle pause
  pause_alternative->add(chain_pause);     // Finish active pause

  auto chain = std::make_shared<vda5050pp::core::events::EventControlChain>();
  chain->add(fn_pause);
  chain->add(pause_alternative);

  return chain;
}

std::shared_ptr<vda5050pp::core::events::EventControlBlock>
vda5050pp::core::interpreter::makeResumeControlBlock(
    std::shared_ptr<const vda5050::Action> action) {
  auto done_run = [action]() {
    auto running_evt = std::make_shared<vda5050pp::core::events::OrderActionStatusChanged>();
    running_evt->action_id = action->actionId;
    running_evt->action_status = vda5050::ActionStatus::RUNNING;
    vda5050pp::core::Instance::ref().getOrderEventManager().synchronousDispatch(running_evt);
  };

  auto done_fin = [action]() {
    auto finished_evt = std::make_shared<vda5050pp::core::events::OrderActionStatusChanged>();
    finished_evt->action_id = action->actionId;
    finished_evt->action_status = vda5050::ActionStatus::FINISHED;
    vda5050pp::core::Instance::ref().getOrderEventManager().synchronousDispatch(finished_evt);
  };

  using Status = vda5050pp::misc::OrderStatus;

  auto fn_resume = std::make_shared<vda5050pp::core::events::FunctionBlock>();
  fn_resume->setFunction([action] {
    auto resume = std::make_shared<vda5050pp::core::events::InterpreterOrderControl>();
    resume->status = vda5050pp::core::events::InterpreterOrderControl::Status::k_resume;
    resume->associated_action = action;
    Instance::ref().getInterpreterEventManager().dispatch(resume);
  });

  auto latch_run = std::make_shared<
      vda5050pp::core::events::LambdaEventLatch<vda5050pp::core::events::OrderStatus>>(
      Instance::ref().getOrderEventManager().getScopedSubscriber(),
      isOrderStatus<Status::k_order_resuming>, std::move(done_run));

  auto latch_fin = std::make_shared<
      vda5050pp::core::events::LambdaEventLatch<vda5050pp::core::events::OrderStatus>>(
      Instance::ref().getOrderEventManager().getScopedSubscriber(),
      isOrderStatus<Status::k_order_active>, done_fin);

  auto latch_idle_fin = std::make_shared<
      vda5050pp::core::events::LambdaEventLatch<vda5050pp::core::events::OrderStatus>>(
      Instance::ref().getOrderEventManager().getScopedSubscriber(),
      isOrderStatus<Status::k_order_idle>, done_fin);

  auto resume_chain = std::make_shared<vda5050pp::core::events::EventControlChain>();
  resume_chain->add(latch_run);
  resume_chain->add(latch_fin);

  auto resume_alternative = std::make_shared<vda5050pp::core::events::EventControlAlternative>();
  resume_alternative->add(latch_idle_fin);
  resume_alternative->add(resume_chain);

  auto chain = std::make_shared<vda5050pp::core::events::EventControlChain>();
  chain->add(fn_resume);
  chain->add(resume_alternative);

  return chain;
}

std::shared_ptr<vda5050pp::core::events::EventControlBlock>
vda5050pp::core::interpreter::makeRequestFactsheetControlBlock(
    std::shared_ptr<const vda5050::Action> action) {
  auto send_fs = std::make_shared<vda5050pp::core::events::FunctionBlock>();
  send_fs->setFunction([] {
    auto g_evt = std::make_shared<vda5050pp::core::events::FactsheetGatherEvent>();
    auto result = g_evt->getFuture();
    Instance::ref().getFactsheetEventManager().synchronousDispatch(g_evt);
    if (result.wait_for(1us) == std::future_status::timeout) {
      throw vda5050pp::VDA5050PPSynchronizedEventTimedOut(
          MK_FN_EX_CONTEXT("Unhandled FactsheetGatherEvent"));
    }
    auto fs_evt = std::make_shared<vda5050pp::core::events::SendFactsheetMessageEvent>();
    fs_evt->factsheet = std::make_shared<vda5050::AgvFactsheet>(result.get());
    Instance::ref().getMessageEventManager().dispatch(fs_evt);
  });

  auto set_finished = std::make_shared<vda5050pp::core::events::FunctionBlock>();
  set_finished->setFunction([action]() {
    auto finished_evt = std::make_shared<vda5050pp::core::events::OrderActionStatusChanged>();
    finished_evt->action_id = action->actionId;
    finished_evt->action_status = vda5050::ActionStatus::FINISHED;
    vda5050pp::core::Instance::ref().getOrderEventManager().synchronousDispatch(finished_evt);
  });

  auto chain = std::make_shared<vda5050pp::core::events::EventControlChain>();
  chain->add(send_fs);
  chain->add(set_finished);

  return chain;
}

std::shared_ptr<vda5050pp::core::events::EventControlBlock>
vda5050pp::core::interpreter::makeStateRequestControlBlock(
    std::shared_ptr<const vda5050::Action> action) {
  // Just set the action to finished, since the state will be sent anyway
  auto set_finished = std::make_shared<vda5050pp::core::events::FunctionBlock>();
  set_finished->setFunction([action]() {
    auto finished_evt = std::make_shared<vda5050pp::core::events::OrderActionStatusChanged>();
    finished_evt->action_id = action->actionId;
    finished_evt->action_status = vda5050::ActionStatus::FINISHED;
    vda5050pp::core::Instance::ref().getOrderEventManager().synchronousDispatch(finished_evt);
  });

  return set_finished;
}

std::shared_ptr<vda5050pp::core::events::EventControlBlock>
vda5050pp::core::interpreter::makeControlInstantActionControlBlock(
    std::shared_ptr<const vda5050::Action> action) {
  if (action == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(MK_FN_EX_CONTEXT(""));
  }

  if (action->actionType == "cancelOrder") {
    return makeCancelControlBlock(action);
  } else if (action->actionType == "startPause") {
    return makePauseControlBlock(action);
  } else if (action->actionType == "stopPause") {
    return makeResumeControlBlock(action);
  } else if (action->actionType == "requestFactsheet") {
    return makeRequestFactsheetControlBlock(action);
  } else if (action->actionType == "stateRequest") {
    return makeStateRequestControlBlock(action);
  } else {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_FN_EX_CONTEXT(fmt::format("Unknown control action(type={})", action->actionType)));
  }
}