//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/interpreter/functional.h"

#include <fmt/format.h>

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/logger.h"

using namespace vda5050pp::core::interpreter;

// EventIter ///////////////////////////////////////////////////////////////////////////////////////
class EventIterImpl : public EventIter {
public:
  EventIterImpl(const NodeIter &n_beg, const NodeIter &n_end, const EdgeIter &e_beg,
                const EdgeIter &e_end) {
    this->getNodeIter() = n_beg;
    this->getNodeEnd() = n_end;
    this->getEdgeIter() = e_beg;
    this->getEdgeEnd() = e_end;
  }

  using EventIter::ceilCurrentActionGroupBlockingType;
  using EventIter::getActionEnd;
  using EventIter::getActionEventsAfterNavigation;
  using EventIter::getActionIter;
  using EventIter::getCollectedGraph;
  using EventIter::getCurrentActionGroup;
  using EventIter::getCurrentActionGroupBlockingType;
  using EventIter::getCurrentGoalNode;
  using EventIter::getCurrentViaEdge;
  using EventIter::getEdgeEnd;
  using EventIter::getEdgeIter;
  using EventIter::getIterState;
  using EventIter::getNodeEnd;
  using EventIter::getNodeIter;
  using EventIter::getOrderId;
  using EventIter::getOrderUpdateId;
  using EventIter::getStopAtGoal;
  using EventIter::IterState;
};

EventIter::IterState &EventIter::getIterState() { return this->iter_state_; }

NodeIter &EventIter::getNodeIter() { return this->node_iter_; }

NodeIter &EventIter::getNodeEnd() { return this->node_end_; }

EdgeIter &EventIter::getEdgeIter() { return this->edge_iter_; }

EdgeIter &EventIter::getEdgeEnd() { return this->edge_end_; }

ActionIter &EventIter::getActionIter() { return this->action_iter_; }

ActionIter &EventIter::getActionEnd() { return this->action_end_; }

std::string &EventIter::getOrderId() { return this->order_id_; }

uint32_t &EventIter::getOrderUpdateId() { return this->order_update_id_; }

std::shared_ptr<vda5050pp::core::state::Graph> &EventIter::getCollectedGraph() {
  return this->collected_graph_;
}

std::shared_ptr<vda5050::Node> &EventIter::getCurrentGoalNode() { return this->current_goal_node_; }

std::shared_ptr<vda5050::Edge> &EventIter::getCurrentViaEdge() { return this->current_via_edge_; }

std::vector<std::shared_ptr<const vda5050::Action>> &EventIter::getCurrentActionGroup() {
  return this->current_action_group_;
}

bool &EventIter::getStopAtGoal() { return this->stop_at_goal_; }

vda5050::BlockingType &EventIter::getCurrentActionGroupBlockingType() {
  return this->current_action_group_blocking_type_;
}

std::queue<std::shared_ptr<vda5050pp::core::events::InterpreterEvent>>
    &EventIter::getActionEventsAfterNavigation() {
  return this->action_events_after_navigation_;
}

void EventIter::ceilCurrentActionGroupBlockingType(vda5050::BlockingType additional_blocking_type) {
  switch (this->current_action_group_blocking_type_) {
    case vda5050::BlockingType::HARD:
      break;
    case vda5050::BlockingType::SOFT:
      if (additional_blocking_type == vda5050::BlockingType::HARD) {
        this->current_action_group_blocking_type_ = vda5050::BlockingType::HARD;
      }
      break;
    case vda5050::BlockingType::NONE:
      [[fallthrough]];
    default:
      this->current_action_group_blocking_type_ = additional_blocking_type;
      break;
  }
}

std::unique_ptr<EventIter> EventIter::fromOrder(const vda5050::Order &order) {
  auto ret = std::make_unique<EventIterImpl>(order.nodes.begin(), order.nodes.end(),
                                             order.edges.begin(), order.edges.end());
  ret->getOrderId() = order.orderId;
  ret->getOrderUpdateId() = order.orderUpdateId;

  return ret;
}

