//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/core/state/order_manager.h"

#include <fmt/format.h>

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/logger.h"

using namespace vda5050pp::core::state;

void OrderManager::addNewAction(std::shared_ptr<vda5050::Action> action,
                                const std::unique_lock<std::mutex> &lock) {
  if (this->invalidLock(lock)) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT("Lock not owned"));
  }

  if (action == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(MK_EX_CONTEXT("Action nullptr"));
  }

  getStateLogger()->debug("State: Inserting new action with id={}", action->actionId);

  if (auto [_, ok] = this->action_by_id_.try_emplace(action->actionId, action); !ok) {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("Action with id={} already exists.", action->actionId)));
  }

  auto action_state = std::make_shared<vda5050::ActionState>();
  action_state->actionId = action->actionId;
  action_state->actionType = action->actionType;
  action_state->actionDescription = action->actionDescription;
  action_state->actionStatus = vda5050::ActionStatus::WAITING;

  if (auto [_, ok] = this->action_state_by_id_.try_emplace(action->actionId, action_state); !ok) {
    getStateLogger()->error(
        "OrderManager invariant broken, ActionState exists without an existing action");
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("ActionState with id={} already exists.", action->actionId)));
  }
}

std::shared_ptr<vda5050::Action> OrderManager::getAction(std::string_view action_id) noexcept(
    false) {
  auto ret = this->action_by_id_.find(action_id);
  if (ret == this->action_by_id_.end()) {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("No Action with id={}", action_id)));
  }
  return ret->second;
}

std::shared_ptr<vda5050::Action> OrderManager::tryGetAction(std::string_view action_id) {
  auto ret = this->action_by_id_.find(action_id);
  if (ret == this->action_by_id_.end()) {
    return nullptr;
  }
  return ret->second;
}

std::shared_ptr<vda5050::ActionState> OrderManager::getActionState(
    std::string_view action_id) noexcept(false) {
  auto ret = this->action_state_by_id_.find(action_id);
  if (ret == this->action_state_by_id_.end()) {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("No ActionState with id={}", action_id)));
  }
  return ret->second;
}

std::shared_ptr<vda5050::ActionState> OrderManager::tryGetActionState(std::string_view action_id) {
  auto ret = this->action_state_by_id_.find(action_id);
  if (ret == this->action_state_by_id_.end()) {
    return nullptr;
  }
  return ret->second;
}

std::pair<std::string, uint32_t> OrderManager::getOrderId() const {
  std::unique_lock lock(this->mutex_);
  return {this->order_id_, this->order_update_id_};
}

vda5050pp::misc::OrderStatus OrderManager::getOrderStatus() const {
  std::unique_lock lock(this->mutex_);
  return this->order_status_;
}

void OrderManager::setOrderStatus(vda5050pp::misc::OrderStatus status) {
  std::unique_lock lock(this->mutex_);
  this->order_status_ = status;
}

void OrderManager::addNewAction(std::shared_ptr<vda5050::Action> action) {
  std::unique_lock lock(this->mutex_);
  this->addNewAction(action, lock);
}

std::pair<GraphElement::SequenceId, GraphElement::SequenceId> OrderManager::extendGraph(
    Graph &&extension, const std::unique_lock<std::mutex> &lock) noexcept(false) {
  if (this->invalidLock(lock)) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT("Lock not owned"));
  }

  // Log
  {
    std::string msg = "<none>";
    if (this->graph_.has_value()) {
      auto [f, l] = this->graph_->bounds();
      msg = fmt::format("[{}, {}]", f, l);
    }
    auto [ef, el] = extension.bounds();
    msg = fmt::format("State: extending current Graph {} with Graph [{}, {}]", msg, ef, el);
    getStateLogger()->debug(msg);
  }

  if (graph_.has_value()) {
    return this->graph_->extend(std::move(extension));
  } else {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT("State has no current graph"));
  }
}

std::pair<GraphElement::SequenceId, GraphElement::SequenceId> OrderManager::extendGraph(
    Graph &&extension) noexcept(false) {
  std::unique_lock lock(this->mutex_);
  return this->extendGraph(std::move(extension), lock);
}

std::pair<GraphElement::SequenceId, GraphElement::SequenceId> OrderManager::extendGraph(
    Graph &&extension, uint32_t order_update_id,
    const std::unique_lock<std::mutex> &lock) noexcept(false) {
  if (this->invalidLock(lock)) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT("Lock not owned"));
  }

  this->order_update_id_ = order_update_id;
  return this->extendGraph(std::move(extension), lock);
}

