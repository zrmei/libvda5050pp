//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_EVENTS_INTERPRETER_EVENT_H_
#define VDA5050_2B_2B_CORE_EVENTS_INTERPRETER_EVENT_H_

#include <vda5050/InstantActions.h>
#include <vda5050/Order.h>

#include <memory>
#include <string>
#include <vector>

#include "vda5050++/core/state/graph.h"
#include "vda5050++/events/event_type.h"

namespace vda5050pp::core::events {

enum class InterpreterEventType {
  k_yield_navigation_step,
  k_yield_action_group,
  k_yield_instant_action_group,
  k_yield_graph_extension,
  k_yield_graph_replacement,
  k_yield_new_action,
  k_yield_clear_actions,
  k_done,
  k_order_control,
};

struct InterpreterEvent : public vda5050pp::events::Event<InterpreterEventType> {};

struct YieldNavigationStepEvent
    : public vda5050pp::events::EventId<InterpreterEvent,
                                        InterpreterEventType::k_yield_navigation_step> {
  std::shared_ptr<const vda5050::Node> goal_node;
  std::shared_ptr<const vda5050::Edge> via_edge;
  bool has_stop_at_goal_hint = false;
};

struct YieldActionGroupEvent
    : public vda5050pp::events::EventId<InterpreterEvent,
                                        InterpreterEventType::k_yield_action_group> {
  std::vector<std::shared_ptr<const vda5050::Action>> actions;
  vda5050::BlockingType blocking_type_ceiling = vda5050::BlockingType::NONE;
};

struct YieldInstantActionGroup
    : public vda5050pp::events::EventId<InterpreterEvent,
                                        InterpreterEventType::k_yield_instant_action_group> {
  std::vector<std::shared_ptr<const vda5050::Action>> instant_actions;
  vda5050::BlockingType blocking_type_ceiling = vda5050::BlockingType::NONE;
};

struct YieldGraphExtension
    : public vda5050pp::events::EventId<InterpreterEvent,
                                        InterpreterEventType::k_yield_graph_extension> {
  uint32_t order_update_id;
  std::shared_ptr<vda5050pp::core::state::Graph> graph;
};

struct YieldGraphReplacement
    : public vda5050pp::events::EventId<InterpreterEvent,
                                        InterpreterEventType::k_yield_graph_replacement> {
  std::string order_id;
  std::shared_ptr<vda5050pp::core::state::Graph> graph;
};

struct YieldNewAction
    : public vda5050pp::events::EventId<InterpreterEvent,
                                        InterpreterEventType::k_yield_new_action> {
  std::shared_ptr<vda5050::Action> action;
};

struct YieldClearActions
    : public vda5050pp::events::EventId<InterpreterEvent,
                                        InterpreterEventType::k_yield_clear_actions> {};

struct InterpreterDone
    : public vda5050pp::events::EventId<InterpreterEvent, InterpreterEventType::k_done> {};

struct InterpreterOrderControl
    : public vda5050pp::events::EventId<InterpreterEvent, InterpreterEventType::k_order_control> {
  enum class Status {
    k_pause,
    k_resume,
    k_cancel,
  };
  std::shared_ptr<const vda5050::Action> associated_action;
  Status status;
};

}  // namespace vda5050pp::core::events

#endif  // VDA5050_2B_2B_CORE_EVENTS_INTERPRETER_EVENT_H_
