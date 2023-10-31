//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_HANDLER_BASE_NAVIGATION_HANDLER_H_
#define PUBLIC_VDA5050_2B_2B_HANDLER_BASE_NAVIGATION_HANDLER_H_

#include <vda5050/AGVPosition.h>
#include <vda5050/Edge.h>
#include <vda5050/Node.h>

#include <list>
#include <memory>
#include <string_view>

namespace vda5050pp::handler {

///
///\brief The BaseNavigationHandler class can be overwritten to let the library automatically handle
/// NavigationEvents and dispatch NavigationStatusEvents.
///
class BaseNavigationHandler {
public:
  virtual ~BaseNavigationHandler() = default;

  ///
  ///\brief This function will be called by the library, once the horizon was update.
  ///
  ///\param horz_replace_nodes the new horizon nodes
  ///\param horz_replace_edges the new horizon edges
  ///
  virtual void horizonUpdated(
      const std::list<std::shared_ptr<const vda5050::Node>> &horz_replace_nodes,
      const std::list<std::shared_ptr<const vda5050::Edge>> &horz_replace_edges) = 0;

  ///
  ///\brief This function will be called by the library, once the base was increased.
  ///
  ///\param base_expand_nodes the new base nodes (does not contain the lase base node)
  ///\param base_expand_edges the new base edges
  ///
  virtual void baseIncreased(
      const std::list<std::shared_ptr<const vda5050::Node>> &base_expand_nodes,
      const std::list<std::shared_ptr<const vda5050::Edge>> &base_expand_edges) = 0;

  ///
  ///\brief This function will be called once the library wants the AGV to navigate to a specific
  /// node.
  /// (Alternative to upcomingSegment)
  ///
  ///\param next_node the goal node
  ///\param via_edge the via edge
  ///
  virtual void navigateToNextNode(std::shared_ptr<const vda5050::Node> next_node,
                                  std::shared_ptr<const vda5050::Edge> via_edge) = 0;

  ///
  ///\brief This function will be called once the library wants the AGV to traverse a set of nodes
  /// and edges.
  /// The segment from [begin_seq, end_seq] can be traversed without stopping (except by instant
  /// actions).
  ///
  ///\param begin_seq the first seq_id of the segment (lookup in base).
  ///\param end_seq the first end_seq of the segment (lookup in base).
  ///
  virtual void upcomingSegment(decltype(vda5050::Node::sequenceId) begin_seq,
                               decltype(vda5050::Node::sequenceId) end_seq) = 0;

  ///
  ///\brief This function will be called by the library, once the AGV has cancel navigation.
  ///
  virtual void cancel() = 0;

  ///
  ///\brief This function will be called by the library, once the AGV has to pause navigation.
  ///
  virtual void pause() = 0;

  ///
  ///\brief This function will be called by the library, once the AGV has to resume navigation.
  ///
  virtual void resume() = 0;

  ///
  ///\brief This has to be called by the user once the navigation is paused (can be called
  /// independently of pause())
  ///
  void setPaused() const;

  ///
  ///\brief This has to be called by the user once the navigation is resumed (can be called
  /// independently of resume())
  ///
  void setResumed() const;

  ///
  ///\brief This has to be called by the user once the navigation is failed (can be called
  /// independently of cancel())
  ///
  void setFailed() const;

  ///
  /// \brief Tell the library, that a specific node was reached (alternative to evalPosition).
  /// Same as setNodeReached(uint32_t), this just extracts the id.
  ///
  /// NOTE: cannot be undone (set to predecessor node)
  ///
  /// \param node the node reached
  ///
  void setNodeReached(const vda5050::Node &node) const noexcept(false);

  ///
  /// \brief Tell the library, that a specific node was reached (alternative to evalPosition)
  /// Same as setNodeReached(const vda5050pp::Node &), this does not extract the id.
  ///
  /// NOTE: cannot be undone (set to predecessor node)
  ///
  /// \param node_seq the sequence id of the node
  ///
  void setNodeReached(uint32_t node_seq) const noexcept(false);

  ///
  /// \brief Set the current position (full vda5050 data) and check if the next node was reached to
  /// indicate task progress (alternative to setNodeReached).
  ///
  /// \param position the position of the AGV
  /// \return true, if the next node was reached
  ///
  bool evalPosition(const vda5050::AGVPosition &position) const noexcept(false);

  ///
  /// \brief Set the current position of the AGV and check if the next node was reached to
  /// indicate task progress (alternative to setNodeReached).
  ///
  /// \param x [m] x position on the map
  /// \param y [m] y position on the map
  /// \param theta [rad] Range : [-Pi … Pi] orientation of the AGV, relative to the map
  /// \param map reference for x, y and theta (has to be the same as the current node's map in order
  /// to reach the node)
  /// \return true, if the next node was reached
  ///
  bool evalPosition(double x, double y, double theta, std::string_view map) const noexcept(false);

  ///
  /// \brief Set the current position with deviation of the AGV and check if the next node was
  /// reached to indicate task progress (alternative to setNodeReached).
  ///
  /// \param x [m] x position on the map
  /// \param y [m] y position on the map
  /// \param theta [rad] Range : [-Pi … Pi] orientation of the AGV, relative to the map
  /// \param map reference for x, y and theta (has to be the same as the current node's map in order
  /// to reach the node)
  /// \param deviation deviation of the position (if the node deviation fully encloses the agv
  /// deviation, it is considered as reached)
  /// \return true, if the next node was reached
  ///
  bool evalPosition(double x, double y, double theta, std::string_view map, double deviation) const
      noexcept(false);

  ///
  /// \brief Set the current position (full vda5050 data). Does not indicate task progress.
  ///
  /// NOTE: The distanceSinceLastNode will be the length of the linear interpolated path described
  /// by all calls to "setPosition" functions.
  ///
  /// \param position the position of the AGV
  ///
  void setPosition(const vda5050::AGVPosition &position) const noexcept(false);

  ///
  /// \brief Set the current position of the AGV. Does not indicate task progress.
  ///
  /// NOTE: The distanceSinceLastNode will be the length of the linear interpolated path described
  /// by all calls to "setPosition" functions.
  ///
  /// \param x [m] x position on the map
  /// \param y [m] y position on the map
  /// \param theta [rad] Range : [-Pi … Pi] orientation of the AGV, relative to the map
  /// \param map reference for x, y and theta
  ///
  void setPosition(double x, double y, double theta, std::string_view map) const noexcept(false);

  ///
  /// \brief Set the current position with deviation of the AGV. Does not indicate task progress.
  ///
  /// NOTE: The distanceSinceLastNode will be the length of the linear interpolated path described
  /// by all calls to "setPosition" functions.
  ///
  /// \param x [m] x position on the map
  /// \param y [m] y position on the map
  /// \param theta [rad] Range : [-Pi … Pi] orientation of the AGV, relative to the map
  /// \param map reference for x, y and theta (has to be the same as the current node's map in order
  /// to reach the node)
  /// \param deviation deviation of the position (if the nodes deviation and the agvs deviation
  /// intersect, the node is reached)
  ///
  void setPosition(double x, double y, double theta, std::string_view map, double deviation) const
      noexcept(false);
};

}  // namespace vda5050pp::handler

#endif  // PUBLIC_VDA5050_2B_2B_HANDLER_BASE_NAVIGATION_HANDLER_H_
