// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/core/checks/order.h"

#include <catch2/catch_all.hpp>

#include "vda5050++/core/instance.h"

TEST_CASE("core::checks::checkOrderGraphConsistency", "[core][checks]") {
  WHEN("An empty order is validated") {
    vda5050::Order order;

    THEN("An error is returned") {
      REQUIRE_FALSE(vda5050pp::core::checks::checkOrderGraphConsistency(order).empty());
    }
  }

  WHEN("The order contains a node with an odd sequence id") {
    vda5050::Order order;
    vda5050::Node node;
    node.sequenceId = 1;
    order.nodes.push_back(node);

    THEN("An error is returned") {
      REQUIRE_FALSE(vda5050pp::core::checks::checkOrderGraphConsistency(order).empty());
    }
  }

  WHEN("The order contains a duplicate node sequence id") {
    vda5050::Order order;
    vda5050::Node node;
    node.sequenceId = 0;
    order.nodes.push_back(node);
    order.nodes.push_back(node);

    THEN("An error is returned") {
      REQUIRE_FALSE(vda5050pp::core::checks::checkOrderGraphConsistency(order).empty());
    }
  }

  WHEN("The order contains an edge with an even sequence id") {
    vda5050::Order order;
    vda5050::Edge edge;
    edge.sequenceId = 0;
    order.edges.push_back(edge);

    THEN("An error is returned") {
      REQUIRE_FALSE(vda5050pp::core::checks::checkOrderGraphConsistency(order).empty());
    }
  }

  WHEN("The order contains an edge with duplicate ids") {
    vda5050::Order order;
    vda5050::Edge edge;
    edge.sequenceId = 1;
    order.edges.push_back(edge);
    order.edges.push_back(edge);

    THEN("An error is returned") {
      REQUIRE_FALSE(vda5050pp::core::checks::checkOrderGraphConsistency(order).empty());
    }
  }

  WHEN("The order contains skips within the graph") {
    vda5050::Order order;
    vda5050::Node node;
    vda5050::Edge edge;
    node.sequenceId = 0;
    edge.sequenceId = 1;
    order.nodes.push_back(node);
    order.edges.push_back(edge);
    node.sequenceId = 2;
    edge.sequenceId = 3;
    order.nodes.push_back(node);
    order.edges.push_back(edge);
    node.sequenceId = 6;
    order.nodes.push_back(node);

    THEN("An error is returned") {
      REQUIRE_FALSE(vda5050pp::core::checks::checkOrderGraphConsistency(order).empty());
    }
  }

  WHEN("The order contains an invalid base/horizon separation") {
    vda5050::Order order;
    vda5050::Node node;
    vda5050::Edge edge;
    node.sequenceId = 0;
    node.released = true;
    edge.sequenceId = 1;
    edge.released = true;
    order.nodes.push_back(node);
    order.edges.push_back(edge);
    node.sequenceId = 2;
    node.released = false;
    edge.sequenceId = 3;
    edge.released = true;
    order.nodes.push_back(node);
    order.edges.push_back(edge);
    node.released = false;
    node.sequenceId = 6;
    order.nodes.push_back(node);

    THEN("An error is returned") {
      REQUIRE_FALSE(vda5050pp::core::checks::checkOrderGraphConsistency(order).empty());
    }
  }

  WHEN("The order is intact") {
    vda5050::Order order;
    vda5050::Node node;
    vda5050::Edge edge;
    node.sequenceId = 0;
    node.released = true;
    edge.sequenceId = 1;
    edge.released = true;
    order.nodes.push_back(node);
    order.edges.push_back(edge);
    node.sequenceId = 2;
    edge.sequenceId = 3;
    order.nodes.push_back(node);
    order.edges.push_back(edge);
    node.sequenceId = 4;
    order.nodes.push_back(node);

    THEN("No error is returned") {
      REQUIRE(vda5050pp::core::checks::checkOrderGraphConsistency(order).empty());
    }
  }
}

