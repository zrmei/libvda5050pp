//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_STATE_GRAPH_H_
#define VDA5050_2B_2B_CORE_STATE_GRAPH_H_

#include <vda5050/Edge.h>
#include <vda5050/EdgeState.h>
#include <vda5050/Node.h>
#include <vda5050/NodeState.h>

#include <list>
#include <map>
#include <memory>
#include <variant>

namespace vda5050pp::core::state {

class GraphElement {
private:
  std::variant<std::shared_ptr<vda5050::Node>, std::shared_ptr<vda5050::Edge>> element_;

public:
  using SequenceId = decltype(vda5050::Node::sequenceId);
  explicit GraphElement(std::shared_ptr<vda5050::Node> node) noexcept(false);
  explicit GraphElement(std::shared_ptr<vda5050::Edge> edge) noexcept(false);

  SequenceId getSequenceId() const;
  std::string_view getId() const;

  bool isNode() const;
  bool isEdge() const;
  bool isHorizon() const;
  bool isBase() const;

  std::shared_ptr<vda5050::Node> getNode() noexcept(false);
  std::shared_ptr<vda5050::Edge> getEdge() noexcept(false);
  std::shared_ptr<const vda5050::Node> getNode() const noexcept(false);
  std::shared_ptr<const vda5050::Edge> getEdge() const noexcept(false);

  explicit operator std::shared_ptr<vda5050::Node>() noexcept(false);
  explicit operator std::shared_ptr<vda5050::Edge>() noexcept(false);
  explicit operator std::shared_ptr<const vda5050::Node>() const noexcept(false);
  explicit operator std::shared_ptr<const vda5050::Edge>() const noexcept(false);

  bool operator==(const GraphElement &rhs) const;
};

///
/// \brief Manages as VDA5050 Graph
///
/// Invariants:
///   - Not empty
///   - graph_ never has a base element with a sequence number greater then a horizon element
///   - the agv_pos cannot be part of the horizon
///
class Graph {
private:
  std::optional<GraphElement::SequenceId> agv_pos_;
  std::map<GraphElement::SequenceId, GraphElement, std::less<>> graph_;

protected:
  explicit Graph(decltype(graph_) &&graph);

public:
  /// \brief Create a graph with elements
  /// \param elements the graph elements
  /// \throw vda5050pp::VDA5050PPInvalidArgument on empty elements
  explicit Graph(std::initializer_list<GraphElement> elements) noexcept(false);

  /// \brief Create a graph with elements
  /// \param elements the graph elements
  /// \throw vda5050pp::VDA5050PPInvalidArgument on empty elements
  explicit Graph(const std::list<GraphElement> &elements) noexcept(false);

  /// \brief Extend this graph with another graph
  ///   - clears this horizon
  ///   - overwrites with elements of other
  /// \param other the other graph to extend this with
  /// \throw vda5050pp::VDA5050PPInvalidArgument when invariants are broken
  /// \return delta sequence ids (all new elements)
  std::pair<GraphElement::SequenceId, GraphElement::SequenceId> extend(Graph &&other) noexcept(
      false);

  /// \brief Removes every GraphElement with getHorizon() == true
  void cleanHorizon();

  /// \brief update or insert an element
  /// \param element the element to insert or overwrite
  /// \throw vda5050pp::VDA5050PPInvalidArgument when invariants are broken
  void update(GraphElement element) noexcept(false);

  /// \brief Has this graph a base?
  /// \return hasBase
  bool hasBase() const;

  /// \brief Has this graph a horizon?
  /// \return hasHorizon
  bool hasHorizon() const;

  /// \brief Get the sequence id bounds of the graph
  /// \return pair{first_seq, last_seq}
  std::pair<GraphElement::SequenceId, GraphElement::SequenceId> bounds() const;

