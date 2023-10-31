//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_EVENTS_CONTROL_EVENT_H_
#define VDA5050_2B_2B_CORE_EVENTS_CONTROL_EVENT_H_

#include "vda5050++/events/event_type.h"

namespace vda5050pp::core::events {

enum class ControlEventType {
  k_control_messages,
};

struct ControlEvent : public vda5050pp::events::Event<ControlEventType> {};

struct ControlMessagesEvent
    : public vda5050pp::events::EventId<ControlEvent, ControlEventType::k_control_messages>,
      vda5050pp::events::SynchronizedEvent<void> {
  enum class Type {
    k_connect,
    k_disconnect,
  };
  Type type;
};

}  // namespace vda5050pp::core::events

#endif  // VDA5050_2B_2B_CORE_EVENTS_CONTROL_EVENT_H_
