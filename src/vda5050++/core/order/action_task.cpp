//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/order/action_task.h"

#include "spdlog/fmt/fmt.h"
#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/logger.h"

using namespace vda5050pp::core::order;

ActionTransition ActionTransition::doStart() {
  return ActionTransition(Type::k_do_start, std::nullopt);
}
ActionTransition ActionTransition::isInitializing() {
  return ActionTransition(Type::k_is_initializing, std::nullopt);
}
ActionTransition ActionTransition::isRunning() {
  return ActionTransition(Type::k_is_running, std::nullopt);
}
ActionTransition ActionTransition::doPause() {
  return ActionTransition(Type::k_do_pause, std::nullopt);
}
ActionTransition ActionTransition::isPaused() {
  return ActionTransition(Type::k_is_paused, std::nullopt);
}
ActionTransition ActionTransition::doResume() {
  return ActionTransition(Type::k_do_resume, std::nullopt);
}
ActionTransition ActionTransition::isFailed() {
  return ActionTransition(Type::k_is_failed, std::nullopt);
}
ActionTransition ActionTransition::isFinished(std::optional<std::string_view> result) {
  return ActionTransition(Type::k_is_finished, result);
}
ActionTransition ActionTransition::doCancel() {
  return ActionTransition(Type::k_do_cancel, std::nullopt);
}

ActionTransition::Type ActionTransition::getType() const { return this->type_; }

std::optional<std::string_view> ActionTransition::getResult() const { return this->result_; }

ActionState::ActionState(ActionTask &task) : task_(task) {}
ActionTask &ActionState::task() { return this->task_; }

// ActionWaiting ///////////////////////////////////////////////////////////////////////////////////
ActionWaiting::ActionWaiting(ActionTask &task) : ActionStateT(task) {}
std::unique_ptr<ActionState> ActionWaiting::transition(const ActionTransition &transition) {
  switch (transition.getType()) {
    case ActionTransition::Type::k_do_start:
      return std::make_unique<ActionInitializing>(this->task());
    case ActionTransition::Type::k_do_cancel:
      return std::make_unique<ActionCanceling>(this->task());
    default:
      throw vda5050pp::VDA5050PPInvalidState(
          MK_EX_CONTEXT(fmt::format("Cannot {} during WAITING", transition)));
  }
}
void ActionWaiting::effect() {
  // No effect
}
std::string ActionWaiting::describe() { return "ActionWaiting"; }

// ActionInitializing //////////////////////////////////////////////////////////////////////////////
ActionInitializing::ActionInitializing(ActionTask &task) : ActionStateT(task) {}
std::unique_ptr<ActionState> ActionInitializing::transition(const ActionTransition &transition) {
  switch (transition.getType()) {
    case ActionTransition::Type::k_is_failed:
      return std::make_unique<ActionFailed>(this->task());
    case ActionTransition::Type::k_is_finished:
      return std::make_unique<ActionFinished>(this->task(), transition.getResult());
    case ActionTransition::Type::k_is_running:
      return std::make_unique<ActionRunning>(this->task());
    case ActionTransition::Type::k_do_cancel:
      return std::make_unique<ActionCanceling>(this->task());
    case ActionTransition::Type::k_is_initializing:
      return std::make_unique<ActionInitializingNoEffect>(this->task());
    case ActionTransition::Type::k_do_pause:
      return std::make_unique<ActionPausing>(this->task());
    case ActionTransition::Type::k_is_paused:
      return std::make_unique<ActionPaused>(this->task());
    default:
      throw vda5050pp::VDA5050PPInvalidState(
          MK_EX_CONTEXT(fmt::format("Cannot {} during INITIALIZING", transition)));
  }
}
void ActionInitializing::effect() {
  auto evt_a = std::make_shared<vda5050pp::events::ActionStart>();
  evt_a->action_id = this->task().getAction().actionId;

  auto evt_s = std::make_shared<vda5050pp::core::events::OrderActionStatusChanged>();
  evt_s->action_id = this->task().getAction().actionId;
  evt_s->action_status = vda5050::ActionStatus::INITIALIZING;

  Instance::ref().getOrderEventManager().dispatch(evt_s);
  Instance::ref().getActionEventManager().dispatch(evt_a);
}
std::string ActionInitializing::describe() { return "ActionInitializing"; }