TEST_CASE("core::checks::checkOrderAppend", "[core][checks]") {
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().useWhiteList();
  vda5050pp::core::Instance::reset();
  auto instance = vda5050pp::core::Instance::init(cfg).lock();

  WHEN("There is no current graph") {
    WHEN("An appending order is checked") {
      vda5050::Order order;
      order.orderUpdateId = 1;
      vda5050::Node node;
      vda5050::Edge edge;
      node.sequenceId = 2;
      edge.sequenceId = 3;
      order.nodes.push_back(node);
      order.edges.push_back(edge);
      node.sequenceId = 4;
      order.nodes.push_back(node);

      THEN("An error is returned") {
        REQUIRE_FALSE(vda5050pp::core::checks::checkOrderAppend(order).empty());
      }
    }

    WHEN("An replacing order is checked") {
      vda5050::Order order;
      order.orderUpdateId = 0;
      vda5050::Node node;
      vda5050::Edge edge;
      node.sequenceId = 0;
      edge.sequenceId = 1;
      order.nodes.push_back(node);
      order.edges.push_back(edge);
      node.sequenceId = 2;
      order.nodes.push_back(node);

      THEN("No error is returned") {
        REQUIRE(vda5050pp::core::checks::checkOrderAppend(order).empty());
      }
    }
  }

  WHEN("There is a current graph") {
    auto g0 = std::make_shared<vda5050::Node>();
    auto g1 = std::make_shared<vda5050::Edge>();
    auto g2 = std::make_shared<vda5050::Node>();
    g0->sequenceId = 0;
    g0->released = true;
    g1->sequenceId = 1;
    g1->released = true;
    g2->sequenceId = 2;
    g2->released = true;
    instance->getOrderManager().replaceGraph(
        vda5050pp::core::state::Graph({vda5050pp::core::state::GraphElement(g0),
                                       vda5050pp::core::state::GraphElement(g1),
                                       vda5050pp::core::state::GraphElement(g2)}),
        "order");

    WHEN("The order is active") {
      instance->getOrderManager().setOrderStatus(vda5050pp::misc::OrderStatus::k_order_active);

      WHEN("A replacing order is checked") {
        vda5050::Order order;
        vda5050::Node node;
        node.sequenceId = 0;
        node.released = true;
        order.orderId = "other";
        order.orderUpdateId = 0;
        order.nodes.push_back(node);

        THEN("An error is returned") {
          REQUIRE_FALSE(vda5050pp::core::checks::checkOrderAppend(order).empty());
        }
      }

      WHEN("A extending order is checked") {
        vda5050::Order order;
        order.orderId = "order";
        order.orderUpdateId = 1;
        vda5050::Node node;
        vda5050::Edge edge;
        node.released = true;
        node.sequenceId = 2;
        edge.sequenceId = 3;
        edge.released = true;
        order.nodes.push_back(node);
        order.edges.push_back(edge);
        node.sequenceId = 4;
        order.nodes.push_back(node);

        THEN("No error is returned") {
          REQUIRE(vda5050pp::core::checks::checkOrderAppend(order).empty());
        }
      }
      WHEN("A invalid extending order is checked") {
        vda5050::Order order;
        order.orderId = "order";
        order.orderUpdateId = 1;
        vda5050::Node node;
        vda5050::Edge edge;
        node.released = true;
        node.sequenceId = 4;
        edge.sequenceId = 5;
        edge.released = true;
        order.nodes.push_back(node);
        order.edges.push_back(edge);
        node.sequenceId = 6;
        order.nodes.push_back(node);

        THEN("An error is returned") {
          REQUIRE_FALSE(vda5050pp::core::checks::checkOrderAppend(order).empty());
        }
      }
    }

    WHEN("The order is idle") {
      instance->getOrderManager().setOrderStatus(vda5050pp::misc::OrderStatus::k_order_idle);

      WHEN("A replacing order is checked") {
        vda5050::Order order;
        vda5050::Node node;
        node.sequenceId = 0;
        node.released = true;
        order.orderId = "other";
        order.orderUpdateId = 0;
        order.nodes.push_back(node);

        THEN("No error is returned") {
          REQUIRE(vda5050pp::core::checks::checkOrderAppend(order).empty());
        }
      }
    }
  }
}

TEST_CASE("core::checks::checkOrderId", "[core][checks]") {
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().useWhiteList();
  vda5050pp::core::Instance::reset();
  auto instance = vda5050pp::core::Instance::init(cfg).lock();

  WHEN("The order id is decreased") {
    auto g0 = std::make_shared<vda5050::Node>();
    auto g1 = std::make_shared<vda5050::Edge>();
    auto g2 = std::make_shared<vda5050::Node>();
    g0->sequenceId = 0;
    g0->released = true;
    g1->sequenceId = 1;
    g1->released = true;
    g2->sequenceId = 2;
    g2->released = true;
    instance->getOrderManager().replaceGraph(
        vda5050pp::core::state::Graph({vda5050pp::core::state::GraphElement(g0)}), "order");

    instance->getOrderManager().extendGraph(
        vda5050pp::core::state::Graph({vda5050pp::core::state::GraphElement(g0),
                                       vda5050pp::core::state::GraphElement(g1),
                                       vda5050pp::core::state::GraphElement(g2)}),
        5);

    WHEN("An update with a decreasing order update id is checked") {
      THEN("It fails") {
        vda5050::Order order;
        order.orderId = "order";
        order.orderUpdateId = 4;
        REQUIRE_FALSE(vda5050pp::core::checks::checkOrderId(order).empty());
      }
    }
    WHEN("An update with the same order update id is checked") {
      THEN("It succeeds") {
        vda5050::Order order;
        order.orderId = "order";
        order.orderUpdateId = 5;
        REQUIRE(vda5050pp::core::checks::checkOrderId(order).empty());
      }
    }
    WHEN("When a new order is checked") {
      THEN("It succeeds") {
        vda5050::Order order;
        order.orderId = "order2";
        order.orderUpdateId = 0;
        REQUIRE(vda5050pp::core::checks::checkOrderId(order).empty());
      }
    }
  }
}

TEST_CASE("core::checks::checkOrderActionIds", "[core][checks]") {
  vda5050pp::Config cfg;
  cfg.refGlobalConfig().useWhiteList();
  vda5050pp::core::Instance::reset();
  auto instance = vda5050pp::core::Instance::init(cfg).lock();

  WHEN("An order with duplicate actions is checked") {
    vda5050::Action action;
    action.actionId = "a1";
    vda5050::Node node;
    node.actions.push_back(action);
    vda5050::Order order;
    order.nodes.push_back(node);
    order.nodes.push_back(node);

    THEN("An error is returned") {
      REQUIRE_FALSE(vda5050pp::core::checks::checkOrderActionIds(order).empty());
    }
  }
}