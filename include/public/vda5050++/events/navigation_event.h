//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_EVENTS_NAVIGATION_EVENT_H_
#define PUBLIC_VDA5050_2B_2B_EVENTS_NAVIGATION_EVENT_H_

#include <vda5050/Edge.h>
#include <vda5050/Node.h>
#include <vda5050/State.h>

#include <list>

#include "vda5050++/events/synchronized_event.h"

namespace vda5050pp::events {

///
///\brief The ID Type for NavigationEvents
///
enum class NavigationEventType {
  k_horizon_update,
  k_base_increased,
  k_next_node,
  k_upcoming_segment,
  k_control,
};

///
///\brief The type of NavigationControl Events (not an identifier, but an event member)
///
enum class NavigationControlType {
  k_pause,
  k_resume,
  k_cancel,
};

///
///\brief The type of NavigationStatusControl Events (not an identifier, but an event member)
///
enum class NavigationStatusControlType {
  k_paused,
  k_resumed,
  k_failed,
};

///
///\brief Base NavigationEvent type.
///
struct NavigationEvent {};

///
///\brief This event is dispatched by the library, once the horizon has changed. It contains the
/// __full__ new horizon.
///
struct NavigationHorizonUpdate : public NavigationEvent {
  std::list<std::shared_ptr<const vda5050::Node>> horz_replace_nodes;
  std::list<std::shared_ptr<const vda5050::Edge>> horz_replace_edges;
};

///
///\brief This event is dispatched by the library, once the base was extended. (New released
/// nodes/edges)
///
struct NavigationBaseIncreased : public NavigationEvent {
  std::list<std::shared_ptr<const vda5050::Node>> base_expand_nodes;
  std::list<std::shared_ptr<const vda5050::Edge>> base_expand_edges;
};

///
///\brief This event is dispatched by the library, as soon, as the AGV shall navigate to a certain
/// node.
///
struct NavigationNextNode : public NavigationEvent {
  std::shared_ptr<const vda5050::Node> next_node;
  std::shared_ptr<const vda5050::Edge> via_edge;
};

///
///\brief This event is dispatched by the library, as soon, as a new sequence of nodes and edges can
/// be traversed without
/// the need of stopping for a blocking action.
///
/// Each node and edge in [begin_seq, end_seq] can be traversed without stopping (except when
/// blocking instantActions arrive).
/// The arrival on each node must still be tracked with NavigationStatusNodeReached in order.
///
struct NavigationUpcomingSegment : public NavigationEvent {
  using SeqT = decltype(vda5050::Node::sequenceId);
  SeqT begin_seq;
  SeqT end_seq;
};

///
///\brief This event is dispatched by the library to control the navigation. (Pause/Resume/Stop).
///
struct NavigationControl : public NavigationEvent {
  NavigationControlType type;
};

///
///\brief The base type for NavigationStatus events.
///
struct NavigationStatus {};

///
///\brief The ID Type for NavigationStatus events.
///
enum class NavigationStatusType {
  k_position,
  k_velocity,
  k_node_reached,
  k_distance_since_last_node,
  k_driving,
  k_navigation_control_status,
};

///
///\brief This event can be dispatched by the user to set a new position.
/// If auto_check_node_reached is true, the library checks if the AGV arrived at the
/// current goal node. If so this is equivalent to dispatching a NavigationStatusNodeReached for the
/// current goal. The result of the check can be obtained by the SynchronizedEvent result
/// (this->getFuture()).
///
struct NavigationStatusPosition : public NavigationStatus, SynchronizedEvent<bool> {
  vda5050::AGVPosition position;
  bool auto_check_node_reached = false;
};

///
///\brief This event can be dispatched by the user to update the current velocity.
///
struct NavigationStatusVelocity : public NavigationStatus {
  vda5050::Velocity velocity;
};

///
///\brief This event can be dispatched by the user to set the current driving status of the AGV.
///
struct NavigationStatusDriving : public NavigationStatus {
  bool is_driving;
};

///
///\brief This event can be dispatched by the user to indicate the arrival at a certain node.
/// The node_seq_id is used for order tracking, while the lase_node_id only sets the
/// state.lastNodeId field.
///
struct NavigationStatusNodeReached : public NavigationStatus {
  std::optional<decltype(vda5050::Node::sequenceId)> node_seq_id;
  std::optional<std::string> last_node_id;
};

///
///\brief This event can be dispatched by the user to set the distance traveled since the lase node
///(in meters).
///
struct NavigationStatusDistanceSinceLastNode : public NavigationStatus {
  double distance_since_last_node;
};

///
///\brief This event can be dispatched by the user to tell the library, that the
/// navigation status changed. (paused,resumed,failed). If no BaseNavigationHandler is set,
/// this muse be used.
///
struct NavigationStatusControl : public NavigationStatus {
  NavigationStatusControlType type;
};

}  // namespace vda5050pp::events

#endif  // PUBLIC_VDA5050_2B_2B_EVENTS_NAVIGATION_EVENT_H_
