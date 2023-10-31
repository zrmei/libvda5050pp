//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/handler/init_position_handler.h"

#include "vda5050++/core/instance.h"
#include "vda5050++/misc/action_declarations.h"

using namespace vda5050pp::handler;

void InitPositionHandler::setLastNodeId(std::string_view last_node_id) const {
  auto evt = std::make_shared<vda5050pp::events::NavigationStatusNodeReached>();
  evt->last_node_id = last_node_id;
  evt->node_seq_id = std::nullopt;

  core::Instance::ref().getNavigationStatusManager().dispatch(evt);
}

ActionCallbacks InitPositionHandler::prepare(
    std::shared_ptr<vda5050pp::handler::ActionState>,
    std::shared_ptr<ParametersMap> parameters) noexcept(false) {
  auto on_start = [this, parameters](auto &self) {
    this->handleInitPosition(
        std::get<double>(parameters->at("x")), std::get<double>(parameters->at("y")),
        std::get<double>(parameters->at("theta")), std::get<std::string>(parameters->at("mapId")),
        std::get<std::string>(parameters->at("lastNodeId")), self);
  };

  auto do_nothing = [](const auto &) {
    // Pause, resume and cancel do not make sense here.
  };

  return ActionCallbacks(std::move(on_start), do_nothing, do_nothing, do_nothing);
}

InitPositionHandler::InitPositionHandler()
    : vda5050pp::handler::SimpleActionHandler(
          vda5050pp::misc::action_declarations::k_init_position) {}