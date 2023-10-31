//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#include "test/data.h"

#include "test/exception.h"

using namespace test::data;

vda5050::Action test::data::mkAction(std::string_view id, std::string_view type,
                                     vda5050::BlockingType b_type) {
  vda5050::Action ret;
  ret.actionId = id;
  ret.actionType = type;
  ret.blockingType = b_type;

  return ret;
}

vda5050::Edge test::data::mkEdge(std::string_view id, uint32_t seq_id, bool released,
                                 std::vector<vda5050::Action> &&actions) {
  vda5050::Edge ret;
  ret.edgeId = id;
  ret.sequenceId = seq_id;
  ret.released = released;
  ret.actions = std::move(actions);

  return ret;
}

vda5050::Node test::data::mkNode(std::string_view id, uint32_t seq_id, bool released,
                                 std::vector<vda5050::Action> &&actions) {
  vda5050::Node ret;
  ret.nodeId = id;
  ret.sequenceId = seq_id;
  ret.released = released;
  ret.actions = std::move(actions);

  return ret;
}

vda5050::Order test::data::mkTemplateOrder(std::vector<TemplateElement> &&elements) noexcept(
    false) {
  vda5050::Order ret;

  if (elements.empty()) {
    return ret;
  }

  auto expect_next = elements.front().seq_id;

  for (auto &e : elements) {
    if (e.seq_id != expect_next) {
      throw test::exception::VDA5050TestException("Cannot make Order, unexpected seq_id");
    }

    if (e.seq_id % 2 == 0) {
      ret.nodes.push_back(mkNode(e.id, e.seq_id, e.released, std::move(e.actions)));
      if (!ret.edges.empty()) {
        ret.edges.back().endNodeId = ret.nodes.back().nodeId;
      }
    } else {
      ret.edges.push_back(mkEdge(e.id, e.seq_id, e.released, std::move(e.actions)));
      ret.edges.back().startNodeId = ret.nodes.back().nodeId;
    }
    expect_next = e.seq_id + 1;
  }

  return ret;
}