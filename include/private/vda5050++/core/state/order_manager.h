//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_STATE_ORDER_MANAGER_H_
#define VDA5050_2B_2B_CORE_STATE_ORDER_MANAGER_H_

#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include "vda5050++/core/common/math/linear_path_length_calculator.h"
#include "vda5050++/core/state/graph.h"
#include "vda5050++/misc/order_status.h"
#include "vda5050/ActionState.h"
#include "vda5050/Error.h"
#include "vda5050/Info.h"
#include "vda5050/State.h"

namespace vda5050pp::core::state {

class OrderManager {
private:
  mutable std::mutex mutex_;
  std::string order_id_;
  uint32_t order_update_id_ = 0;
  std::string last_node_id_;
  decltype(vda5050::Node::sequenceId) last_node_sequence_id_ = 0;
  std::optional<Graph> graph_;
  std::map<std::string, std::shared_ptr<vda5050::Action>, std::less<>> action_by_id_;
  std::map<std::string, std::shared_ptr<vda5050::ActionState>, std::less<>> action_state_by_id_;
  vda5050pp::misc::OrderStatus order_status_ = vda5050pp::misc::OrderStatus::k_order_idle;

  inline bool invalidLock(const std::unique_lock<std::mutex> &lock) const {
    return lock.mutex() != &this->mutex_ || !lock.owns_lock();
  }

protected:
  void addNewAction(std::shared_ptr<vda5050::Action> action,
                    const std::unique_lock<std::mutex> &lock) noexcept(false);
  std::pair<GraphElement::SequenceId, GraphElement::SequenceId> extendGraph(
      Graph &&extension, const std::unique_lock<std::mutex> &lock) noexcept(false);
  std::pair<GraphElement::SequenceId, GraphElement::SequenceId> extendGraph(
      Graph &&extension, uint32_t order_update_id,
      const std::unique_lock<std::mutex> &lock) noexcept(false);
  void replaceGraph(Graph &&new_graph, std::string_view order_id,
                    const std::unique_lock<std::mutex> &lock) noexcept(false);

public:
  std::pair<std::string, uint32_t> getOrderId() const;
  vda5050pp::misc::OrderStatus getOrderStatus() const;
  void setOrderStatus(vda5050pp::misc::OrderStatus status);
  void addNewAction(std::shared_ptr<vda5050::Action> action) noexcept(false);
  std::shared_ptr<vda5050::Action> getAction(std::string_view action_id) noexcept(false);
  std::shared_ptr<vda5050::Action> tryGetAction(std::string_view action_id);
  std::shared_ptr<vda5050::ActionState> getActionState(std::string_view action_id) noexcept(false);
  std::shared_ptr<vda5050::ActionState> tryGetActionState(std::string_view action_id);
  std::pair<GraphElement::SequenceId, GraphElement::SequenceId> extendGraph(
      Graph &&extension) noexcept(false);
  std::pair<GraphElement::SequenceId, GraphElement::SequenceId> extendGraph(
      Graph &&extension, uint32_t order_update_id) noexcept(false);
  void replaceGraph(Graph &&new_graph, std::string_view order_id) noexcept(false);
  const Graph &getCurrentGraph() const noexcept(false);
  bool hasGraph() const;
  void setAGVLastNode(uint32_t seq_id) noexcept(false);
  void setAGVLastNodeId(std::string_view last_node_id) noexcept(false);
  void dumpTo(vda5050::State &state) const;
  void clearGraph();
  void cancelWaitingActions();
  void clearActions();
};

}  // namespace vda5050pp::core::state

#endif  // VDA5050_2B_2B_CORE_STATE_ORDER_MANAGER_H_
