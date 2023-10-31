//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_EVENTS_ORDER_EVENT_H_
#define VDA5050_2B_2B_CORE_EVENTS_ORDER_EVENT_H_

#include "vda5050++/events/event_type.h"
#include "vda5050++/misc/order_status.h"
#include "vda5050/ActionStatus.h"

namespace vda5050pp::core::events {

enum class OrderEventType {
  k_action_status_changed,
  k_new_last_node_id,
  k_order_status,
  k_clear_after_cancel,
};

struct OrderEvent : public vda5050pp::events::Event<OrderEventType> {};

struct OrderActionStatusChanged
    : public vda5050pp::events::EventId<OrderEvent, OrderEventType::k_action_status_changed> {
  std::string action_id;
  vda5050::ActionStatus action_status;
  std::optional<std::string> result;
};

struct OrderNewLastNodeId
    : public vda5050pp::events::EventId<OrderEvent, OrderEventType::k_new_last_node_id> {
  std::string last_node_id;
  decltype(vda5050::Node::sequenceId) seq_id;
};

struct OrderStatus : public vda5050pp::events::EventId<OrderEvent, OrderEventType::k_order_status> {
  vda5050pp::misc::OrderStatus status;
};

struct OrderClearAfterCancel
    : public vda5050pp::events::EventId<OrderEvent, OrderEventType::k_clear_after_cancel> {
  std::shared_ptr<const vda5050::Action> cancel_action;
};

}  // namespace vda5050pp::core::events

#endif  // VDA5050_2B_2B_CORE_EVENTS_ORDER_EVENT_H_
