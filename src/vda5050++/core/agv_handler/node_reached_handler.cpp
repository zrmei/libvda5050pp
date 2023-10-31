//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/agv_handler/node_reached_handler.h"

#include "vda5050++/core/agv_handler/functional.h"
#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/common/math/geometry.h"
#include "vda5050++/core/instance.h"

using namespace vda5050pp::core::agv_handler;

inline std::shared_ptr<spdlog::logger> getNodeReachedHandlerLogger() {
  using namespace vda5050pp::core;
  return getRemappedLogger(module_keys::k_node_reached_handler_key);
}

void NodeReachedHandler::handleNavigationStatusPosition(
    std::shared_ptr<vda5050pp::events::NavigationStatusPosition> evt) const {
  if (evt == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(
        MK_EX_CONTEXT("NavigationStatusPosition is nullptr"));
  }

  if (!evt->auto_check_node_reached) {
    return;
  }

  auto result = evt->acquireResultToken();

  if (!result.isAcquired()) {
    getNodeReachedHandlerLogger()->debug("Could not acquire NavigationStatusPosition result token");
    return;
  }

  if (!Instance::ref().getOrderManager().hasGraph()) {
    result.setValue(false);
    return;
  }

  auto goal = Instance::ref().getOrderManager().getCurrentGraph().currentGoal();

  if (!goal.has_value() || !goal->getNode()->released ||
      !goal->getNode()->nodePosition.has_value()) {
    result.setValue(false);
    return;
  }

  auto reached = isOnNode(evt->position, *goal->getNode()->nodePosition,
                          this->sub_config_->getDefaultNodeDeviationXY(),
                          this->sub_config_->getDefaultNodeDeviationTheta(),
                          this->sub_config_->getOverwriteNodeDeviationXY(),
                          this->sub_config_->getOverwriteNodeDeviationTheta());

  getNodeReachedHandlerLogger()->debug(
      "Checking NodeReached AGV({:0.2f},{:0.2f}:{:0.2f}) Node({:0.2f},{:0.2f}:{:0.2f}) -> "
      "reached={}",
      evt->position.x, evt->position.y, evt->position.theta, goal->getNode()->nodePosition->x,
      goal->getNode()->nodePosition->y,
      goal->getNode()->nodePosition->theta.value_or(std::numeric_limits<double>::signaling_NaN()),
      reached);

  if (reached) {
    auto reached_evt = std::make_shared<vda5050pp::events::NavigationStatusNodeReached>();
    reached_evt->node_seq_id = goal->getSequenceId();
    Instance::ref().getNavigationStatusManager().dispatch(reached_evt);
  }

  result.setValue(reached);
}

void NodeReachedHandler::initialize(Instance &instance) {
  this->sub_config_ = instance.getConfig().lookupModuleConfigAs<config::NodeReachedSubConfig>(
      module_keys::k_node_reached_handler_key);

  this->navigation_status_subscriber_ =
      instance.getNavigationStatusManager().getScopedNavigationStatusSubscriber();
  this->navigation_status_subscriber_->subscribe(
      std::bind(std::mem_fn(&NodeReachedHandler::handleNavigationStatusPosition), this,
                std::placeholders::_1));
}

void NodeReachedHandler::deinitialize(Instance &) {
  this->navigation_status_subscriber_.reset();
  this->sub_config_.reset();
}

std::string_view NodeReachedHandler::describe() const { return "NodeReachedHandler"; }

std::shared_ptr<vda5050pp::config::ModuleSubConfig> NodeReachedHandler::generateSubConfig() const {
  return std::make_shared<vda5050pp::config::NodeReachedSubConfig>();
}