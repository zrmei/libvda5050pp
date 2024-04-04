//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/state/graph.h"

#include <spdlog/fmt/fmt.h>

#include "vda5050++/core/common/exception.h"

using namespace vda5050pp::core::state;

GraphElement::GraphElement(std::shared_ptr<vda5050::Node> node) noexcept(false) : element_(node) {
  if (node == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(MK_EX_CONTEXT(""));
  }
}

GraphElement::GraphElement(std::shared_ptr<vda5050::Edge> edge) noexcept(false) : element_(edge) {
  if (edge == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(MK_EX_CONTEXT(""));
  }
}

GraphElement::SequenceId GraphElement::getSequenceId() const {
  return std::visit([](const auto &arg) { return arg->sequenceId; }, this->element_);
}

std::string_view GraphElement::getId() const {
  return std::visit(
      [](const auto &arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::shared_ptr<vda5050::Node>>) {
          return std::string_view(arg->nodeId);
        } else {
          return std::string_view(arg->edgeId);
        }
      },
      this->element_);
}

bool GraphElement::isNode() const {
  return std::holds_alternative<std::shared_ptr<vda5050::Node>>(this->element_);
}

bool GraphElement::isEdge() const {
  return std::holds_alternative<std::shared_ptr<vda5050::Edge>>(this->element_);
}

bool GraphElement::isHorizon() const {
  return std::visit([](const auto &arg) { return !arg->released; }, this->element_);
}

bool GraphElement::isBase() const { return !this->isHorizon(); }

std::shared_ptr<vda5050::Node> GraphElement::getNode() noexcept(false) {
  try {
    return std::get<std::shared_ptr<vda5050::Node>>(this->element_);
  } catch (const std::bad_variant_access &) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT("GraphElement is not a Node"));
  }
}

std::shared_ptr<vda5050::Edge> GraphElement::getEdge() noexcept(false) {
  try {
    return std::get<std::shared_ptr<vda5050::Edge>>(this->element_);
  } catch (const std::bad_variant_access &) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT("GraphElement is not an Edge"));
  }
}

std::shared_ptr<const vda5050::Node> GraphElement::getNode() const noexcept(false) {
  try {
    return std::get<std::shared_ptr<vda5050::Node>>(this->element_);
  } catch (const std::bad_variant_access &) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT("GraphElement is not a Node"));
  }
}

std::shared_ptr<const vda5050::Edge> GraphElement::getEdge() const noexcept(false) {
  try {
    return std::get<std::shared_ptr<vda5050::Edge>>(this->element_);
  } catch (const std::bad_variant_access &) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT("GraphElement is not an Edge"));
  }
}

GraphElement::operator std::shared_ptr<vda5050::Node>() noexcept(false) { return this->getNode(); }

GraphElement::operator std::shared_ptr<vda5050::Edge>() noexcept(false) { return this->getEdge(); }

GraphElement::operator std::shared_ptr<const vda5050::Node>() const noexcept(false) {
  return this->getNode();
}

GraphElement::operator std::shared_ptr<const vda5050::Edge>() const noexcept(false) {
  return this->getEdge();
}

bool GraphElement::operator==(const GraphElement &rhs) const {
  return this->element_ == rhs.element_;
}

Graph::Graph(decltype(graph_) &&graph) : graph_(std::move(graph)) {}

Graph::Graph(std::initializer_list<GraphElement> elements) noexcept(false) {
  if (elements.size() == 0) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT("Elements must not be empty"));
  }

  for (auto &element : elements) {
    this->update(element);
  }
}

Graph::Graph(const std::list<GraphElement> &elements) noexcept(false) {
  if (elements.empty()) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT("Elements must not be empty"));
  }

  for (auto &element : elements) {
    this->update(element);
  }
}

