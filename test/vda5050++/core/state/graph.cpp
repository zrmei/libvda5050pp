//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/state/graph.h"

#include <catch2/catch.hpp>

#include "vda5050++/exception.h"

TEST_CASE("core::state::GraphElement behaviour", "[core][state]") {
  auto node1 = std::make_shared<vda5050::Node>();
  node1->nodeId = "node1";
  node1->sequenceId = 0;
  node1->released = true;

  auto node2 = std::make_shared<vda5050::Node>();
  node2->nodeId = "node2";
  node2->sequenceId = 2;
  node2->released = false;

  auto edge1 = std::make_shared<vda5050::Edge>();
  edge1->edgeId = "edge1";
  edge1->sequenceId = 1;
  edge1->released = true;

  auto edge2 = std::make_shared<vda5050::Edge>();
  edge2->edgeId = "edge2";
  edge2->sequenceId = 3;
  edge2->released = false;

  vda5050pp::core::state::GraphElement elem_n1(node1);
  vda5050pp::core::state::GraphElement elem_n2(node2);
  vda5050pp::core::state::GraphElement elem_e1(edge1);
  vda5050pp::core::state::GraphElement elem_e2(edge2);

  WHEN("isNode is called") {
    THEN("Nodes return true") {
      REQUIRE(elem_n1.isNode());
      REQUIRE(elem_n2.isNode());
    }
    THEN("Edges return false") {
      REQUIRE_FALSE(elem_e1.isNode());
      REQUIRE_FALSE(elem_e2.isNode());
    }
  }
  WHEN("isEdge is called") {
    THEN("Nodes return false") {
      REQUIRE_FALSE(elem_n1.isEdge());
      REQUIRE_FALSE(elem_n2.isEdge());
    }
    THEN("Edges return true") {
      REQUIRE(elem_e1.isEdge());
      REQUIRE(elem_e2.isEdge());
    }
  }

  WHEN("getSequenceId is called") {
    THEN("It always returns the correct id") {
      REQUIRE(elem_n1.getSequenceId() == 0);
      REQUIRE(elem_e1.getSequenceId() == 1);
      REQUIRE(elem_n2.getSequenceId() == 2);
      REQUIRE(elem_e2.getSequenceId() == 3);
    }
  }

  WHEN("getId is called") {
    THEN("It always returns the correct id") {
      REQUIRE(elem_n1.getId() == "node1");
      REQUIRE(elem_e1.getId() == "edge1");
      REQUIRE(elem_n2.getId() == "node2");
      REQUIRE(elem_e2.getId() == "edge2");
    }
  }

  WHEN("isHorizon is called") {
    THEN("It always returns the correct answer") {
      REQUIRE_FALSE(elem_n1.isHorizon());
      REQUIRE_FALSE(elem_e1.isHorizon());
      REQUIRE(elem_n2.isHorizon());
      REQUIRE(elem_e2.isHorizon());
    }
  }

  WHEN("getNode() is called") {
    THEN("Nodes return a ptr") {
      REQUIRE(elem_n1.getNode() == node1);
      REQUIRE(elem_n2.getNode() == node2);
      REQUIRE_THROWS_AS(elem_e1.getNode(), vda5050pp::VDA5050PPInvalidArgument);
      REQUIRE_THROWS_AS(elem_e2.getNode(), vda5050pp::VDA5050PPInvalidArgument);
    }
  }

  WHEN("getEdge() is called") {
    THEN("Edges return a ptr") {
      REQUIRE(elem_e1.getEdge() == edge1);
      REQUIRE(elem_e2.getEdge() == edge2);
      REQUIRE_THROWS_AS(elem_n1.getEdge(), vda5050pp::VDA5050PPInvalidArgument);
      REQUIRE_THROWS_AS(elem_n2.getEdge(), vda5050pp::VDA5050PPInvalidArgument);
    }
  }
}