template <typename Derived, typename Base, typename Deleter>
static std::unique_ptr<Derived, Deleter> staticUniquePtrCast(std::unique_ptr<Base, Deleter> &&ptr) {
  auto raw = static_cast<Derived *>(ptr.release());
  return std::unique_ptr<Derived, Deleter>(raw, std::move(ptr.get_deleter()));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline std::tuple<std::shared_ptr<vda5050pp::core::events::InterpreterEvent>,
                  std::unique_ptr<EventIter>>
handlePreEdgeAction(std::unique_ptr<EventIter> &&event_iter) noexcept(false) {
  auto it = staticUniquePtrCast<EventIterImpl>(std::move(event_iter));

  const auto &e_it = it->getEdgeIter();

  if (e_it == it->getEdgeEnd()) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_FN_EX_CONTEXT("No edges remaining"));
  }

  if (e_it->released) {
    // Prepare iterators
    it->getActionIter() = e_it->actions.begin();
    it->getActionEnd() = e_it->actions.end();

    // Start handling edge actions
    it->getIterState() = EventIterImpl::IterState::k_handling_edge_action;
  } else {
    // Continue with navigation
    it->getIterState() = EventIterImpl::IterState::k_handling_navigation;
  }

  return nextEvent(std::move(it));
}

inline std::tuple<std::shared_ptr<vda5050pp::core::events::InterpreterEvent>,
                  std::unique_ptr<EventIter>>
handleEdgeAction(std::unique_ptr<EventIter> &&event_iter) noexcept(false) {
  auto it = staticUniquePtrCast<EventIterImpl>(std::move(event_iter));

  auto &a_it = it->getActionIter();

  if (a_it == it->getActionEnd()) {
    // If no edges remain, yield possibly missing
    it->getIterState() = EventIterImpl::IterState::k_handling_edge_action_queue;
    return nextEvent(std::move(it));
  }

  switch (a_it->blockingType) {
    case vda5050::BlockingType::HARD:
      if (!it->getCurrentActionGroup().empty()) {
        // Ignore action this cycle and yield the current group
        it->getIterState() = EventIterImpl::IterState::k_handling_edge_action_queue;
        return nextEvent(std::move(it));
      }
      // Yield action this cycle, next cycle yield the action group
      it->getIterState() = EventIterImpl::IterState::k_handling_edge_action_queue;
      [[fallthrough]];
    case vda5050::BlockingType::SOFT:
      it->getStopAtGoal() = true;  // Set stop hint for the preceding node
      [[fallthrough]];
    case vda5050::BlockingType::NONE: {
      // Add to current action group
      it->ceilCurrentActionGroupBlockingType(a_it->blockingType);
      auto current_action = std::make_shared<vda5050::Action>(*a_it);
      auto new_action_event = std::make_shared<vda5050pp::core::events::YieldNewAction>();
      it->getCurrentActionGroup().push_back(current_action);
      a_it++;
      new_action_event->action = current_action;
      return {new_action_event, std::move(it)};
    }
    default:
      throw vda5050pp::VDA5050PPInvalidArgument(
          MK_FN_EX_CONTEXT(fmt::format("Invalid BlockingType {}", (int)a_it->blockingType)));
  }
}

inline std::tuple<std::shared_ptr<vda5050pp::core::events::InterpreterEvent>,
                  std::unique_ptr<EventIter>>
handleEdgeActionQueue(std::unique_ptr<EventIter> &&event_iter) noexcept(false) {
  auto it = staticUniquePtrCast<EventIterImpl>(std::move(event_iter));

  if (!it->getCurrentActionGroup().empty()) {
    // Yield the action group
    auto yield_action_group_event =
        std::make_shared<vda5050pp::core::events::YieldActionGroupEvent>();
    yield_action_group_event->actions = std::move(it->getCurrentActionGroup());
    yield_action_group_event->blocking_type_ceiling = it->getCurrentActionGroupBlockingType();
    it->getActionEventsAfterNavigation().push(std::move(yield_action_group_event));
    // Reset
    it->getCurrentActionGroup().clear();
    it->getCurrentActionGroupBlockingType() = vda5050::BlockingType::NONE;

    return nextEvent(std::move(it));
  } else {
    // Skip to the next step
    if (it->getActionIter() != it->getActionEnd()) {
      // go back to edge action handling
      it->getIterState() = EventIterImpl::IterState::k_handling_edge_action;
    } else {
      // go to navigation handling
      it->getIterState() = EventIterImpl::IterState::k_handling_navigation;
    }
    return nextEvent(std::move(it));
  }
}

