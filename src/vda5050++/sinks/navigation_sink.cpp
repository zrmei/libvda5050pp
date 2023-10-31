//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/sinks/navigation_sink.h"

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/instance.h"

using namespace vda5050pp::sinks;

void NavigationSink::setPosition(const vda5050::AGVPosition &agv_position) const noexcept(false) {
  auto pos_evt = std::make_shared<vda5050pp::events::NavigationStatusPosition>();
  pos_evt->position = agv_position;
  vda5050pp::core::Instance::ref().getNavigationStatusManager().dispatch(pos_evt);
}

void NavigationSink::setVelocity(const vda5050::Velocity &velocity) const noexcept(false) {
  auto vel_evt = std::make_shared<vda5050pp::events::NavigationStatusVelocity>();
  vel_evt->velocity = velocity;
  vda5050pp::core::Instance::ref().getNavigationStatusManager().dispatch(vel_evt);
}

void NavigationSink::setDriving(bool driving) const noexcept(false) {
  auto drv_evt = std::make_shared<vda5050pp::events::NavigationStatusDriving>();
  drv_evt->is_driving = driving;
  vda5050pp::core::Instance::ref().getNavigationStatusManager().dispatch(drv_evt);
}

void NavigationSink::setNodeReached(decltype(vda5050::Node::sequenceId) seq_id) const
    noexcept(false) {
  auto rch_evt = std::make_shared<vda5050pp::events::NavigationStatusNodeReached>();
  rch_evt->node_seq_id = seq_id;
  vda5050pp::core::Instance::ref().getNavigationStatusManager().dispatch(rch_evt);
}

void NavigationSink::setNodeReached(std::shared_ptr<const vda5050::Node> node) const
    noexcept(false) {
  if (node == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(MK_EX_CONTEXT("node is nullptr"));
  }

  this->setNodeReached(node->sequenceId);
}

void NavigationSink::setLastNodeId(std::string_view last_node_id) const noexcept(false) {
  auto rch_evt = std::make_shared<vda5050pp::events::NavigationStatusNodeReached>();
  rch_evt->last_node_id = last_node_id;
  vda5050pp::core::Instance::ref().getNavigationStatusManager().dispatch(rch_evt);
}

void NavigationSink::setDistanceSinceLastNode(double distance_since_last_node) const
    noexcept(false) {
  auto dst_evt = std::make_shared<vda5050pp::events::NavigationStatusDistanceSinceLastNode>();
  dst_evt->distance_since_last_node = distance_since_last_node;
  vda5050pp::core::Instance::ref().getNavigationStatusManager().dispatch(dst_evt);
}

void NavigationSink::setNavigationStatus(
    vda5050pp::events::NavigationStatusControlType status) const noexcept(false) {
  auto sta_evt = std::make_shared<vda5050pp::events::NavigationStatusControl>();
  sta_evt->type = status;
  vda5050pp::core::Instance::ref().getNavigationStatusManager().dispatch(sta_evt);
}