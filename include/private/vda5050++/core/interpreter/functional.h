//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_INTERPRETER_FUNCTIONAL_H_
#define VDA5050_2B_2B_CORE_INTERPRETER_FUNCTIONAL_H_

#include <vda5050/Order.h>

#include <queue>
#include <tuple>

#include "vda5050++/core/events/interpreter_event.h"
#include "vda5050++/core/state/graph.h"

namespace vda5050pp::core::interpreter {

using NodeIter = std::vector<vda5050::Node>::const_iterator;
using EdgeIter = std::vector<vda5050::Edge>::const_iterator;
using ActionIter = std::vector<vda5050::Action>::const_iterator;

struct EventIterState {
  NodeIter node_iter;
  EdgeIter edge_iter;
  bool do_action;
};

class EventIter {
public:
  enum class IterState {
    k_handling_initial,
    k_handling_pre_node_action,
    k_handling_node_action,
    k_handling_node_action_queue,
    k_handling_pre_edge_action,
    k_handling_edge_action,
    k_handling_edge_action_queue,
    k_handling_action_yield,
    k_handling_navigation,
    k_handling_transition,
    k_pre_done,
    k_done,
  };

protected:
  IterState &getIterState();
  NodeIter &getNodeIter();
  NodeIter &getNodeEnd();
  EdgeIter &getEdgeIter();
  EdgeIter &getEdgeEnd();
  ActionIter &getActionIter();
  ActionIter &getActionEnd();
  std::string &getOrderId();
  uint32_t &getOrderUpdateId();
  std::shared_ptr<vda5050pp::core::state::Graph> &getCollectedGraph();
  std::shared_ptr<vda5050::Node> &getCurrentGoalNode();
  std::shared_ptr<vda5050::Edge> &getCurrentViaEdge();
  std::vector<std::shared_ptr<const vda5050::Action>> &getCurrentActionGroup();
  bool &getStopAtGoal();
  vda5050::BlockingType &getCurrentActionGroupBlockingType();
  std::queue<std::shared_ptr<vda5050pp::core::events::InterpreterEvent>>
      &getActionEventsAfterNavigation();
  void ceilCurrentActionGroupBlockingType(vda5050::BlockingType additional_blocking_type);

  EventIter() = default;

private:
  NodeIter node_iter_;
  EdgeIter edge_iter_;
  NodeIter node_end_;
  EdgeIter edge_end_;
  ActionIter action_iter_;
  ActionIter action_end_;

  std::string order_id_;
  uint32_t order_update_id_ = 0;
  std::shared_ptr<vda5050pp::core::state::Graph> collected_graph_;
  std::shared_ptr<vda5050::Node> current_goal_node_;
  std::shared_ptr<vda5050::Edge> current_via_edge_;
  bool stop_at_goal_ = false;

  std::vector<std::shared_ptr<const vda5050::Action>> current_action_group_;
  vda5050::BlockingType current_action_group_blocking_type_;
  std::queue<std::shared_ptr<vda5050pp::core::events::InterpreterEvent>>
      action_events_after_navigation_;

  IterState iter_state_ = IterState::k_handling_initial;

public:
  static std::unique_ptr<EventIter> fromOrder(const vda5050::Order &order);
};

std::tuple<std::shared_ptr<vda5050pp::core::events::InterpreterEvent>, std::unique_ptr<EventIter>>
nextEvent(std::unique_ptr<EventIter> &&event_iter) noexcept(false);

}  // namespace vda5050pp::core::interpreter

#endif  // VDA5050_2B_2B_CORE_INTERPRETER_FUNCTIONAL_H_