std::pair<GraphElement::SequenceId, GraphElement::SequenceId> Graph::extend(Graph &&other) noexcept(
    false) {
  // Merge does not splice elements already existing elements

  if (this->agv_pos_.has_value() && other.agv_pos_.has_value() &&
      this->agv_pos_ != other.agv_pos_) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT(fmt::format(
        "Cannot determine AGV location this {}, other {}", *this->agv_pos_, *other.agv_pos_)));
  }

  auto [bf, bl] = this->baseBounds();
  auto [of, ol] = other.bounds();

  // Allow 1-node overlap
  if (bl + 1 != of && bl != of) {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("other [{}, {}] does not extend this [{}, {}]", bf, bl, of, ol)));
  }

  if (!this->agv_pos_.has_value()) {
    this->agv_pos_ = other.agv_pos_;
  }

  this->cleanHorizon();
  other.graph_.merge(std::move(this->graph_));
  this->graph_ = std::move(other.graph_);

  return {std::max(bl + 1, of), ol};
}

void Graph::cleanHorizon() {
  if (!this->hasHorizon()) {
    return;
  }

  auto [first, _] = this->horizonBounds();
  this->graph_.erase(this->graph_.find(first), this->graph_.end());
}

void Graph::update(GraphElement element) noexcept(false) {
  if (this->graph_.empty()) {
    // OK
  } else {
    auto end = this->graph_.end();
    auto after = this->graph_.upper_bound(element.getSequenceId());
    auto before = after;
    before = before == this->graph_.begin() ? end : --before;

    if ((element.isBase() && before != end && before->second.isHorizon()) ||
        (element.isHorizon() && after != end && after->second.isBase())) {
      throw vda5050pp::VDA5050PPInvalidArgument(
          MK_EX_CONTEXT("Inserting this element will break the invariant"));
    }
  }

  this->graph_.insert_or_assign(element.getSequenceId(), std::move(element));
}

bool Graph::hasBase() const { return this->graph_.begin()->second.isBase(); }

bool Graph::hasHorizon() const { return this->graph_.rbegin()->second.isHorizon(); }

std::pair<GraphElement::SequenceId, GraphElement::SequenceId> Graph::bounds() const {
  return {this->graph_.begin()->first, this->graph_.rbegin()->first};
}

std::pair<GraphElement::SequenceId, GraphElement::SequenceId> Graph::baseBounds() const
    noexcept(false) {
  auto match_base = [](const auto &tup) { return tup.second.isBase(); };

  auto begin = this->graph_.begin()->first;

  auto it = std::find_if(this->graph_.rbegin(), this->graph_.rend(), match_base);

  // No Base
  if (it == this->graph_.rend()) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT("Base is Empty"));
  }

  return {begin, it->first};
}

std::pair<GraphElement::SequenceId, GraphElement::SequenceId> Graph::horizonBounds() const
    noexcept(false) {
  auto [_, last] = this->bounds();
  auto [__, base_last] = this->baseBounds();

  // No Horizon
  if (base_last == last) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT("Horizon is Empty"));
  }

  return {base_last + 1, last};
}

GraphElement Graph::at(GraphElement::SequenceId seq) const noexcept(false) {
  if (auto [f, l] = this->bounds(); l < seq || seq < f) {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("Out of bounds ({} is not in range [{},{}]", seq, f, l)));
  }
  return this->graph_.at(seq);
}

Graph Graph::subgraph(GraphElement::SequenceId first, GraphElement::SequenceId last) const
    noexcept(false) {
  auto f = this->graph_.find(first);
  auto l = this->graph_.find(last);

  if (last < first) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT("First must be smaller then Last"));
  }
  if (f == this->graph_.end()) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT("First is not in graph range"));
  }
  if (l == this->graph_.end()) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT("Last is not in graph range"));
  }

  decltype(this->graph_) ret;
  std::copy(f, ++l, std::inserter(ret, ret.end()));

  Graph graph(std::move(ret));
  if (this->agv_pos_.has_value() && first <= *this->agv_pos_ && *this->agv_pos_ <= last) {
    graph.agv_pos_ = this->agv_pos_;
  }

  return graph;
}

