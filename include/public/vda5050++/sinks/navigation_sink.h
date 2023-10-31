//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_SINKS_NAVIGATION_SINK_H_
#define PUBLIC_VDA5050_2B_2B_SINKS_NAVIGATION_SINK_H_

#include <vda5050/AGVPosition.h>
#include <vda5050/Node.h>
#include <vda5050/Velocity.h>

#include "vda5050++/events/navigation_event.h"

namespace vda5050pp::sinks {

///
///\brief The NavigationSink can be used to easily dump navigation related information into the
/// library.
///
class NavigationSink {
public:
  ///
  ///\brief set the current AGVPosition.
  ///
  ///\param agv_position the agv position.
  ///\throws VDA5050PPNotInitialized if the library was not initialized yet.
  ///
  void setPosition(const vda5050::AGVPosition &agv_position) const noexcept(false);

  ///
  ///\brief set the current velocity.
  ///
  ///\param velocity the agv velocity.
  ///\throws VDA5050PPNotInitialized if the library was not initialized yet.
  ///
  void setVelocity(const vda5050::Velocity &velocity) const noexcept(false);

  ///
  ///\brief set the current driving status.
  ///
  ///\param driving the driving status.
  ///\throws VDA5050PPNotInitialized if the library was not initialized yet.
  ///
  void setDriving(bool driving) const noexcept(false);

  ///
  ///\brief Tell the library, that the AGV reached a new node (controls the order flow)
  ///
  ///\param seq_id the sequence id of the node.
  ///\throws VDA5050PPNotInitialized if the library was not initialized yet.
  ///
  void setNodeReached(decltype(vda5050::Node::sequenceId) seq_id) const noexcept(false);

  ///
  ///\brief Tell the library, that the AGV reached a new node (controls the order flow)
  ///
  ///\param node the reached node.
  ///\throws VDA5050PPNotInitialized if the library was not initialized yet.
  ///
  void setNodeReached(std::shared_ptr<const vda5050::Node> node) const noexcept(false);

  ///
  ///\brief Set the new state.lastNodeId without updating the order flow.
  ///
  ///\param last_node_id the new lastNodeId
  ///\throws VDA5050PPNotInitialized if the library was not initialized yet.
  ///
  void setLastNodeId(std::string_view last_node_id) const noexcept(false);

  ///
  ///\brief Set the new distance since last node.
  ///
  ///\param distance_since_last_node the new distance since the last node.
  ///\throws VDA5050PPNotInitialized if the library was not initialized yet.
  ///
  void setDistanceSinceLastNode(double distance_since_last_node) const noexcept(false);

  ///
  ///\brief Set the current navigation status (controls order flow).
  ///
  ///\param status the new order status.
  ///\throws VDA5050PPNotInitialized if the library was not initialized yet.
  ///
  void setNavigationStatus(vda5050pp::events::NavigationStatusControlType status) const
      noexcept(false);
};

}  // namespace vda5050pp::sinks

#endif  // PUBLIC_VDA5050_2B_2B_SINKS_NAVIGATION_SINK_H_