inline std::tuple<std::shared_ptr<vda5050pp::core::events::InterpreterEvent>,
                  std::unique_ptr<EventIter>>
handlePreNodeAction(std::unique_ptr<EventIter> &&event_iter) noexcept(false) {
  auto it = staticUniquePtrCast<EventIterImpl>(std::move(event_iter));

  const auto &n_it = it->getNodeIter();

  if (n_it == it->getNodeEnd()) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_FN_EX_CONTEXT("No nodes remaining"));
  }

  if (n_it->released) {
    // Prepare iterators
    it->getActionIter() = n_it->actions.begin();
    it->getActionEnd() = n_it->actions.end();

    // Start handling node actions
    it->getIterState() = EventIterImpl::IterState::k_handling_node_action;
  } else {
    // Transition
    it->getIterState() = EventIterImpl::IterState::k_handling_transition;
  }

  return nextEvent(std::move(it));
}

inline std::tuple<std::shared_ptr<vda5050pp::core::events::InterpreterEvent>,
                  std::unique_ptr<EventIter>>
handleNodeAction(std::unique_ptr<EventIter> &&event_iter) noexcept(false) {
  auto it = staticUniquePtrCast<EventIterImpl>(std::move(event_iter));

  auto &a_it = it->getActionIter();

  if (a_it == it->getActionEnd()) {
    // If no actions remain, yield possibly missing
    it->getIterState() = EventIterImpl::IterState::k_handling_node_action_queue;
    return nextEvent(std::move(it));
  }

  switch (a_it->blockingType) {
    case vda5050::BlockingType::HARD:
      if (!it->getCurrentActionGroup().empty()) {
        // Ignore action this cycle and yield the current group
        it->getIterState() = EventIterImpl::IterState::k_handling_node_action_queue;
        return nextEvent(std::move(it));
      }
      // Yield action this cycle, next cycle yield the action group
      it->getIterState() = EventIterImpl::IterState::k_handling_node_action_queue;
      [[fallthrough]];
    case vda5050::BlockingType::SOFT:
      it->getStopAtGoal() = true;  // Set stop hint for the current node
      [[fallthrough]];
    case vda5050::BlockingType::NONE: {
      // Add to current action group
      it->ceilCurrentActionGroupBlockingType(a_it->blockingType);
      auto current_action = std::make_shared<vda5050::Action>(*a_it);
      auto new_action_event = std::make_shared<vda5050pp::core::events::YieldNewAction>();
      it->getCurrentActionGroup().push_back(current_action);
      a_it++;
      new_action_event->action = current_action;
      return {new_action_event, std::move(it)};
    }
    default:
      throw vda5050pp::VDA5050PPInvalidArgument(
          MK_FN_EX_CONTEXT(fmt::format("Invalid BlockingType {}", (int)a_it->blockingType)));
  }
}

inline std::tuple<std::shared_ptr<vda5050pp::core::events::InterpreterEvent>,
                  std::unique_ptr<EventIter>>
