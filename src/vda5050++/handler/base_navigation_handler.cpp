//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/handler/base_navigation_handler.h"

#include <chrono>

#include "vda5050++/core/instance.h"
#include "vda5050++/core/logger.h"

using namespace std::chrono_literals;

using namespace vda5050pp::handler;

static void dispatchStatusControl(vda5050pp::events::NavigationStatusControlType type) {
  auto event = std::make_shared<vda5050pp::events::NavigationStatusControl>();
  event->type = type;

  vda5050pp::core::Instance::ref().getNavigationStatusManager().dispatch(event);
}

void BaseNavigationHandler::setPaused() const {
  dispatchStatusControl(vda5050pp::events::NavigationStatusControlType::k_paused);
}

void BaseNavigationHandler::setResumed() const {
  dispatchStatusControl(vda5050pp::events::NavigationStatusControlType::k_resumed);
}

void BaseNavigationHandler::setFailed() const {
  dispatchStatusControl(vda5050pp::events::NavigationStatusControlType::k_failed);
}

void BaseNavigationHandler::setNodeReached(const vda5050::Node &node) const noexcept(false) {
  this->setNodeReached(node.sequenceId);
}

void BaseNavigationHandler::setNodeReached(uint32_t node_seq) const noexcept(false) {
  auto event = std::make_shared<vda5050pp::events::NavigationStatusNodeReached>();
  event->node_seq_id = node_seq;

  vda5050pp::core::Instance::ref().getNavigationStatusManager().dispatch(event);
}

bool BaseNavigationHandler::evalPosition(const vda5050::AGVPosition &position) const
    noexcept(false) {
  auto event = std::make_shared<vda5050pp::events::NavigationStatusPosition>();
  event->position = position;
  event->auto_check_node_reached = true;
  auto has_reached_node = event->getFuture();

  vda5050pp::core::Instance::ref().getNavigationStatusManager().dispatch(event);

  if (has_reached_node.wait_for(1s) == std::future_status::timeout) {
    core::getAGVHandlerLogger()->error(
        "NavigationStatusPosition event result has timed out after 1s.");

    return false;
  }

  return has_reached_node.get();
}

bool BaseNavigationHandler::evalPosition(double x, double y, double theta,
                                         std::string_view map) const noexcept(false) {
  vda5050::AGVPosition pos;
  pos.x = x;
  pos.y = y;
  pos.theta = theta;
  pos.mapId = map;
  pos.positionInitialized = true;

  return this->evalPosition(pos);
}

bool BaseNavigationHandler::evalPosition(double x, double y, double theta, std::string_view map,
                                         double deviation) const noexcept(false) {
  vda5050::AGVPosition pos;
  pos.x = x;
  pos.y = y;
  pos.theta = theta;
  pos.mapId = map;
  pos.positionInitialized = true;
  pos.deviationRange = deviation;

  return this->evalPosition(pos);
}

void BaseNavigationHandler::setPosition(const vda5050::AGVPosition &position) const
    noexcept(false) {
  auto event = std::make_shared<vda5050pp::events::NavigationStatusPosition>();
  event->position = position;
  event->auto_check_node_reached = false;

  vda5050pp::core::Instance::ref().getNavigationStatusManager().dispatch(event);
}

void BaseNavigationHandler::setPosition(double x, double y, double theta,
                                        std::string_view map) const noexcept(false) {
  vda5050::AGVPosition pos;
  pos.x = x;
  pos.y = y;
  pos.theta = theta;
  pos.mapId = map;
  pos.positionInitialized = true;

  this->setPosition(pos);
}

void BaseNavigationHandler::setPosition(double x, double y, double theta, std::string_view map,
                                        double deviation) const noexcept(false) {
  vda5050::AGVPosition pos;
  pos.x = x;
  pos.y = y;
  pos.theta = theta;
  pos.mapId = map;
  pos.positionInitialized = true;
  pos.deviationRange = deviation;

  this->setPosition(pos);
}