// ActionInitializingNoEffect //////////////////////////////////////////////////////////////////////
ActionInitializingNoEffect::ActionInitializingNoEffect(ActionTask &task) : ActionStateT(task) {}
std::unique_ptr<ActionState> ActionInitializingNoEffect::transition(
    const ActionTransition &transition) {
  switch (transition.getType()) {
    case ActionTransition::Type::k_is_failed:
      return std::make_unique<ActionFailed>(this->task());
    case ActionTransition::Type::k_is_finished:
      return std::make_unique<ActionFinished>(this->task(), transition.getResult());
    case ActionTransition::Type::k_is_running:
      return std::make_unique<ActionRunning>(this->task());
    case ActionTransition::Type::k_do_cancel:
      return std::make_unique<ActionCanceling>(this->task());
    case ActionTransition::Type::k_is_initializing:
      return std::make_unique<ActionInitializingNoEffect>(this->task());
    case ActionTransition::Type::k_is_paused:
      return std::make_unique<ActionPaused>(this->task());
    case ActionTransition::Type::k_do_pause:
      return std::make_unique<ActionPausing>(this->task());
    default:
      throw vda5050pp::VDA5050PPInvalidState(
          MK_EX_CONTEXT(fmt::format("Cannot {} during INITIALIZING_NO_EFFECT", transition)));
  }
}
void ActionInitializingNoEffect::effect() {
  // No effect
}
std::string ActionInitializingNoEffect::describe() { return "ActionInitializingNoEffect"; }

// ActionRunning ///////////////////////////////////////////////////////////////////////////////////
ActionRunning::ActionRunning(ActionTask &task) : ActionStateT(task) {}
std::unique_ptr<ActionState> ActionRunning::transition(const ActionTransition &transition) {
  switch (transition.getType()) {
    case ActionTransition::Type::k_is_failed:
      return std::make_unique<ActionFailed>(this->task());
    case ActionTransition::Type::k_is_finished:
      return std::make_unique<ActionFinished>(this->task(), transition.getResult());
    case ActionTransition::Type::k_do_pause:
      return std::make_unique<ActionPausing>(this->task());
    case ActionTransition::Type::k_do_cancel:
      return std::make_unique<ActionCanceling>(this->task());
    default:
      throw vda5050pp::VDA5050PPInvalidState(
          MK_EX_CONTEXT(fmt::format("Cannot {} during RUNNING", transition)));
  }
}
void ActionRunning::effect() {
  auto evt = std::make_shared<vda5050pp::core::events::OrderActionStatusChanged>();
  evt->action_id = this->task().getAction().actionId;
  evt->action_status = vda5050::ActionStatus::RUNNING;

  Instance::ref().getOrderEventManager().dispatch(evt);
}
std::string ActionRunning::describe() { return "ActionRunning"; }

// ActionPausing ///////////////////////////////////////////////////////////////////////////////////
ActionPausing::ActionPausing(ActionTask &task) : ActionStateT(task) {}
std::unique_ptr<ActionState> ActionPausing::transition(const ActionTransition &transition) {
  switch (transition.getType()) {
    case ActionTransition::Type::k_is_failed:
      return std::make_unique<ActionFailed>(this->task());
    case ActionTransition::Type::k_is_paused:
      return std::make_unique<ActionPaused>(this->task());
    case ActionTransition::Type::k_do_cancel:
      return std::make_unique<ActionCanceling>(this->task());
    default:
      throw vda5050pp::VDA5050PPInvalidState(
          MK_EX_CONTEXT(fmt::format("Cannot {} during PAUSING", transition)));
  }
}
void ActionPausing::effect() {
  auto evt_a = std::make_shared<vda5050pp::events::ActionPause>();
  evt_a->action_id = this->task().getAction().actionId;

  Instance::ref().getActionEventManager().dispatch(evt_a);
}
std::string ActionPausing::describe() { return "ActionPausing"; }

// ActionResuming //////////////////////////////////////////////////////////////////////////////////
ActionResuming::ActionResuming(ActionTask &task) : ActionStateT(task) {}
std::unique_ptr<ActionState> ActionResuming::transition(const ActionTransition &transition) {
  switch (transition.getType()) {
    case ActionTransition::Type::k_is_failed:
      return std::make_unique<ActionFailed>(this->task());
    case ActionTransition::Type::k_is_running:
      return std::make_unique<ActionRunning>(this->task());
    case ActionTransition::Type::k_do_cancel:
      return std::make_unique<ActionCanceling>(this->task());
    case ActionTransition::Type::k_is_initializing:
      return std::make_unique<ActionInitializingNoEffect>(this->task());
    default:
      throw vda5050pp::VDA5050PPInvalidState(
          MK_EX_CONTEXT(fmt::format("Cannot {} during RESUMING", transition)));
  }
}
void ActionResuming::effect() {
  auto evt_a = std::make_shared<vda5050pp::events::ActionResume>();
  evt_a->action_id = this->task().getAction().actionId;

  Instance::ref().getActionEventManager().dispatch(evt_a);
}
std::string ActionResuming::describe() { return "ActionResuming"; }