handleNodeActionQueue(std::unique_ptr<EventIter> &&event_iter) noexcept(false) {
  auto it = staticUniquePtrCast<EventIterImpl>(std::move(event_iter));

  // After the potential yield...
  if (it->getActionIter() != it->getActionEnd()) {
    // go back to node action handling
    it->getIterState() = EventIterImpl::IterState::k_handling_node_action;
  } else {
    // go to transition handling
    it->getIterState() = EventIterImpl::IterState::k_handling_transition;
  }

  if (!it->getCurrentActionGroup().empty()) {
    // queue the action group
    auto yield_action_group_event =
        std::make_shared<vda5050pp::core::events::YieldActionGroupEvent>();
    yield_action_group_event->actions = std::move(it->getCurrentActionGroup());
    yield_action_group_event->blocking_type_ceiling = it->getCurrentActionGroupBlockingType();
    it->getActionEventsAfterNavigation().push(std::move(yield_action_group_event));
    // Reset
    it->getCurrentActionGroup().clear();
    it->getCurrentActionGroupBlockingType() = vda5050::BlockingType::NONE;
  }

  // Skip to next event
  return nextEvent(std::move(it));
}

inline std::tuple<std::shared_ptr<vda5050pp::core::events::InterpreterEvent>,
                  std::unique_ptr<EventIter>>
handleActionYield(std::unique_ptr<EventIter> &&event_iter) noexcept(false) {
  auto it = staticUniquePtrCast<EventIterImpl>(std::move(event_iter));

  auto &q = it->getActionEventsAfterNavigation();

  if (q.empty()) {
    // Continue handling the node's actions
    it->getIterState() = EventIterImpl::IterState::k_handling_pre_node_action;

    return nextEvent(std::move(it));
  } else {
    // Yield buffered events
    auto event = q.front();
    q.pop();
    return {event, std::move(it)};
  }
}

inline std::tuple<std::shared_ptr<vda5050pp::core::events::InterpreterEvent>,
                  std::unique_ptr<EventIter>>
handleInitial(std::unique_ptr<EventIter> &&event_iter) noexcept(false) {
  auto it = staticUniquePtrCast<EventIterImpl>(std::move(event_iter));

  const auto &n_it = it->getNodeIter();
  const auto &e_it = it->getEdgeIter();

  if (n_it == it->getNodeEnd()) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_FN_EX_CONTEXT("No Nodes remaining"));
  }
  if (e_it == it->getEdgeEnd()) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_FN_EX_CONTEXT("No Edges remaining"));
  }

  if (n_it->sequenceId < e_it->sequenceId && it->getOrderUpdateId() == 0) {
    // First of we need to handle the node's actions
    it->getIterState() = EventIterImpl::IterState::k_handling_pre_node_action;
  } else {
    // The navigation is handled before continuing
    it->getIterState() = EventIterImpl::IterState::k_handling_transition;
  }

  if (it->getOrderUpdateId() == 0) {
    // Clear actions
    return {std::make_shared<vda5050pp::core::events::YieldClearActions>(), std::move(it)};
  } else {
    // Do not clear actions
    return nextEvent(std::move(it));
  }
}

inline std::tuple<std::shared_ptr<vda5050pp::core::events::InterpreterEvent>,
                  std::unique_ptr<EventIter>>
