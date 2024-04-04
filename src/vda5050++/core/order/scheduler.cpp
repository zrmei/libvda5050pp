//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/order/scheduler.h"

#include <spdlog/fmt/fmt.h>

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/instance.h"
#include "vda5050++/core/logger.h"

using namespace vda5050pp::core::order;

SchedulerState::SchedulerState(Scheduler &scheduler) : scheduler_(scheduler) {}
Scheduler &SchedulerState::scheduler() { return this->scheduler_; }

// Scheduler Idle State ////////////////////////////////////////////////////////////////////////////
SchedulerIdle::SchedulerIdle(Scheduler &scheduler, bool notify) : SchedulerStateID(scheduler) {
  if (notify) {
    auto evt = std::make_shared<vda5050pp::core::events::OrderStatus>();
    evt->status = vda5050pp::misc::OrderStatus::k_order_idle;
    Instance::ref().getOrderEventManager().dispatch(evt);
  }
  this->scheduler().current_segment_ = std::nullopt;
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerIdle::cancel() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot cancel during idle"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerIdle::pause() {
  // Instantly go to idle paused, since no tasks run anyways
  return {std::make_unique<SchedulerIdlePaused>(this->scheduler(), true), false};
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerIdle::resume() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot resume during idle"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerIdle::interrupt() {
  this->scheduler().doInterrupt();
  return {std::make_unique<SchedulerInterrupting>(this->scheduler(), true), true};
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerIdle::update() {
  this->scheduler().updateFetchNext();

  if (this->scheduler().getNavigationTask() != nullptr ||
      !this->scheduler().getActiveActionTasksById().empty()) {
    return {std::make_unique<SchedulerActive>(this->scheduler(), true), false};
  }

  return {std::make_unique<SchedulerIdle>(this->scheduler()), false};
}
std::string SchedulerIdle::describe() { return "SchedulerIdle"; }
////////////////////////////////////////////////////////////////////////////////////////////////////

// Scheduler Idle Paused State /////////////////////////////////////////////////////////////////////
SchedulerIdlePaused::SchedulerIdlePaused(Scheduler &scheduler, bool notify)
    : SchedulerStateID(scheduler) {
  if (notify) {
    auto evt = std::make_shared<vda5050pp::core::events::OrderStatus>();
    evt->status = vda5050pp::misc::OrderStatus::k_order_idle_paused;
    Instance::ref().getOrderEventManager().dispatch(evt);
  }
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerIdlePaused::cancel() {
  // Just clear the queues, since no tasks run anyways
  this->scheduler().clearQueues();
  return {std::make_unique<SchedulerIdlePaused>(this->scheduler()), false};
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerIdlePaused::pause() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot pause during idle paused"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerIdlePaused::resume() {
  // Instantly resume, since no tasks run anyways
  return {std::make_unique<SchedulerIdle>(this->scheduler(), true), true};
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerIdlePaused::interrupt() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot interrupt during paused"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerIdlePaused::update() {
  // Don't do anything, since there are no tasks to be updated
  return {std::make_unique<SchedulerIdlePaused>(this->scheduler()), false};
}
std::string SchedulerIdlePaused::describe() { return "SchedulerIdlePaused"; }
////////////////////////////////////////////////////////////////////////////////////////////////////

// Scheduler Active State //////////////////////////////////////////////////////////////////////////
SchedulerActive::SchedulerActive(Scheduler &scheduler, bool notify) : SchedulerStateID(scheduler) {
  if (notify) {
    auto evt = std::make_shared<vda5050pp::core::events::OrderStatus>();
    evt->status = vda5050pp::misc::OrderStatus::k_order_active;
    Instance::ref().getOrderEventManager().dispatch(evt);
  }
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerActive::cancel() {
  for (const auto &[id, task] : this->scheduler().getActiveActionTasksById()) {
    task->transition(ActionTransition::doCancel());
  }

  if (auto nt = this->scheduler().getNavigationTask();
      nt != nullptr && !nt->isPaused() && !nt->isTerminal()) {
    nt->transition(vda5050pp::core::order::NavigationTransition::doCancel());
  }

  this->scheduler().clearQueues();

  return {std::make_unique<SchedulerCanceling>(this->scheduler(), true), false};
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerActive::pause() {
  for (const auto &[id, task] : this->scheduler().getRunningActionTasksById()) {
    task->transition(ActionTransition::doPause());
  }

  if (auto nt = this->scheduler().getNavigationTask(); nt != nullptr) {
    nt->transition(NavigationTransition::doPause());
  }

  return {std::make_unique<SchedulerPausing>(this->scheduler(), true), false};
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerActive::resume() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot resume during active"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerActive::interrupt() {
  this->scheduler().doInterrupt();
  return {std::make_unique<SchedulerInterrupting>(this->scheduler(), true), true};
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerActive::update() {
  this->scheduler().updateTasks();
  this->scheduler().updateTasksInterruptMapping();
  this->scheduler().updateFetchNext();

  if (this->scheduler().getActiveActionTasksById().empty() &&
      (this->scheduler().getNavigationTask() == nullptr ||
       this->scheduler().getNavigationTask()->isTerminal())) {
    return {std::make_unique<SchedulerIdle>(this->scheduler(), true), false};
  }

  return {std::make_unique<SchedulerActive>(this->scheduler(), false), false};
}
std::string SchedulerActive::describe() { return "SchedulerActive"; }
////////////////////////////////////////////////////////////////////////////////////////////////////

// Scheduler Canceling State
// ///////////////////////////////////////////////////////////////////////
SchedulerCanceling::SchedulerCanceling(Scheduler &scheduler, bool notify)
    : SchedulerStateID(scheduler) {
  if (notify) {
    auto evt = std::make_shared<vda5050pp::core::events::OrderStatus>();
    evt->status = vda5050pp::misc::OrderStatus::k_order_canceling;
    Instance::ref().getOrderEventManager().dispatch(evt);
  }
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerCanceling::cancel() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot cancel during canceling"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerCanceling::pause() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot pause during canceling"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerCanceling::resume() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot resume during canceling"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerCanceling::interrupt() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot interrupt during canceling"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerCanceling::update() {
  this->scheduler().updateTasks();
  this->scheduler().updateTasksInterruptMapping();

  if (this->scheduler().getActiveActionTasksById().empty() &&
      this->scheduler().getNavigationTask() == nullptr) {
    return {std::make_unique<SchedulerIdle>(this->scheduler(), true), false};
  }

  return {std::make_unique<SchedulerCanceling>(this->scheduler()), false};
}
std::string SchedulerCanceling::describe() { return "SchedulerCanceling"; }
////////////////////////////////////////////////////////////////////////////////////////////////////

// Scheduler Resuming State
// ////////////////////////////////////////////////////////////////////////
SchedulerResuming::SchedulerResuming(Scheduler &scheduler, bool notify)
    : SchedulerStateID(scheduler) {
  if (notify) {
    auto evt = std::make_shared<vda5050pp::core::events::OrderStatus>();
    evt->status = vda5050pp::misc::OrderStatus::k_order_resuming;
    Instance::ref().getOrderEventManager().dispatch(evt);
  }
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerResuming::cancel() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot cancel during resuming"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerResuming::pause() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot pause during resuming"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerResuming::resume() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot resume during resuming"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerResuming::interrupt() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot interrupt during resuming"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerResuming::update() {
  this->scheduler().updateTasks();
  this->scheduler().updateTasksInterruptMapping();

  if (!this->scheduler().getPausedActionTasksById().empty()) {
    return {std::make_unique<SchedulerResuming>(this->scheduler()), false};
  }

  if (auto nt = this->scheduler().getNavigationTask(); nt != nullptr && nt->isPaused()) {
    return {std::make_unique<SchedulerResuming>(this->scheduler()), false};
  }

  return {std::make_unique<SchedulerActive>(this->scheduler(), true), true};
}
std::string SchedulerResuming::describe() { return "SchedulerResuming"; }
////////////////////////////////////////////////////////////////////////////////////////////////////

// Scheduler Pausing State
// /////////////////////////////////////////////////////////////////////////
SchedulerPausing::SchedulerPausing(Scheduler &scheduler, bool notify)
    : SchedulerStateID(scheduler) {
  if (notify) {
    auto evt = std::make_shared<vda5050pp::core::events::OrderStatus>();
    evt->status = vda5050pp::misc::OrderStatus::k_order_pausing;
    Instance::ref().getOrderEventManager().dispatch(evt);
  }
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerPausing::cancel() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot cancel during pausing"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerPausing::pause() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot pause during pausing"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerPausing::resume() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot resume during pausing"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerPausing::interrupt() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot interrupt during pausing"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerPausing::update() {
  this->scheduler().updateTasks();
  this->scheduler().updateTasksInterruptMapping();

  if (!this->scheduler().getRunningActionTasksById().empty()) {
    getOrderLogger()->debug("there are running actions, still pausing");
    return {std::make_unique<SchedulerPausing>(this->scheduler()), false};
  }

  if (auto nt = this->scheduler().getNavigationTask(); nt != nullptr && !nt->isPaused()) {
    getOrderLogger()->debug("there is a running navigation task, still pausing");
    return {std::make_unique<SchedulerPausing>(this->scheduler()), false};
  }

  return {std::make_unique<SchedulerPaused>(this->scheduler(), true), false};
}
std::string SchedulerPausing::describe() { return "SchedulerPausing"; }
////////////////////////////////////////////////////////////////////////////////////////////////////

// Scheduler Paused State
// //////////////////////////////////////////////////////////////////////////
SchedulerPaused::SchedulerPaused(Scheduler &scheduler, bool notify) : SchedulerStateID(scheduler) {
  if (notify) {
    auto evt = std::make_shared<vda5050pp::core::events::OrderStatus>();
    evt->status = vda5050pp::misc::OrderStatus::k_order_paused;
    Instance::ref().getOrderEventManager().dispatch(evt);
  }
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerPaused::cancel() {
  return SchedulerActive(this->scheduler()).cancel();
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerPaused::pause() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot pause during paused"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerPaused::resume() {
  for (const auto &[id, task] : this->scheduler().getPausedActionTasksById()) {
    task->transition(ActionTransition::doResume());
  }

  if (auto nt = this->scheduler().getNavigationTask(); nt != nullptr) {
    nt->transition(NavigationTransition::doResume());
  }

  return {std::make_unique<SchedulerResuming>(this->scheduler(), true), true};
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerPaused::interrupt() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot interrupt during paused"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerPaused::update() {
  this->scheduler().updateTasks();
  this->scheduler().updateTasksInterruptMapping();
  return {std::make_unique<SchedulerPaused>(this->scheduler()), false};
}
std::string SchedulerPaused::describe() { return "SchedulerPaused"; }
////////////////////////////////////////////////////////////////////////////////////////////////////

// Scheduler Failed State
// //////////////////////////////////////////////////////////////////////////
SchedulerFailed::SchedulerFailed(Scheduler &scheduler, bool notify) : SchedulerStateID(scheduler) {
  if (notify) {
    auto evt = std::make_shared<vda5050pp::core::events::OrderStatus>();
    evt->status = vda5050pp::misc::OrderStatus::k_order_failed;
    Instance::ref().getOrderEventManager().dispatch(evt);
  }
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerFailed::cancel() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot cancel during failed"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerFailed::pause() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot pause during failed"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerFailed::resume() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot resume during failed"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerFailed::interrupt() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot interrupt during failed"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerFailed::update() {
  return {std::make_unique<SchedulerFailed>(this->scheduler()), false};
}
std::string SchedulerFailed::describe() { return "SchedulerFailed"; }
////////////////////////////////////////////////////////////////////////////////////////////////////

// Scheduler Interrupting
// //////////////////////////////////////////////////////////////////////////
SchedulerInterrupting::SchedulerInterrupting(Scheduler &scheduler, bool notify)
    : SchedulerStateID(scheduler) {
  if (notify) {
    auto evt = std::make_shared<vda5050pp::core::events::OrderStatus>();
    evt->status = vda5050pp::misc::OrderStatus::k_order_interrupting;

    Instance::ref().getOrderEventManager().dispatch(evt);
  }
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerInterrupting::cancel() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot cancel during interrupting"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerInterrupting::pause() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot pause during interrupting"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerInterrupting::resume() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot resume during interrupting"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerInterrupting::interrupt() {
  throw vda5050pp::VDA5050PPInvalidState(MK_EX_CONTEXT("Cannot interrupt during interrupting"));
}
std::pair<std::unique_ptr<SchedulerState>, bool> SchedulerInterrupting::update() {
  this->scheduler().updateTasks();
  this->scheduler().updateFetchNextInterrupt();

  if (this->scheduler().getRcvInterruptQueue().empty()) {
    // Done activating all interrupt tasks
    return {std::make_unique<SchedulerActive>(this->scheduler(), true), false};
  }

  return {std::make_unique<SchedulerInterrupting>(this->scheduler(), false), false};
}
std::string SchedulerInterrupting::describe() { return "SchedulerInterrupting"; }
////////////////////////////////////////////////////////////////////////////////////////////////////

// Scheduler Class
// /////////////////////////////////////////////////////////////////////////////////
std::map<std::string, std::shared_ptr<ActionTask>, std::less<>>
    &Scheduler::getActiveActionTasksById() {
  return this->active_action_tasks_by_id_;
}

std::map<std::string, std::shared_ptr<ActionTask>, std::less<>>
    &Scheduler::getRunningActionTasksById() {
  return this->running_action_tasks_by_id_;
}

std::map<std::string, std::shared_ptr<ActionTask>, std::less<>>
    &Scheduler::getPausedActionTasksById() {
  return this->paused_action_tasks_by_id_;
}

std::map<std::string, std::shared_ptr<ActionTask>, std::less<>>
    &Scheduler::getNavInterruptingActionTasksById() {
  return this->nav_interrupting_action_tasks_by_id_;
}

std::shared_ptr<NavigationTask> &Scheduler::getNavigationTask() { return this->navigation_task_; }

const std::deque<std::shared_ptr<vda5050pp::core::events::YieldInstantActionGroup>>
    &Scheduler::getRcvInterruptQueue() const {
  return this->rcv_interrupt_queue_;
}

void Scheduler::clearQueues() {
  std::vector<std::string> forget_ids;

  for (; !this->rcv_evt_queue_.empty(); this->rcv_evt_queue_.pop_front()) {
    auto elem = this->rcv_evt_queue_.front();
    if (elem->getId() == vda5050pp::core::events::InterpreterEventType::k_yield_action_group) {
      for (const auto &action :
           std::static_pointer_cast<vda5050pp::core::events::YieldActionGroupEvent>(elem)
               ->actions) {
        forget_ids.push_back(action->actionId);
      }
    }
  }

  for (; !this->rcv_interrupt_queue_.empty(); this->rcv_interrupt_queue_.pop_front()) {
    auto elem = this->rcv_interrupt_queue_.front();
    for (const auto &action : elem->instant_actions) {
      forget_ids.push_back(action->actionId);
    }
  }

  for (const auto &action_id : forget_ids) {
    auto evt = std::make_shared<vda5050pp::events::ActionForget>();
    evt->action_id = action_id;
    Instance::ref().getActionEventManager().dispatch(evt);
  }
}

void Scheduler::updateTasks() {
  // Drop all paused from running mapping and add them to paused mapping
  for (auto it = this->running_action_tasks_by_id_.begin();
       it != this->running_action_tasks_by_id_.end();) {
    if (it->second->isPaused() && !it->second->isTerminal()) {
      this->paused_action_tasks_by_id_.insert(*it);
    }

    if (it->second->isPaused() || it->second->isTerminal()) {
      it = this->running_action_tasks_by_id_.erase(it);
    } else {
      it++;
    }
  }

  // Drop all unpaused from paused mapping and add them to running mapping
  for (auto it = this->paused_action_tasks_by_id_.begin();
       it != this->paused_action_tasks_by_id_.end();) {
    if (!it->second->isPaused() && !it->second->isTerminal()) {
      this->running_action_tasks_by_id_.insert(*it);
    }
    if (!it->second->isPaused() || it->second->isTerminal()) {
      it = this->paused_action_tasks_by_id_.erase(it);
    } else {
      it++;
    }
  }

  // Drop all terminal from active mapping
  for (auto it = this->active_action_tasks_by_id_.begin();
       it != this->active_action_tasks_by_id_.end();) {
    if (it->second->isTerminal()) {
      it = this->active_action_tasks_by_id_.erase(it);
    } else {
      it++;
    }
  }

  // Update navigation
  if (this->navigation_task_ != nullptr && this->navigation_task_->isTerminal()) {
    getOrderLogger()->debug("Scheduler::updateTasks() - dropping terminal navigation_task");
    this->navigation_task_ = nullptr;
  }
}

void Scheduler::updateTasksInterruptMapping() {
  // Drop all terminal from nav_interrupting mapping
  bool dropped_from_nav = false;
  for (auto it = this->nav_interrupting_action_tasks_by_id_.begin();
       it != this->nav_interrupting_action_tasks_by_id_.end();) {
    if (it->second->isTerminal()) {
      it = this->nav_interrupting_action_tasks_by_id_.erase(it);
      dropped_from_nav = true;
    } else {
      it++;
    }
  }
  if (dropped_from_nav && this->nav_interrupting_action_tasks_by_id_.empty() &&
      this->navigation_task_ != nullptr) {
    this->navigation_task_->transition(vda5050pp::core::order::NavigationTransition::doResume());
  }
}

void Scheduler::updateFetchNext() {
  if (this->rcv_evt_queue_.empty()) {
    return;
  }

  const auto &elem = this->rcv_evt_queue_.front();

  if (elem->getId() == vda5050pp::core::events::InterpreterEventType::k_yield_action_group) {
    getOrderLogger()->debug("Scheduler::updateFetchNext(ActionGroup)");
    this->updateFetchNext(
        std::static_pointer_cast<vda5050pp::core::events::YieldActionGroupEvent>(elem));
  } else if (elem->getId() ==
             vda5050pp::core::events::InterpreterEventType::k_yield_navigation_step) {
    getOrderLogger()->debug("Scheduler::updateFetchNext(NavigationStep)");
    this->updateFetchNext(
        std::static_pointer_cast<vda5050pp::core::events::YieldNavigationStepEvent>(elem));
  }
}

void Scheduler::updateFetchNext(
    std::shared_ptr<vda5050pp::core::events::YieldActionGroupEvent> evt) {
  if (!this->active_action_tasks_by_id_.empty()) {
    return;
  }

  // Only allow actions that are non blocking if driving
  if (this->navigation_task_ != nullptr &&
      evt->blocking_type_ceiling != vda5050::BlockingType::NONE) {
    return;
  }

  // Close current segment, because it wont be increased
  this->current_segment_ = std::nullopt;

  // All guards passed -> activate actions
  this->current_action_blocking_type_ = evt->blocking_type_ceiling;
  for (const auto &action : evt->actions) {
    auto task = std::make_shared<ActionTask>(action);
    this->active_action_tasks_by_id_[action->actionId] = task;
    this->running_action_tasks_by_id_[action->actionId] = task;
    task->transition(ActionTransition::doStart());
  }

  this->rcv_evt_queue_.pop_front();
  this->updateFetchNext();
}

void Scheduler::updateFetchNext(
    std::shared_ptr<vda5050pp::core::events::YieldNavigationStepEvent> evt) {
  if (this->navigation_task_ != nullptr) {
    // If there is a running navigation task, it already has a segment.
    // This segment must be extended and a patch event must be dispatched
    this->doPatchSegment();
    return;
  }

  if (!this->nav_interrupting_action_tasks_by_id_.empty()) {
    return;
  }

  if (!this->active_action_tasks_by_id_.empty() &&
      this->current_action_blocking_type_ != vda5050::BlockingType::NONE) {
    return;
  }

  this->navigation_task_ = std::make_shared<NavigationTask>(evt->goal_node, evt->via_edge);

  // Check of the segment is up to date
  if (this->current_segment_.has_value() &&
      this->current_segment_->second >= evt->goal_node->sequenceId) {
    // Nothing to do, just proceed with fetching
    this->rcv_evt_queue_.pop_front();
    this->navigation_task_->transition(NavigationTransition::doStart());
    return this->updateFetchNext();
  }

  // Update current segment
  this->current_segment_ = std::make_pair(evt->goal_node->sequenceId, evt->goal_node->sequenceId);

  // prefetch max reachable sequence id from queue
  for (const auto &p_evt : this->rcv_evt_queue_) {
    if (p_evt->getId() == vda5050pp::core::events::InterpreterEventType::k_yield_navigation_step) {
      auto nav = std::static_pointer_cast<vda5050pp::core::events::YieldNavigationStepEvent>(p_evt);
      this->current_segment_->second = nav->goal_node->sequenceId;
      if (nav->has_stop_at_goal_hint) {
        break;
      }
    }
  }

  this->navigation_task_->setSegment(*this->current_segment_);
  this->navigation_task_->transition(NavigationTransition::doStart());

  this->rcv_evt_queue_.pop_front();
  this->updateFetchNext();
}

void Scheduler::doPatchSegment() {
  if (this->navigation_task_ == nullptr || this->current_segment_ == std::nullopt) {
    return;
  }

  auto old_segment_last = this->current_segment_->second;

  // prefetch max reachable sequence id from queue
  for (const auto &p_evt : this->rcv_evt_queue_) {
    if (p_evt->getId() == vda5050pp::core::events::InterpreterEventType::k_yield_navigation_step) {
      auto nav = std::static_pointer_cast<vda5050pp::core::events::YieldNavigationStepEvent>(p_evt);
      this->current_segment_->second = nav->goal_node->sequenceId;
      if (nav->has_stop_at_goal_hint) {
        break;
      }
    }
  }

  if (this->current_segment_->second == old_segment_last) {
    return;  // Nothing to do
  }

  auto evt = std::make_shared<vda5050pp::events::NavigationUpcomingSegment>();
  evt->begin_seq = old_segment_last + 1;
  evt->end_seq = this->current_segment_->second;
  Instance::ref().getNavigationEventManager().dispatch(evt);
}

void Scheduler::doInterrupt() {
  if (this->rcv_interrupt_queue_.empty()) {
    return;  // Nothing to do
  }

  const auto &evt = this->rcv_interrupt_queue_.front();

  // Only NONE Blocking allows driving
  if (evt->blocking_type_ceiling != vda5050::BlockingType::NONE &&
      this->navigation_task_ != nullptr) {
    this->navigation_task_->transition(NavigationTransition::doPause());
  }

  // HARD Blocking cannot run in parallel
  for (const auto &[id, task] : this->active_action_tasks_by_id_) {
    if (evt->blocking_type_ceiling == vda5050::BlockingType::HARD ||
        task->getAction().blockingType == vda5050::BlockingType::HARD) {
      task->transition(ActionTransition::doCancel());
    }
  }
}

void Scheduler::updateFetchNextInterrupt() {
  if (this->rcv_interrupt_queue_.empty()) {
    return;  // Nothing to do
  }

  const auto &evt = this->rcv_interrupt_queue_.front();

  // Only NONE Blocking allows driving
  if (evt->blocking_type_ceiling != vda5050::BlockingType::NONE &&
      this->navigation_task_ != nullptr && !this->navigation_task_->isPaused()) {
    return;
  }

  // HARD Blocking cannot run in parallel
  for (const auto &[id, task] : this->active_action_tasks_by_id_) {
    if (evt->blocking_type_ceiling == vda5050::BlockingType::HARD ||
        task->getAction().blockingType == vda5050::BlockingType::HARD) {
      return;
    }
  }

  // All guards passed -> activate instant actions
  this->current_action_blocking_type_ = evt->blocking_type_ceiling;
  for (const auto &action : evt->instant_actions) {
    auto task = std::make_shared<ActionTask>(action);
    this->active_action_tasks_by_id_[action->actionId] = task;
    this->running_action_tasks_by_id_[action->actionId] = task;
    if (action->blockingType != vda5050::BlockingType::NONE) {
      this->nav_interrupting_action_tasks_by_id_[action->actionId] = task;
    }
    task->transition(ActionTransition::doStart());
  }
  this->rcv_interrupt_queue_.pop_front();
}

Scheduler::Scheduler() { this->state_ = std::make_unique<SchedulerIdle>(*this); }

void Scheduler::cancel(std::optional<Lock> lock) {
  auto e_lock = this->ensureLock(std::move(lock));

  if (this->state_ == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(MK_EX_CONTEXT("state_ is nullptr"));
  }

  getOrderLogger()->debug("Scheduler::cancel() pre_state_={}", this->state_->describe());
  auto [state, update] = this->state_->cancel();
  this->state_ = std::move(state);
  getOrderLogger()->debug("Scheduler::cancel() post_state_={}", this->state_->describe());

  if (update) {
    this->update(std::move(e_lock));
  }
}

void Scheduler::pause(std::optional<Lock> lock) {
  auto e_lock = this->ensureLock(std::move(lock));

  if (this->state_ == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(MK_EX_CONTEXT("state_ is nullptr"));
  }

  getOrderLogger()->debug("Scheduler::pause() pre_state_={}", this->state_->describe());
  auto [state, update] = this->state_->pause();
  this->state_ = std::move(state);
  getOrderLogger()->debug("Scheduler::pause() post_state_={}", this->state_->describe());

  if (update) {
    this->update(std::move(e_lock));
  }
}

void Scheduler::resume(std::optional<Lock> lock) {
  auto e_lock = this->ensureLock(std::move(lock));

  if (this->state_ == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(MK_EX_CONTEXT("state_ is nullptr"));
  }

  getOrderLogger()->debug("Scheduler::resume() pre_state_={}", this->state_->describe());
  auto [state, update] = this->state_->resume();
  this->state_ = std::move(state);
  getOrderLogger()->debug("Scheduler::resume() post_state_={}", this->state_->describe());

  if (update) {
    this->update(std::move(e_lock));
  }
}

void Scheduler::update(std::optional<Lock> lock) {
  auto e_lock = this->ensureLock(std::move(lock));

  if (this->state_ == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(MK_EX_CONTEXT("state_ is nullptr"));
  }

  getOrderLogger()->debug("Scheduler::update() pre_state_={}", this->state_->describe());
  auto [state, update] = this->state_->update();
  this->state_ = std::move(state);
  getOrderLogger()->debug("Scheduler::update() post_state_={}", this->state_->describe());

  if (update) {
    this->update(std::move(e_lock));
  }
}

SchedulerStateType Scheduler::getState(std::optional<Lock> lock) {
  auto e_lock = this->ensureLock(std::move(lock));

  if (this->state_ == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(MK_EX_CONTEXT("state_ is nullptr"));
  }

  return this->state_->getState();
}

void Scheduler::actionTransition(std::string_view action_id, const ActionTransition &transition,
                                 std::optional<Lock> lock) {
  auto e_lock = this->ensureLock(std::move(lock));
  auto it = this->active_action_tasks_by_id_.find(action_id);

  getOrderLogger()->debug("Scheduler::actionTransition(id={}, type={})", action_id,
                          int(transition.getType()));

  if (it == this->active_action_tasks_by_id_.end()) {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("No known active action task with action_id {}", action_id)));
  }

  it->second->transition(transition);

  this->update(std::move(e_lock));
}

void Scheduler::navigationTransition(NavigationTransition transition, std::optional<Lock> lock) {
  auto e_lock = this->ensureLock(std::move(lock));

  getOrderLogger()->debug("Scheduler::navigationTransition(type={})", int(transition.type()));

  if (this->navigation_task_ == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(MK_EX_CONTEXT("navigation_task_ is nullptr"));
  }

  this->navigation_task_->transition(transition);

  this->update(std::move(e_lock));
}

void Scheduler::enqueueInterruptActions(
    std::shared_ptr<vda5050pp::core::events::YieldInstantActionGroup> evt,
    std::optional<Lock> lock) {
  auto e_lock = this->ensureLock(std::move(lock));

  getOrderLogger()->debug("Scheduler::enqueueInterruptActions()");

  if (this->state_ == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(MK_EX_CONTEXT("state_ is nullptr"));
  }

  this->rcv_interrupt_queue_.push_back(evt);

  getOrderLogger()->debug("Scheduler::enqueueInterruptActions() pre_state_={}",
                          this->state_->describe());
  auto [state, update] = this->state_->interrupt();
  this->state_ = std::move(state);
  getOrderLogger()->debug("Scheduler::enqueueInterruptActions() post_state_={}",
                          this->state_->describe());

  if (update) {
    this->update(std::move(e_lock));
  }
}

void Scheduler::enqueue(std::shared_ptr<vda5050pp::core::events::InterpreterEvent> evt,
                        std::optional<Lock> lock) {
  auto e_lock = this->ensureLock(std::move(lock));

  getOrderLogger()->debug("Scheduler::enqueue(type={})", int(evt->getId()));

  this->rcv_evt_queue_.push_back(evt);

  // Do not update yet, to allow accumulation of all yield events by the interpreter
}

std::string Scheduler::describe() const { return ""; }