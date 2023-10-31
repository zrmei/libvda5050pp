// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains the Factsheet events.
//
//

#ifndef VDA5050_2B_2B_CORE_EVENTS_FACTSHEET_EVENT_H_
#define VDA5050_2B_2B_CORE_EVENTS_FACTSHEET_EVENT_H_

#include <list>
#include <memory>

#include "vda5050++/events/event_type.h"
#include "vda5050++/events/synchronized_event.h"
#include "vda5050/AgvFactsheet.h"

namespace vda5050pp::core::events {

///
///\brief This is the Factsheet event identifier type.
///
enum class FactsheetEventType {
  k_factsheet_gather,
  k_factsheet_control_action_list,
};

///
///\brief The FactsheetEvent base class.
///
struct FactsheetEvent : public vda5050pp::events::Event<FactsheetEventType> {};

///
///\brief This event is used to request the factsheet module to gather the factsheet.
///
struct FactsheetGatherEvent
    : public vda5050pp::events::EventId<FactsheetEvent, FactsheetEventType::k_factsheet_gather>,
      public vda5050pp::events::SynchronizedEvent<vda5050::AgvFactsheet> {};

///
///\brief This event is dispatched by the factsheet module to gather all available control actions
///
struct FactsheetControlActionListEvent
    : public vda5050pp::events::EventId<FactsheetEvent,
                                        FactsheetEventType::k_factsheet_control_action_list>,
      public vda5050pp::events::SynchronizedEvent<std::shared_ptr<std::list<vda5050::AgvAction>>> {
};

}  // namespace vda5050pp::core::events

#endif  // VDA5050_2B_2B_CORE_EVENTS_FACTSHEET_EVENT_H_