std::pair<GraphElement::SequenceId, GraphElement::SequenceId> OrderManager::extendGraph(
    Graph &&extension, uint32_t order_update_id) noexcept(false) {
  std::unique_lock lock(this->mutex_);
  return this->extendGraph(std::move(extension), order_update_id, lock);
}

void OrderManager::replaceGraph(Graph &&new_graph, std::string_view order_id,
                                const std::unique_lock<std::mutex> &lock) noexcept(false) {
  if (this->invalidLock(lock)) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT("Lock not owned"));
  }

  // log
  {
    auto [f, l] = new_graph.bounds();
    getStateLogger()->debug("Replacing Graph [{}, {}] (order_id {})", f, l, order_id);
  }

  this->graph_ = std::move(new_graph);
  this->order_id_ = order_id;
  this->order_update_id_ = 0;
  this->graph_->setAgvLastNodeSequenceId(0);  // Order is only accepted, if the AGV is on node 0
}

void OrderManager::replaceGraph(Graph &&new_graph, std::string_view order_id) noexcept(false) {
  std::unique_lock lock(this->mutex_);
  this->replaceGraph(std::move(new_graph), order_id, lock);
}

const Graph &OrderManager::getCurrentGraph() const noexcept(false) {
  // Ensure no concurrent changes, although later changes cannot be ruled out
  std::unique_lock lock(this->mutex_);

  if (!this->graph_.has_value()) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT("No known graph"));
  }

  return *this->graph_;
}

bool OrderManager::hasGraph() const { return this->graph_.has_value(); }

void OrderManager::setAGVLastNode(uint32_t agv_seq_id) noexcept(false) {
  std::unique_lock lock(this->mutex_);

  getStateLogger()->debug("setAGVLastNode({})", agv_seq_id);

  if (!this->graph_.has_value()) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT("No known graph"));
  }

  this->graph_->setAgvLastNodeSequenceId(agv_seq_id);
  this->graph_->trim();

  // Persistently store last_node information (available without graph)
  this->last_node_sequence_id_ = this->graph_->agvPosition();
  this->last_node_id_ = this->graph_->at(this->last_node_sequence_id_).getId();
}

void OrderManager::setAGVLastNodeId(std::string_view last_node_id) noexcept(false) {
  std::unique_lock lock(this->mutex_);
  this->last_node_id_ = last_node_id;
}

void OrderManager::dumpTo(vda5050::State &state) const {
  std::unique_lock lock(this->mutex_);

  // basic fields
  state.orderId = this->order_id_;
  state.orderUpdateId = this->order_update_id_;
  state.paused = this->order_status_ == vda5050pp::misc::OrderStatus::k_order_paused ||
                 this->order_status_ == vda5050pp::misc::OrderStatus::k_order_resuming;
  state.lastNodeId = this->last_node_id_;
  state.lastNodeSequenceId = this->last_node_sequence_id_;

  // action states
  for (const auto &[_, action_state] : this->action_state_by_id_) {
    state.actionStates.push_back(*action_state);
  }

  // graph
  if (this->graph_.has_value()) {
    this->graph_->dumpTo(state.nodeStates, state.edgeStates, true);
  }
}

void OrderManager::clearGraph() {
  std::unique_lock lock(this->mutex_);
  getStateLogger()->debug("clearing graph");
  this->graph_.reset();
}

void OrderManager::cancelWaitingActions() {
  std::unique_lock lock(this->mutex_);

  getStateLogger()->debug("cancelWaitingActions()");

  for (const auto &[id, state] : this->action_state_by_id_) {
    if (state == nullptr) {
      throw vda5050pp::VDA5050PPNullPointer(
          MK_EX_CONTEXT(fmt::format("ActionState(id={}) is nullptr", id)));
    }
    if (state->actionStatus == vda5050::ActionStatus::WAITING) {
      getStateLogger()->debug("Settings action (id={}) to failed", id);
      state->actionStatus = vda5050::ActionStatus::FAILED;
    }
  }
}

void OrderManager::clearActions() {
  std::unique_lock lock(this->mutex_);

  getStateLogger()->debug("clearActions()");

  this->action_state_by_id_.clear();
  this->action_by_id_.clear();
}