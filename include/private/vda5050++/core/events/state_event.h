//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_EVENTS_STATE_EVENT_H_
#define VDA5050_2B_2B_CORE_EVENTS_STATE_EVENT_H_

#include "vda5050++/core/state/state_update_urgency.h"
#include "vda5050++/events/event_type.h"
#include "vda5050++/events/synchronized_event.h"

namespace vda5050pp::core::events {

enum class StateEventType {
  k_request_state_update,
};

struct StateEvent : public vda5050pp::events::Event<StateEventType> {};

struct RequestStateUpdateEvent
    : public vda5050pp::events::EventId<StateEvent, StateEventType::k_request_state_update>,
      vda5050pp::events::SynchronizedEvent<void> {
  vda5050pp::core::state::StateUpdateUrgency urgency;
};

}  // namespace vda5050pp::core::events

#endif  // VDA5050_2B_2B_CORE_EVENTS_STATE_EVENT_H_
