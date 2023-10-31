//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/order/navigation_task.h"

#include <fmt/format.h>

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/instance.h"
#include "vda5050++/core/logger.h"

using namespace vda5050pp::core::order;

NavigationTransition::NavigationTransition(Type type, decltype(vda5050::Node::sequenceId) seq_id)
    : type_(type), seq_id_(seq_id) {}

NavigationTransition NavigationTransition::doStart() {
  return NavigationTransition(Type::k_do_start, 0);
}
NavigationTransition NavigationTransition::isResumed() {
  return NavigationTransition(Type::k_is_resumed, 0);
}
NavigationTransition NavigationTransition::doPause() {
  return NavigationTransition(Type::k_do_pause, 0);
}
NavigationTransition NavigationTransition::isPaused() {
  return NavigationTransition(Type::k_is_paused, 0);
}
NavigationTransition NavigationTransition::doResume() {
  return NavigationTransition(Type::k_do_resume, 0);
}
NavigationTransition NavigationTransition::toSeqId(decltype(vda5050::Node::sequenceId) seq_id) {
  return NavigationTransition(Type::k_to_seq_id, seq_id);
}
NavigationTransition NavigationTransition::doCancel() {
  return NavigationTransition(Type::k_do_cancel, 0);
}
NavigationTransition NavigationTransition::isFailed() {
  return NavigationTransition(Type::k_is_failed, 0);
}

NavigationTransition::Type NavigationTransition::type() const { return this->type_; }
decltype(vda5050::Node::sequenceId) NavigationTransition::seqId() const { return this->seq_id_; }

std::string NavigationTransition::describe() const {
  switch (this->type()) {
    case Type::k_do_start:
      return "k_do_start";
    case Type::k_is_resumed:
      return "k_is_resumed";
    case Type::k_do_pause:
      return "k_do_pause";
    case Type::k_is_paused:
      return "k_is_paused";
    case Type::k_do_resume:
      return "k_do_resume";
    case Type::k_to_seq_id:
      return fmt::format("k_to_seq_id={}", this->seq_id_);
    case Type::k_do_cancel:
      return "k_do_cancel";
    case Type::k_is_failed:
      return "k_is_failed";
    default:
      return "unknown";
  }
}

NavigationTask &NavigationState::task() { return this->task_; }

NavigationState::NavigationState(NavigationTask &task) : task_(task) {}

// NavigationWaiting ///////////////////////////////////////////////////////////////////////////////
NavigationWaiting::NavigationWaiting(NavigationTask &task) : NavigationStateT(task) {}
std::unique_ptr<NavigationState> NavigationWaiting::transfer(NavigationTransition transition) {
  switch (transition.type()) {
    case NavigationTransition::Type::k_do_cancel:
      return std::make_unique<NavigationCanceling>(this->task());
    case NavigationTransition::Type::k_do_start:
      return std::make_unique<NavigationFirstInProgress>(this->task());
    case NavigationTransition::Type::k_is_failed:
      return std::make_unique<NavigationFailed>(this->task());
    default:
      throw vda5050pp::VDA5050PPInvalidState(
          MK_EX_CONTEXT(fmt::format("Cannot {} during WAITING", transition)));
  }
}
void NavigationWaiting::effect() {
  // no effect
}
std::string NavigationWaiting::describe() { return "NavigationWaiting"; }

// NavigationFirstInProgress ///////////////////////////////////////////////////////////////////////
NavigationFirstInProgress::NavigationFirstInProgress(NavigationTask &task)
    : NavigationStateT(task) {}
std::unique_ptr<NavigationState> NavigationFirstInProgress::transfer(
    NavigationTransition transition) {
  switch (transition.type()) {
    case NavigationTransition::Type::k_do_cancel:
      return std::make_unique<NavigationCanceling>(this->task());
    case NavigationTransition::Type::k_do_pause:
      return std::make_unique<NavigationPausing>(this->task());
    case NavigationTransition::Type::k_is_failed:
      return std::make_unique<NavigationFailed>(this->task());
    case NavigationTransition::Type::k_is_paused:
      return std::make_unique<NavigationPaused>(this->task());
    case NavigationTransition::Type::k_to_seq_id:
      if (transition.seqId() == this->task().getGoal()->sequenceId) {
        return std::make_unique<NavigationDone>(this->task());
      } else {
        [[fallthrough]];
      }
    default:
      throw vda5050pp::VDA5050PPInvalidState(
          MK_EX_CONTEXT(fmt::format("Cannot {} during FIRST_IN_PROGRESS", transition)));
  }
}
void NavigationFirstInProgress::effect() {
  auto next_node_evt = std::make_shared<vda5050pp::events::NavigationNextNode>();
  next_node_evt->next_node = this->task().getGoal();
  next_node_evt->via_edge = this->task().getViaEdge();

  Instance::ref().getNavigationEventManager().dispatch(next_node_evt);

  if (auto maybe_seg = this->task().getSegment(); maybe_seg.has_value()) {
    auto upcoming_segment_evt = std::make_shared<vda5050pp::events::NavigationUpcomingSegment>();
    upcoming_segment_evt->begin_seq = maybe_seg->first;
    upcoming_segment_evt->end_seq = maybe_seg->second;
    Instance::ref().getNavigationEventManager().dispatch(upcoming_segment_evt);
  }
}
std::string NavigationFirstInProgress::describe() { return "NavigationFirstInProgress"; }