Graph Graph::subgraph(std::pair<GraphElement::SequenceId, GraphElement::SequenceId> bounds) const
    noexcept(false) {
  return this->subgraph(bounds.first, bounds.second);
}

bool Graph::agvHere() const { return this->agv_pos_.has_value(); }

GraphElement::SequenceId Graph::agvPosition() const noexcept(false) {
  if (!this->agv_pos_.has_value()) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT("AGV is not on this graph"));
  }

  return *this->agv_pos_;
}

GraphElement::SequenceId Graph::currentGoalSequenceId() const noexcept(false) {
  auto agv_pos = this->agvPosition();

  if (this->at(agv_pos).isNode()) {
    agv_pos += 2;  // Step from node
  } else {
    agv_pos += 1;  // Step from edge
  }

  return agv_pos;
}

std::optional<GraphElement> Graph::currentGoal() const noexcept(false) {
  auto goal_pos = this->currentGoalSequenceId();

  if (auto it = this->graph_.find(goal_pos);
      it != this->graph_.end() && it->second.isNode() && it->second.getNode()->released) {
    return it->second;
  } else {
    return std::nullopt;
  }
}

std::optional<std::string_view> Graph::currentMap() const {
  GraphElement ge =
      this->agvHere() ? this->at(this->agvPosition()) : this->at(this->bounds().first);

  if (ge.isEdge()) {
    ge = this->at(ge.getSequenceId() + 1);
  }

  if (ge.getNode()->nodePosition.has_value()) {
    return ge.getNode()->nodePosition->mapId;
  } else {
    return std::nullopt;
  }
}

void Graph::setAgvLastNodeSequenceId(GraphElement::SequenceId seq_id) noexcept(false) {
  auto it = this->graph_.find(seq_id);

  if (it == this->graph_.end()) {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("SeqId={} is not part if this graph", seq_id)));
  }

  if (!it->second.isNode()) {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("SeqId={} is not a node", seq_id)));
  }

  if (it->second.isHorizon()) {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("SeqId={} is part of the horizon", seq_id)));
  }

  this->agv_pos_ = seq_id;
}

void Graph::trim() {
  if (!this->agv_pos_.has_value()) {
    // nothing to trim
  }

  // agv_pos_ is on the graph -> find does not return end()
  this->graph_.erase(this->graph_.begin(), this->graph_.find(*this->agv_pos_));
}

std::list<std::shared_ptr<const vda5050::Node>> Graph::getNodes() {
  decltype(getNodes()) ret;

  for (const auto &[seq, element] : this->graph_) {
    if (element.isNode()) {
      ret.push_back(element.getNode());
    }
  }

  return ret;
}

std::list<std::shared_ptr<const vda5050::Edge>> Graph::getEdges() {
  decltype(getEdges()) ret;

  for (const auto &[seq, element] : this->graph_) {
    if (element.isEdge()) {
      ret.push_back(element.getEdge());
    }
  }

  return ret;
}

void Graph::dumpTo(std::vector<vda5050::NodeState> &nodes, std::vector<vda5050::EdgeState> &edges,
                   bool skip_agv_elem) const {
  for (const auto &[seq, elem] : this->graph_) {
    if (skip_agv_elem && this->agvHere() && seq == this->agvPosition()) {
      continue;
    }
    if (elem.isNode()) {
      auto node = elem.getNode();
      vda5050::NodeState state;
      state.nodeId = node->nodeId;
      state.nodeDescription = node->nodeDescription;
      state.nodePosition = node->nodePosition;
      state.released = node->released;
      state.sequenceId = node->sequenceId;
      nodes.push_back(state);
    } else if (elem.isEdge()) {
      auto edge = elem.getEdge();
      vda5050::EdgeState state;
      state.edgeId = edge->edgeId;
      state.edgeDescription = edge->edgeDescription;
      state.trajectory = edge->trajectory;
      state.released = edge->released;
      state.sequenceId = edge->sequenceId;
      edges.push_back(state);
    }
  }
}