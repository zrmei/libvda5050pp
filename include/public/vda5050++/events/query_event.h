//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_EVENTS_QUERY_EVENT_H_
#define PUBLIC_VDA5050_2B_2B_EVENTS_QUERY_EVENT_H_

#include <list>
#include <variant>

#include "vda5050++/events/synchronized_event.h"
#include "vda5050/Error.h"

namespace vda5050pp::events {

///
///\brief The QueryEvent ID type.
///
enum class QueryEventType {
  k_pauseable,
  k_resumable,
  k_accept_zone_set,
};

///
///\brief The QueryEvent base type.
///
struct QueryEvent {};

///
///\brief The result type for QueryPauseable and QueryResumable events.
///
struct QueryPauseResumeResult {
  ///
  ///\brief A list of errors (empty iff. pause/resume is allowed)
  ///
  std::list<vda5050::Error> errors;
  ///
  ///\brief Notify via actions via ActionPause or ActionResume events.
  ///
  bool notify = true;
};

///
///\brief This event is dispatched by the library, once a startPause instant action was received.
/// It is meant to ask the user if the action is feasible.
/// The QueryPauseResumeResult shall be set via this->acquireResultToken().setValue().
///
struct QueryPauseable : public QueryEvent, SynchronizedEvent<QueryPauseResumeResult> {};

///
///\brief This event is dispatched by the library, once a stopPause instant action was received.
/// It is meant to ask the user if the action is feasible.
/// The QueryPauseResumeResult shall be set via this->acquireResultToken().setValue().
///
struct QueryResumable : public QueryEvent, SynchronizedEvent<QueryPauseResumeResult> {};

///
///\brief This event is dispatched by the library, for each received order, which contains a
/// zoneSetId.
/// Before accepting the order, the user has to set this->acquireResultToken().setValue() (or set
/// default / handler).
///
///
struct QueryAcceptZoneSet : public QueryEvent, SynchronizedEvent<std::list<vda5050::Error>> {
  std::string zone_set_id;
};

}  // namespace vda5050pp::events

#endif  // PUBLIC_VDA5050_2B_2B_EVENTS_QUERY_EVENT_H_