TEST_CASE("core::state::Graph behaviour", "[core][state]") {
  auto node1 = std::make_shared<vda5050::Node>();
  node1->nodeId = "node1";
  node1->sequenceId = 0;
  node1->released = true;

  auto node2 = std::make_shared<vda5050::Node>();
  node2->nodeId = "node2";
  node2->sequenceId = 2;
  node2->released = true;

  auto node3 = std::make_shared<vda5050::Node>();
  node3->nodeId = "node3";
  node3->sequenceId = 4;
  node3->released = false;

  auto edge1 = std::make_shared<vda5050::Edge>();
  edge1->edgeId = "edge1";
  edge1->sequenceId = 1;
  edge1->released = true;

  auto edge2 = std::make_shared<vda5050::Edge>();
  edge2->edgeId = "edge2";
  edge2->sequenceId = 3;
  edge2->released = false;

  auto node12 = std::make_shared<vda5050::Node>();
  node12->nodeId = "node12";
  node12->sequenceId = 4;
  node12->released = true;

  auto node22 = std::make_shared<vda5050::Node>();
  node22->nodeId = "node22";
  node22->sequenceId = 6;
  node22->released = true;

  auto node32 = std::make_shared<vda5050::Node>();
  node32->nodeId = "node32";
  node32->sequenceId = 8;
  node32->released = false;

  auto edge12 = std::make_shared<vda5050::Edge>();
  edge12->edgeId = "edge12";
  edge12->sequenceId = 3;
  edge12->released = true;

  auto edge22 = std::make_shared<vda5050::Edge>();
  edge22->edgeId = "edge22";
  edge22->sequenceId = 5;
  edge22->released = true;

  auto edge32 = std::make_shared<vda5050::Edge>();
  edge32->edgeId = "edge32";
  edge32->sequenceId = 7;
  edge32->released = false;

  vda5050pp::core::state::GraphElement gn1(node1);
  vda5050pp::core::state::GraphElement gn2(node2);
  vda5050pp::core::state::GraphElement gn3(node3);
  vda5050pp::core::state::GraphElement ge1(edge1);
  vda5050pp::core::state::GraphElement ge2(edge2);
  vda5050pp::core::state::GraphElement gn12(node12);
  vda5050pp::core::state::GraphElement gn22(node22);
  vda5050pp::core::state::GraphElement gn32(node32);
  vda5050pp::core::state::GraphElement ge12(edge12);
  vda5050pp::core::state::GraphElement ge22(edge22);
  vda5050pp::core::state::GraphElement ge32(edge32);

  vda5050pp::core::state::Graph g1({gn1, gn2, gn3, ge1, ge2});
  vda5050pp::core::state::Graph g2({gn12, gn22, gn32, ge12, ge22, ge32});
  vda5050pp::core::state::Graph g3({gn32, ge22, gn22});

  auto bounds = [](vda5050pp::core::state::GraphElement::SequenceId a,
                   vda5050pp::core::state::GraphElement::SequenceId b) -> decltype(g1.bounds()) {
    return {a, b};
  };

  WHEN("An empty graph is constructed") {
    THEN("An exception is thrown") {
      REQUIRE_THROWS_AS(vda5050pp::core::state::Graph({}), vda5050pp::VDA5050PPInvalidArgument);
    }
  }

  WHEN("Bounds are checked") {
    THEN("They are correct") {
      REQUIRE(g1.bounds() == bounds(0, 4));
      REQUIRE(g1.baseBounds() == bounds(0, 2));
      REQUIRE(g1.horizonBounds() == bounds(3, 4));
      REQUIRE(g2.bounds() == bounds(3, 8));
      REQUIRE(g2.baseBounds() == bounds(3, 6));
      REQUIRE(g2.horizonBounds() == bounds(7, 8));
    };
  }

  WHEN("The horizon is cleared") {
    g1.cleanHorizon();
    g2.cleanHorizon();
    THEN("The base bounds do not change, but the bounds and horizon bounds change") {
      REQUIRE(g1.bounds() == bounds(0, 2));
      REQUIRE(g1.baseBounds() == bounds(0, 2));
      REQUIRE(g2.bounds() == bounds(3, 6));
      REQUIRE(g2.baseBounds() == bounds(3, 6));
    }
    THEN("horizonBounds throws") {
      REQUIRE_THROWS_AS(g1.horizonBounds(), vda5050pp::VDA5050PPInvalidArgument);
      REQUIRE_THROWS_AS(g2.horizonBounds(), vda5050pp::VDA5050PPInvalidArgument);
    }
    THEN("hasHorizon returns false") {
      REQUIRE_FALSE(g1.hasHorizon());
      REQUIRE_FALSE(g2.hasHorizon());
    }
    THEN("hasBase still returns true") {
      REQUIRE(g1.hasBase());
      REQUIRE(g2.hasBase());
    }
  }

  WHEN("at() is called") {
    THEN("the expected element is returned") {
      REQUIRE(g1.at(0) == gn1);
      REQUIRE(g1.at(1) == ge1);
      REQUIRE(g1.at(2) == gn2);
      REQUIRE(g1.at(3) == ge2);
      REQUIRE(g1.at(4) == gn3);
      REQUIRE(g2.at(3) == ge12);
      REQUIRE(g2.at(4) == gn12);
      REQUIRE(g2.at(5) == ge22);
      REQUIRE(g2.at(6) == gn22);
      REQUIRE(g2.at(7) == ge32);
      REQUIRE(g2.at(8) == gn32);
    }
  }

  WHEN("dumpTo() is called") {
    THEN("the expected elements are returned") {
      std::vector<vda5050::NodeState> ns;
      std::vector<vda5050::EdgeState> es;
      g1.dumpTo(ns, es);
      REQUIRE(ns[0].sequenceId == gn1.getSequenceId());
      REQUIRE(ns[1].sequenceId == gn2.getSequenceId());
      REQUIRE(ns[2].sequenceId == gn3.getSequenceId());
      REQUIRE(es[0].sequenceId == ge1.getSequenceId());
      REQUIRE(es[1].sequenceId == ge2.getSequenceId());
      REQUIRE(ns.size() == 3);
      REQUIRE(es.size() == 2);

      std::vector<vda5050::NodeState> ns2;
      std::vector<vda5050::EdgeState> es2;
      g2.dumpTo(ns2, es2);
      REQUIRE(ns2[0].sequenceId == gn12.getSequenceId());
      REQUIRE(ns2[1].sequenceId == gn22.getSequenceId());
      REQUIRE(ns2[2].sequenceId == gn32.getSequenceId());
      REQUIRE(es2[0].sequenceId == ge12.getSequenceId());
      REQUIRE(es2[1].sequenceId == ge22.getSequenceId());
      REQUIRE(es2[2].sequenceId == ge32.getSequenceId());
      REQUIRE(ns2.size() == 3);
      REQUIRE(es2.size() == 3);
    }
  }

  WHEN("g1 has a position") {
    g1.setAgvLastNodeSequenceId(2);
    THEN("extending with g2 without pos maintains the position") {
      g1.extend(std::move(g2));
      REQUIRE(g1.agvPosition() == 2);
    }
    THEN("extending with g2 with a different pos throws") {
      g2.setAgvLastNodeSequenceId(4);
      REQUIRE_THROWS_AS(g1.extend(std::move(g2)), vda5050pp::VDA5050PPInvalidArgument);
    }
    WHEN("trim() is called") {
      g1.trim();

      THEN("All elements before seq 2 are removed") {
        REQUIRE_THROWS_AS(g1.at(0), vda5050pp::VDA5050PPInvalidArgument);
        REQUIRE_THROWS_AS(g1.at(1), vda5050pp::VDA5050PPInvalidArgument);
        REQUIRE(g1.at(2) == gn2);
        REQUIRE(g1.at(3) == ge2);
        REQUIRE(g1.at(4) == gn3);
      }
    }
    WHEN("The last NodeId is not the last released node") {
      g1.setAgvLastNodeSequenceId(0);
      THEN("currentGoal() return the correct goal") {
        REQUIRE(g1.currentGoalSequenceId() == 2);
        REQUIRE(g1.currentGoal() == gn2);
      }
    }
    WHEN("The last NodeId is the last released node") {
      g1.setAgvLastNodeSequenceId(2);
      THEN("currentGoal() return nullopt") { REQUIRE(g1.currentGoal() == std::nullopt); }
    }
  }

  WHEN("Extending g1 without position with g2 with a position") {
    g2.setAgvLastNodeSequenceId(4);
    THEN("The position is present in g1") {
      g1.extend(std::move(g2));
      REQUIRE(g1.agvPosition() == 4);
    }
  }

  WHEN("g1 has no position") {
    THEN("calling agvPosition() throws") {
      REQUIRE_THROWS_AS(g1.agvPosition(), vda5050pp::VDA5050PPInvalidArgument);
    }
    THEN("calling currentGoalSequenceId() throws") {
      REQUIRE_THROWS_AS(g1.currentGoalSequenceId(), vda5050pp::VDA5050PPInvalidArgument);
    }
  }

  WHEN("g1 is extended with g2") {
    g1.extend(std::move(g2));

    THEN("g1 bounds are as expected") {
      REQUIRE(g1.bounds() == bounds(0, 8));
      REQUIRE(g1.baseBounds() == bounds(0, 6));
      REQUIRE(g1.horizonBounds() == bounds(7, 8));
    }
    THEN("g1 old horizon is overwritten with g2 elements") {
      REQUIRE(g1.at(3) == ge12);
      REQUIRE(g1.at(4) == gn12);
    }
    THEN("g1 old base preserved") {
      REQUIRE(g1.at(0) == gn1);
      REQUIRE(g1.at(1) == ge1);
      REQUIRE(g1.at(2) == gn2);
    }
    THEN("g1 contains g2 contents") {
      REQUIRE(g1.at(3) == ge12);
      REQUIRE(g1.at(4) == gn12);
      REQUIRE(g1.at(5) == ge22);
      REQUIRE(g1.at(6) == gn22);
      REQUIRE(g1.at(7) == ge32);
      REQUIRE(g1.at(8) == gn32);
    }
  }

  WHEN("g1 is extended with non appending g3") {
    THEN("An exception is thrown") {
      REQUIRE_THROWS_AS(g1.extend(std::move(g3)), vda5050pp::VDA5050PPInvalidArgument);
    }
  }

  WHEN("The position of an edge is set") {
    THEN("an exception is thrown") {
      REQUIRE_THROWS_AS(g1.setAgvLastNodeSequenceId(1), vda5050pp::VDA5050PPInvalidArgument);
    }
  }

  WHEN("The position outside of the graph is set") {
    THEN("an exception is thrown") {
      REQUIRE_THROWS_AS(g1.setAgvLastNodeSequenceId(100), vda5050pp::VDA5050PPInvalidArgument);
    }
  }

  WHEN("A subgraph is requested") {
    g1.setAgvLastNodeSequenceId(0);
    auto g1_2_4 = g1.subgraph(2, 4);
    auto g1_1_4 = g1.subgraph(1, 4);
    auto g1_0_3 = g1.subgraph(0, 3);

    THEN("Invalid bounds will throw") {
      REQUIRE_THROWS_AS(g1.subgraph(0, 5), vda5050pp::VDA5050PPInvalidArgument);
      REQUIRE_THROWS_AS(g1.subgraph(1, 5), vda5050pp::VDA5050PPInvalidArgument);
      REQUIRE_THROWS_AS(g1.subgraph(3, 2), vda5050pp::VDA5050PPInvalidArgument);
      REQUIRE_THROWS_AS(g1.subgraph(3, 3), vda5050pp::VDA5050PPInvalidArgument);
      REQUIRE_THROWS_AS(g2.subgraph(0, 3), vda5050pp::VDA5050PPInvalidArgument);
      REQUIRE_THROWS_AS(g2.subgraph(0, 6), vda5050pp::VDA5050PPInvalidArgument);
      REQUIRE_THROWS_AS(g2.subgraph(1, 7), vda5050pp::VDA5050PPInvalidArgument);
      REQUIRE_THROWS_AS(g2.subgraph(4, 9), vda5050pp::VDA5050PPInvalidArgument);
    }

    THEN("The bounds match") {
      REQUIRE(g1_2_4.bounds() == bounds(2, 4));
      REQUIRE(g1_2_4.baseBounds() == bounds(2, 2));
      REQUIRE(g1_2_4.horizonBounds() == bounds(3, 4));
      REQUIRE(g1_1_4.bounds() == bounds(1, 4));
      REQUIRE(g1_1_4.baseBounds() == bounds(1, 2));
      REQUIRE(g1_1_4.horizonBounds() == bounds(3, 4));
      REQUIRE(g1_0_3.bounds() == bounds(0, 3));
      REQUIRE(g1_0_3.baseBounds() == bounds(0, 2));
      REQUIRE(g1_0_3.horizonBounds() == bounds(3, 3));
    }
    THEN("The elements are the same") {
      REQUIRE(g1_2_4.at(2) == gn2);
      REQUIRE(g1_2_4.at(3) == ge2);
      REQUIRE(g1_2_4.at(4) == gn3);
      REQUIRE(g1_1_4.at(1) == ge1);
      REQUIRE(g1_1_4.at(2) == gn2);
      REQUIRE(g1_1_4.at(3) == ge2);
      REQUIRE(g1_1_4.at(4) == gn3);
      REQUIRE(g1_0_3.at(0) == gn1);
      REQUIRE(g1_0_3.at(1) == ge1);
      REQUIRE(g1_0_3.at(2) == gn2);
      REQUIRE(g1_0_3.at(3) == ge2);
    }
    THEN("requesting striped elements will throw") {
      REQUIRE_THROWS_AS(g1_2_4.at(0), vda5050pp::VDA5050PPInvalidArgument);
      REQUIRE_THROWS_AS(g1_2_4.at(1), vda5050pp::VDA5050PPInvalidArgument);
      REQUIRE_THROWS_AS(g1_1_4.at(0), vda5050pp::VDA5050PPInvalidArgument);
      REQUIRE_THROWS_AS(g1_0_3.at(4), vda5050pp::VDA5050PPInvalidArgument);
    }
    THEN("The AGV Position is valid in each subgraph") {
      REQUIRE_FALSE(g1_2_4.agvHere());
      REQUIRE_FALSE(g1_1_4.agvHere());
      REQUIRE(g1_0_3.agvHere());
      REQUIRE(g1_0_3.agvPosition() == 0);
    }
  }
}