handleTransition(std::unique_ptr<EventIter> &&event_iter) noexcept(false) {
  auto it = staticUniquePtrCast<EventIterImpl>(std::move(event_iter));

  auto &n_it = it->getNodeIter();
  auto &e_it = it->getEdgeIter();
  const auto &n_end = it->getNodeEnd();
  const auto &e_end = it->getEdgeEnd();

  // step
  if (n_it->sequenceId < e_it->sequenceId) {
    // We are handling the first node, so the e_it edge was not interpreted, yet
    // Construct the graph and only step the n_it
    it->getCollectedGraph() = std::make_shared<vda5050pp::core::state::Graph>(
        std::list{vda5050pp::core::state::GraphElement(std::make_shared<vda5050::Node>(*n_it))});
    n_it++;
  } else {
    // Set current goal, update graph and step
    if (!e_it->released) {
      // Skip horizon
      while (e_it != e_end && n_it != n_end) {
        it->getCollectedGraph()->update(
            vda5050pp::core::state::GraphElement(std::make_shared<vda5050::Edge>(*e_it++)));
        it->getCollectedGraph()->update(
            vda5050pp::core::state::GraphElement(std::make_shared<vda5050::Node>(*n_it++)));
      }
    } else if (!n_it->released) {
      // Invalid horizon
      throw vda5050pp::VDA5050PPInvalidArgument(
          MK_FN_EX_CONTEXT("Released Edge leads to unreleased Node"));
    } else {
      // Normal step
      it->getCurrentGoalNode() = std::make_shared<vda5050::Node>(*n_it);
      it->getCurrentViaEdge() = std::make_shared<vda5050::Edge>(*e_it);
      it->getCollectedGraph()->update(
          vda5050pp::core::state::GraphElement(it->getCurrentViaEdge()));
      it->getCollectedGraph()->update(
          vda5050pp::core::state::GraphElement(it->getCurrentGoalNode()));
      e_it++;
      n_it++;
    }
  }

  if (n_it == n_end && e_it == e_end) {
    // There is nothing more to interpret
    it->getIterState() = EventIterImpl::IterState::k_pre_done;
  } else if (n_it == n_end) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_FN_EX_CONTEXT("No more Nodes, but Edges"));
  } else if (e_it == e_end) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_FN_EX_CONTEXT("No more Edges, but Nodes"));
  } else {
    // Continue interpreting the next edge
    it->getIterState() = EventIterImpl::IterState::k_handling_pre_edge_action;
  }

  return nextEvent(std::move(it));
}

inline std::tuple<std::shared_ptr<vda5050pp::core::events::InterpreterEvent>,
                  std::unique_ptr<EventIter>>
handleNavigation(std::unique_ptr<EventIter> &&event_iter) noexcept(false) {
  auto it = staticUniquePtrCast<EventIterImpl>(std::move(event_iter));

  // Yield all buffered action events in the next step
  it->getIterState() = EventIterImpl::IterState::k_handling_action_yield;

  if (it->getCurrentGoalNode() == nullptr && it->getCurrentViaEdge() == nullptr) {
    // Reset
    it->getStopAtGoal() = false;
    // Skip to next event
    return nextEvent(std::move(it));
  } else if (it->getCurrentGoalNode() == nullptr) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_FN_EX_CONTEXT("No ViaEdge, but GoalNode"));
  } else if (it->getCurrentViaEdge() == nullptr) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_FN_EX_CONTEXT("No GoalNode, but ViaEdge"));
  } else {
    // Setup event
    auto yield_nav_step = std::make_shared<vda5050pp::core::events::YieldNavigationStepEvent>();
    yield_nav_step->goal_node = it->getCurrentGoalNode();
    yield_nav_step->via_edge = it->getCurrentViaEdge();
    yield_nav_step->has_stop_at_goal_hint = it->getStopAtGoal();

    // Reset
    it->getCurrentGoalNode() = nullptr;
    it->getCurrentViaEdge() = nullptr;
    it->getStopAtGoal() = false;

    return {yield_nav_step, std::move(it)};
  }
}

inline std::tuple<std::shared_ptr<vda5050pp::core::events::InterpreterEvent>,
                  std::unique_ptr<EventIter>>
handlePreDone(std::unique_ptr<EventIter> &&event_iter) noexcept(false) {
  auto it = staticUniquePtrCast<EventIterImpl>(std::move(event_iter));

  if (!it->getCurrentActionGroup().empty()) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_FN_EX_CONTEXT("ActionGroup must be empty"));
  }

  std::shared_ptr<vda5050pp::core::events::InterpreterEvent> new_event;

  if (it->getCurrentGoalNode() != nullptr && it->getCurrentViaEdge() != nullptr) {
    // Yield the remaining navigation step
    auto nav_yield_event = std::make_shared<vda5050pp::core::events::YieldNavigationStepEvent>();
    nav_yield_event->goal_node = it->getCurrentGoalNode();
    nav_yield_event->via_edge = it->getCurrentViaEdge();
    nav_yield_event->has_stop_at_goal_hint = it->getStopAtGoal();
    it->getCurrentGoalNode() = nullptr;
    it->getCurrentViaEdge() = nullptr;
    it->getStopAtGoal() = false;

    new_event = nav_yield_event;
  } else if (!it->getActionEventsAfterNavigation().empty()) {
    // Yield the remaining actions
    new_event = it->getActionEventsAfterNavigation().front();
    it->getActionEventsAfterNavigation().pop();
  } else if (it->getOrderUpdateId() > 0) {
    // Yield extension event
    auto graph_extension_event = std::make_shared<vda5050pp::core::events::YieldGraphExtension>();
    graph_extension_event->graph = it->getCollectedGraph();
    graph_extension_event->order_update_id = it->getOrderUpdateId();

    it->getIterState() = EventIterImpl::IterState::k_done;
    new_event = graph_extension_event;
  } else {
    // Yield replacement event
    auto graph_replacement_event =
        std::make_shared<vda5050pp::core::events::YieldGraphReplacement>();
    graph_replacement_event->graph = it->getCollectedGraph();
    graph_replacement_event->order_id = it->getOrderId();

    it->getIterState() = EventIterImpl::IterState::k_done;
    new_event = graph_replacement_event;
  }

  return {new_event, std::move(it)};
}