// NavigationInProgress ////////////////////////////////////////////////////////////////////////////
NavigationInProgress::NavigationInProgress(NavigationTask &task) : NavigationStateT(task) {}
std::unique_ptr<NavigationState> NavigationInProgress::transfer(NavigationTransition transition) {
  switch (transition.type()) {
    case NavigationTransition::Type::k_do_cancel:
      return std::make_unique<NavigationCanceling>(this->task());
    case NavigationTransition::Type::k_do_pause:
      return std::make_unique<NavigationPausing>(this->task());
    case NavigationTransition::Type::k_is_failed:
      return std::make_unique<NavigationFailed>(this->task());
    case NavigationTransition::Type::k_is_paused:
      return std::make_unique<NavigationPaused>(this->task());
    case NavigationTransition::Type::k_to_seq_id:
      if (transition.seqId() == this->task().getGoal()->sequenceId) {
        return std::make_unique<NavigationDone>(this->task());
      } else {
        [[fallthrough]];
      }
    default:
      throw vda5050pp::VDA5050PPInvalidState(
          MK_EX_CONTEXT(fmt::format("Cannot {} during IN_PROGRESS", transition)));
  }
}
void NavigationInProgress::effect() {
  // No effect
}
std::string NavigationInProgress::describe() { return "NavigationInProgress"; }

// NavigationPausing ///////////////////////////////////////////////////////////////////////////////
NavigationPausing::NavigationPausing(NavigationTask &task) : NavigationStateT(task) {}
std::unique_ptr<NavigationState> NavigationPausing::transfer(NavigationTransition transition) {
  switch (transition.type()) {
    case NavigationTransition::Type::k_do_cancel:
      return std::make_unique<NavigationCanceling>(this->task());
    case NavigationTransition::Type::k_is_paused:
      return std::make_unique<NavigationPaused>(this->task());
    case NavigationTransition::Type::k_is_failed:
      return std::make_unique<NavigationFailed>(this->task());
    case NavigationTransition::Type::k_to_seq_id:
      if (transition.seqId() == this->task().getGoal()->sequenceId) {
        return std::make_unique<NavigationDone>(this->task());
      } else {
        [[fallthrough]];
      }
    default:
      throw vda5050pp::VDA5050PPInvalidState(
          MK_EX_CONTEXT(fmt::format("Cannot {} during PAUSING", transition)));
  }
}
void NavigationPausing::effect() {
  auto a_evt = std::make_shared<vda5050pp::events::NavigationControl>();
  a_evt->type = vda5050pp::events::NavigationControlType::k_pause;

  Instance::ref().getNavigationEventManager().dispatch(a_evt);
}
std::string NavigationPausing::describe() { return "NavigationPausing"; }

// NavigationPaused ////////////////////////////////////////////////////////////////////////////////
NavigationPaused::NavigationPaused(NavigationTask &task) : NavigationStateT(task) {}
std::unique_ptr<NavigationState> NavigationPaused::transfer(NavigationTransition transition) {
  switch (transition.type()) {
    case NavigationTransition::Type::k_do_cancel:
      return std::make_unique<NavigationCanceling>(this->task());
    case NavigationTransition::Type::k_do_resume:
      return std::make_unique<NavigationResuming>(this->task());
    case NavigationTransition::Type::k_is_resumed:
      return std::make_unique<NavigationInProgress>(this->task());
    case NavigationTransition::Type::k_is_failed:
      return std::make_unique<NavigationFailed>(this->task());
    default:
      throw vda5050pp::VDA5050PPInvalidState(
          MK_EX_CONTEXT(fmt::format("Cannot {} during PAUSED", transition)));
  }
}
void NavigationPaused::effect() {
  // No effect
}
std::string NavigationPaused::describe() { return "NavigationPaused"; }

// NavigationResuming //////////////////////////////////////////////////////////////////////////////
NavigationResuming::NavigationResuming(NavigationTask &task) : NavigationStateT(task) {}
std::unique_ptr<NavigationState> NavigationResuming::transfer(NavigationTransition transition) {
  switch (transition.type()) {
    case NavigationTransition::Type::k_do_cancel:
      return std::make_unique<NavigationCanceling>(this->task());
    case NavigationTransition::Type::k_is_resumed:
      return std::make_unique<NavigationInProgress>(this->task());
    case NavigationTransition::Type::k_is_failed:
      return std::make_unique<NavigationFailed>(this->task());
    case NavigationTransition::Type::k_is_paused:
      return std::make_unique<NavigationPaused>(this->task());
    default:
      throw vda5050pp::VDA5050PPInvalidState(
          MK_EX_CONTEXT(fmt::format("Cannot {} during RESUMING", transition)));
  }
}
void NavigationResuming::effect() {
  auto a_evt = std::make_shared<vda5050pp::events::NavigationControl>();
  a_evt->type = vda5050pp::events::NavigationControlType::k_resume;

  Instance::ref().getNavigationEventManager().dispatch(a_evt);
}
std::string NavigationResuming::describe() { return "NavigationResuming"; }