// ActionPaused ////////////////////////////////////////////////////////////////////////////////////
ActionPaused::ActionPaused(ActionTask &task) : ActionStateT(task) {}
std::unique_ptr<ActionState> ActionPaused::transition(const ActionTransition &transition) {
  switch (transition.getType()) {
    case ActionTransition::Type::k_is_failed:
      return std::make_unique<ActionFailed>(this->task());
    case ActionTransition::Type::k_do_resume:
      return std::make_unique<ActionResuming>(this->task());
    case ActionTransition::Type::k_do_cancel:
      return std::make_unique<ActionCanceling>(this->task());
    case ActionTransition::Type::k_is_initializing:
      return std::make_unique<ActionInitializingNoEffect>(this->task());
    case ActionTransition::Type::k_is_running:
      return std::make_unique<ActionRunning>(this->task());
    default:
      throw vda5050pp::VDA5050PPInvalidState(
          MK_EX_CONTEXT(fmt::format("Cannot {} during PAUSED", transition)));
  }
}
void ActionPaused::effect() {
  auto evt = std::make_shared<vda5050pp::core::events::OrderActionStatusChanged>();
  evt->action_id = this->task().getAction().actionId;
  evt->action_status = vda5050::ActionStatus::PAUSED;

  Instance::ref().getOrderEventManager().dispatch(evt);
}
std::string ActionPaused::describe() { return "ActionPaused"; }

// ActionCanceling /////////////////////////////////////////////////////////////////////////////////
ActionCanceling::ActionCanceling(ActionTask &task) : ActionStateT(task) {}
std::unique_ptr<ActionState> ActionCanceling::transition(const ActionTransition &transition) {
  switch (transition.getType()) {
    case ActionTransition::Type::k_is_failed:
      return std::make_unique<ActionFailed>(this->task());
    case ActionTransition::Type::k_is_finished:
      return std::make_unique<ActionFinished>(this->task(), transition.getResult());
    default:
      throw vda5050pp::VDA5050PPInvalidState(
          MK_EX_CONTEXT(fmt::format("Cannot {} during CANCELING", transition)));
  }
}
void ActionCanceling::effect() {
  auto evt_a = std::make_shared<vda5050pp::events::ActionCancel>();
  evt_a->action_id = this->task().getAction().actionId;

  Instance::ref().getActionEventManager().dispatch(evt_a);
}
std::string ActionCanceling::describe() { return "ActionCanceling"; }

// ActionFailed ////////////////////////////////////////////////////////////////////////////////////
ActionFailed::ActionFailed(ActionTask &task) : ActionStateT(task) {}
std::unique_ptr<ActionState> ActionFailed::transition(const ActionTransition &transition) {
  throw vda5050pp::VDA5050PPInvalidState(
      MK_EX_CONTEXT(fmt::format("Cannot {} during FAILED", transition)));
}
void ActionFailed::effect() {
  auto s_evt = std::make_shared<vda5050pp::core::events::OrderActionStatusChanged>();
  s_evt->action_id = this->task().getAction().actionId;
  s_evt->action_status = vda5050::ActionStatus::FAILED;

  auto f_evt = std::make_shared<vda5050pp::events::ActionForget>();
  f_evt->action_id = this->task().getAction().actionId;

  Instance::ref().getOrderEventManager().dispatch(s_evt);
  Instance::ref().getActionEventManager().dispatch(f_evt);
}
std::string ActionFailed::describe() { return "ActionFailed"; }

// ActionFinished //////////////////////////////////////////////////////////////////////////////////
ActionFinished::ActionFinished(ActionTask &task, std::optional<std::string_view> result)
    : ActionStateT(task), result_(result) {}
std::unique_ptr<ActionState> ActionFinished::transition(const ActionTransition &transition) {
  throw vda5050pp::VDA5050PPInvalidState(
      MK_EX_CONTEXT(fmt::format("Cannot {} during FINISHED", transition)));
}
void ActionFinished::effect() {
  auto s_evt = std::make_shared<vda5050pp::core::events::OrderActionStatusChanged>();
  s_evt->action_id = this->task().getAction().actionId;
  s_evt->action_status = vda5050::ActionStatus::FINISHED;
  s_evt->result = this->result_;

  auto f_evt = std::make_shared<vda5050pp::events::ActionForget>();
  f_evt->action_id = this->task().getAction().actionId;

  Instance::ref().getOrderEventManager().dispatch(s_evt);
  Instance::ref().getActionEventManager().dispatch(f_evt);
}
std::string ActionFinished::describe() { return "ActionFinished"; }

// ActionTask //////////////////////////////////////////////////////////////////////////////////////
ActionTask::ActionTask(std::shared_ptr<const vda5050::Action> action)
    : state_(std::make_unique<ActionWaiting>(*this)), action_(action) {}

const vda5050::Action &ActionTask::getAction() const {
  if (this->action_ == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(MK_EX_CONTEXT("this->action_ is nullptr"));
  }
  return *this->action_;
}

void ActionTask::transition(const ActionTransition &transition) {
  getOrderLogger()->debug("ActionTask(id={})::transition pre_state_={}", this->action_->actionId,
                          this->state_->describe());
  this->state_ = this->state_->transition(transition);
  getOrderLogger()->debug("ActionTask(id={})::transition post_state_={}", this->action_->actionId,
                          this->state_->describe());
  this->state_->effect();
}

bool ActionTask::isTerminal() const { return this->state_->isTerminal(); }

bool ActionTask::isPaused() const { return this->state_->isPaused(); }

std::string ActionTask::describe() const {
  return fmt::format("ActionTask(id={}) state: {}", this->action_->actionId,
                     this->state_->describe());
}