std::tuple<std::shared_ptr<vda5050pp::core::events::InterpreterEvent>, std::unique_ptr<EventIter>>
vda5050pp::core::interpreter::nextEvent(std::unique_ptr<EventIter> &&event_iter) noexcept(false) {
  if (event_iter == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(MK_FN_EX_CONTEXT("EventIter is nullptr"));
  }

  // Decide interpreter step based on the current EventIter state
  switch (auto it = staticUniquePtrCast<EventIterImpl>(std::move(event_iter)); it->getIterState()) {
    case EventIterImpl::IterState::k_handling_initial:
      getInterpreterLogger()->debug("nextEvent state: handlingInitial");
      return handleInitial(std::move(it));
    case EventIterImpl::IterState::k_handling_pre_node_action:
      getInterpreterLogger()->debug("nextEvent state: handlingPreNodeAction");
      return handlePreNodeAction(std::move(it));
    case EventIterImpl::IterState::k_handling_node_action:
      getInterpreterLogger()->debug("nextEvent state: handlingNodeAction");
      return handleNodeAction(std::move(it));
    case EventIterImpl::IterState::k_handling_node_action_queue:
      getInterpreterLogger()->debug("nextEvent state: handlingNodeActionQueue");
      return handleNodeActionQueue(std::move(it));
    case EventIterImpl::IterState::k_handling_pre_edge_action:
      getInterpreterLogger()->debug("nextEvent state: handlingPreEdgeAction");
      return handlePreEdgeAction(std::move(it));
    case EventIterImpl::IterState::k_handling_edge_action:
      getInterpreterLogger()->debug("nextEvent state: handlingEdgeAction");
      return handleEdgeAction(std::move(it));
    case EventIterImpl::IterState::k_handling_edge_action_queue:
      getInterpreterLogger()->debug("nextEvent state: handlingEdgeActionQueue");
      return handleEdgeActionQueue(std::move(it));
    case EventIterImpl::IterState::k_handling_action_yield:
      getInterpreterLogger()->debug("nextEvent state: handlingActionYield");
      return handleActionYield(std::move(it));
    case EventIterImpl::IterState::k_handling_transition:
      getInterpreterLogger()->debug("nextEvent state: handlingTransition");
      return handleTransition(std::move(it));
    case EventIterImpl::IterState::k_handling_navigation:
      getInterpreterLogger()->debug("nextEvent state: handlingNavigation");
      return handleNavigation(std::move(it));
    case EventIterImpl::IterState::k_pre_done:
      getInterpreterLogger()->debug("nextEvent state: handlingPreDone");
      return handlePreDone(std::move(it));
    case EventIterImpl::IterState::k_done:
      getInterpreterLogger()->debug("nextEvent state: handlingDone");
      return {nullptr, std::move(it)};
    default:
      throw vda5050pp::VDA5050PPInvalidArgument(
          MK_FN_EX_CONTEXT(fmt::format("Invalid IterState {}", (int)it->getIterState())));
  }
}