// NavigationCanceling
// ////////////////////////////////////////////////////////////////////////////////
NavigationCanceling::NavigationCanceling(NavigationTask &task) : NavigationStateT(task) {}
std::unique_ptr<NavigationState> NavigationCanceling::transfer(NavigationTransition transition) {
  switch (transition.type()) {
    case NavigationTransition::Type::k_is_failed:
      return std::make_unique<NavigationFailed>(this->task());
    case NavigationTransition::Type::k_to_seq_id:
      if (transition.seqId() == this->task().getGoal()->sequenceId) {
        return std::make_unique<NavigationDone>(this->task());
      } else {
        [[fallthrough]];
      }
    default:
      throw vda5050pp::VDA5050PPInvalidState(
          MK_EX_CONTEXT(fmt::format("Cannot {} during IN_PROGRESS", transition)));
  }
}
void NavigationCanceling::effect() {
  auto a_evt = std::make_shared<vda5050pp::events::NavigationControl>();
  a_evt->type = vda5050pp::events::NavigationControlType::k_cancel;

  Instance::ref().getNavigationEventManager().dispatch(a_evt);
}
std::string NavigationCanceling::describe() { return "NavigationCanceling"; }

// NavigationFailed ////////////////////////////////////////////////////////////////////////////////
NavigationFailed::NavigationFailed(NavigationTask &task) : NavigationStateT(task) {}
std::unique_ptr<NavigationState> NavigationFailed::transfer(NavigationTransition transition) {
  throw vda5050pp::VDA5050PPInvalidState(
      MK_EX_CONTEXT(fmt::format("Cannot {} during FAILED", transition)));
}
void NavigationFailed::effect() {
  // No effect
}
std::string NavigationFailed::describe() { return "NavigationFailed"; }

// NavigationDone ////////////////////////////////////////////////////////////////////////////////
NavigationDone::NavigationDone(NavigationTask &task) : NavigationStateT(task) {}
std::unique_ptr<NavigationState> NavigationDone::transfer(NavigationTransition transition) {
  throw vda5050pp::VDA5050PPInvalidState(
      MK_EX_CONTEXT(fmt::format("Cannot {} during DONE", transition)));
}
void NavigationDone::effect() {
  auto evt = std::make_shared<vda5050pp::core::events::OrderNewLastNodeId>();
  evt->last_node_id = this->task().getGoal()->nodeId;
  evt->seq_id = this->task().getGoal()->sequenceId;

  Instance::ref().getOrderEventManager().synchronousDispatch(evt);
}
std::string NavigationDone::describe() { return "NavigationDone"; }

// NavigationTask //////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<const vda5050::Node> NavigationTask::getGoal() const { return this->goal_; }

std::shared_ptr<const vda5050::Edge> NavigationTask::getViaEdge() const { return this->via_edge_; }

std::optional<std::pair<decltype(vda5050::Node::sequenceId), decltype(vda5050::Node::sequenceId)>>
NavigationTask::getSegment() {
  return this->segment_;
}

NavigationTask::NavigationTask(std::shared_ptr<const vda5050::Node> goal,
                               std::shared_ptr<const vda5050::Edge> via_edge)
    : state_(std::make_unique<NavigationWaiting>(*this)), goal_(goal), via_edge_(via_edge) {}

void NavigationTask::setSegment(
    std::pair<decltype(vda5050::Node::sequenceId), decltype(vda5050::Node::sequenceId)> segment) {
  this->segment_ = segment;
}

void NavigationTask::transition(NavigationTransition transition) {
  if (this->state_ == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(MK_EX_CONTEXT("this->state_ is nullptr"));
  }

  getOrderLogger()->debug("NavigationTask(node_seq={}) pre_state_={}", this->goal_->sequenceId,
                          this->state_->describe());
  this->state_ = this->state_->transfer(transition);
  getOrderLogger()->debug("NavigationTask(node_seq={}) post_state_={}", this->goal_->sequenceId,
                          this->state_->describe());
  this->state_->effect();
}

bool NavigationTask::isTerminal() const {
  if (this->state_ == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(MK_EX_CONTEXT("this->state_ is nullptr"));
  }

  return this->state_->isTerminal();
}

bool NavigationTask::isPaused() const {
  if (this->state_ == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(MK_EX_CONTEXT("this->state_ is nullptr"));
  }

  return this->state_->isPaused();
}

std::string NavigationTask::describe() const {
  return fmt::format("NavigationTask(node_seq={}) state: {}", this->goal_->sequenceId,
                     this->state_->describe());
}