  /// \brief Get the sequence id bounds of the graph base
  /// \return pair{first_base_seq, last_base_seq}
  /// \throws vda5050pp::VDA5050PPInvalidArgument if there is no base
  std::pair<GraphElement::SequenceId, GraphElement::SequenceId> baseBounds() const noexcept(false);

  /// \brief Get the sequence id bounds of the graph horizon
  /// \return pair{first_horz_seq, last_horz_seq}
  /// \throws vda5050pp::VDA5050PPInvalidArgument if there is no horizon
  std::pair<GraphElement::SequenceId, GraphElement::SequenceId> horizonBounds() const
      noexcept(false);

  /// \brief Get the element with a given sequence number
  /// \param seq the sequence number
  /// \return the element
  /// \throws vda5050pp::VDA5050PPInvalidArgument if there is no such element
  GraphElement at(GraphElement::SequenceId seq) const noexcept(false);

  /// \brief Get a subgraph (copy) with the range [first, last]
  /// \param first the first element seq
  /// \param last  the last element seq
  /// \throws vda5050pp::VDA5050PPInvalidArgument if any sequence id is invalid
  /// \return The subgraph
  Graph subgraph(GraphElement::SequenceId first, GraphElement::SequenceId last) const
      noexcept(false);

  /// \brief Get a subgraph (copy) with the range [bounds.first, bounds.second]
  /// \param bounds the bound tuple
  /// \throws vda5050pp::VDA5050PPInvalidArgument if any sequence id is invalid
  /// \return The subgraph
  Graph subgraph(std::pair<GraphElement::SequenceId, GraphElement::SequenceId> bounds) const
      noexcept(false);

  /// \brief Check if the AGV is on this Graph
  /// \return is AGV here
  bool agvHere() const;

  ///\brief Get the logical AGV position
  ///\throws vda5050pp::VDA5050PPInvalidArgument, if the AGV is not on this Graph
  ///\return GraphElement::SequenceId sequence id
  GraphElement::SequenceId agvPosition() const noexcept(false);

  ///\brief Get the position of the next node after the logical AGV position
  ///\throws vda5050pp::VDA5050PPInvalidArgument, if the AGV is not on this Graph
  ///\return GraphElement::SequenceId the node's sequence id
  GraphElement::SequenceId currentGoalSequenceId() const noexcept(false);

  ///\brief Get the the next goal, if the is one
  ///\throws vda5050pp::VDA5050PPInvalidArgument, if the AGV is not on this Graph
  ///\return std::optional<GraphElement> the current goal
  std::optional<GraphElement> currentGoal() const noexcept(false);

  ///
  ///\brief Get the current map of the AGV if it is here, otherwise the first graph element's map
  ///
  ///\return std::optional<std::string_view> if the map is set in the graph
  ///
  std::optional<std::string_view> currentMap() const;

  /// \brief Set the position of the AGV
  /// \param seq_id  the node sequence id, last visited
  ///\throws vda5050pp::VDA5050PPInvalidArgument, if seq is not on the graph, if seq is part of the
  /// horizon, if seq is not a node
  void setAgvLastNodeSequenceId(GraphElement::SequenceId seq_id) noexcept(false);

  /// \brief Remove all GraphElements before agvPosition()
  void trim();

  /// \brief Construct a list containing all nodes
  /// \return all nodes
  std::list<std::shared_ptr<const vda5050::Node>> getNodes();

  /// \brief Construct a list containing all edges
  /// \return all edges
  std::list<std::shared_ptr<const vda5050::Edge>> getEdges();

  /// \brief Gather all nodes and edges ordered by seq
  /// \param nodes the node states vector to extend
  /// \param edges the edge states vector to extend
  /// \param skip_agv_elem do not dump the element, the AGV is on. (default: false)
  void dumpTo(std::vector<vda5050::NodeState> &nodes, std::vector<vda5050::EdgeState> &edges,
              bool skip_agv_elem = false) const;
};

}  // namespace vda5050pp::core::state

#endif  // VDA5050_2B_2B_CORE_STATE_GRAPH_H_
