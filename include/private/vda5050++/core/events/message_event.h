//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_EVENTS_MESSAGE_EVENT_H_
#define VDA5050_2B_2B_CORE_EVENTS_MESSAGE_EVENT_H_

#include <memory>

#include "vda5050++/events/event_type.h"
#include "vda5050++/misc/connection_status.h"
#include "vda5050++/misc/message_error.h"
#include "vda5050/AgvFactsheet.h"
#include "vda5050/InstantActions.h"
#include "vda5050/Order.h"
#include "vda5050/State.h"
#include "vda5050/Visualization.h"

namespace vda5050pp::core::events {

enum class MessageEventType {
  k_receive_order_message,
  k_valid_order_message,
  k_receive_instant_actions_message,
  k_valid_instant_actions_message,
  k_send_factsheet_message,
  k_send_state_message,
  k_send_visualization_message,
  k_connection_changed,
  k_message_error,
};

struct MessageEvent : vda5050pp::events::Event<MessageEventType> {};

struct ReceiveOrderMessageEvent
    : vda5050pp::events::EventId<MessageEvent, MessageEventType::k_receive_order_message> {
  std::shared_ptr<vda5050::Order> order;
};

struct ValidOrderMessageEvent
    : vda5050pp::events::EventId<MessageEvent, MessageEventType::k_valid_order_message> {
  std::shared_ptr<vda5050::Order> valid_order;
};

struct ReceiveInstantActionMessageEvent
    : vda5050pp::events::EventId<MessageEvent,
                                 MessageEventType::k_receive_instant_actions_message> {
  std::shared_ptr<vda5050::InstantActions> instant_actions;
};

struct ValidInstantActionMessageEvent
    : vda5050pp::events::EventId<MessageEvent, MessageEventType::k_valid_instant_actions_message> {
  std::shared_ptr<vda5050::InstantActions> valid_instant_actions;
};

struct SendFactsheetMessageEvent
    : public vda5050pp::events::EventId<MessageEvent, MessageEventType::k_send_factsheet_message> {
  std::shared_ptr<vda5050::AgvFactsheet> factsheet;
};

struct SendStateMessageEvent
    : public vda5050pp::events::EventId<MessageEvent, MessageEventType::k_send_state_message> {
  std::shared_ptr<vda5050::State> state;
};

struct SendVisualizationMessageEvent
    : public vda5050pp::events::EventId<MessageEvent,
                                        MessageEventType::k_send_visualization_message> {
  std::shared_ptr<vda5050::Visualization> visualization;
};

struct ConnectionChangedEvent
    : public vda5050pp::events::EventId<MessageEvent, MessageEventType::k_connection_changed> {
  vda5050pp::misc::ConnectionStatus status;
};

struct MessageErrorEvent
    : public vda5050pp::events::EventId<MessageEvent, MessageEventType::k_message_error> {
  vda5050pp::misc::MessageErrorType error_type;
  std::string description;
};

}  // namespace vda5050pp::core::events

#endif  // VDA5050_2B_2B_CORE_EVENTS_MESSAGE_EVENT_H_
