//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_EVENTS_VALIDATION_EVENT_H_
#define VDA5050_2B_2B_CORE_EVENTS_VALIDATION_EVENT_H_

#include <vda5050/Error.h>
#include <vda5050/InstantActions.h>
#include <vda5050/Order.h>

#include <list>

#include "vda5050++/events/event_type.h"
#include "vda5050++/events/synchronized_event.h"

namespace vda5050pp::core::events {

enum class ValidationEventType {
  k_validate_order,
  k_validate_instant_actions,
};

struct ValidationEvent : public vda5050pp::events::Event<ValidationEventType> {};

using ValidationResult = std::list<vda5050::Error>;

struct ValidateOrderEvent
    : public vda5050pp::events::EventId<ValidationEvent, ValidationEventType::k_validate_order>,
      public vda5050pp::events::SynchronizedEvent<ValidationResult> {
  std::shared_ptr<const vda5050::Order> order;
};

struct ValidateInstantActionsEvent
    : public vda5050pp::events::EventId<ValidationEvent,
                                        ValidationEventType::k_validate_instant_actions>,
      public vda5050pp::events::SynchronizedEvent<ValidationResult> {
  std::shared_ptr<const vda5050::InstantActions> instant_actions;
};

}  // namespace vda5050pp::core::events

#endif  // VDA5050_2B_2B_CORE_EVENTS_VALIDATION_EVENT_H_
