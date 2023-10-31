// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains the factsheet gather function implementations.
//

#include "vda5050++/core/factsheet/gather.h"

#include <algorithm>

#include "vda5050++/config/state_update_timer_subconfig.h"
#include "vda5050++/config/visualization_timer_subconfig.h"
#include "vda5050++/core/instance.h"
#include "vda5050++/core/logger.h"

using namespace std::chrono_literals;

inline std::shared_ptr<spdlog::logger> getFactsheetLogger() {
  return vda5050pp::core::getRemappedLogger(
      vda5050pp::core::module_keys::k_factsheet_event_handler_key);
}

vda5050::AgvGeometry vda5050pp::core::factsheet::gatherGeometry() {
  return Instance::ref().getConfig().getAgvDescription().agv_geometry;
}

vda5050::LoadSpecification vda5050pp::core::factsheet::gatherLoadSpecification() {
  return Instance::ref().getConfig().getAgvDescription().load_specification;
}

vda5050::LocalizationParameters vda5050pp::core::factsheet::gatherLocalizationParameters() {
  return Instance::ref().getConfig().getAgvDescription().localization_parameters;
}

vda5050::PhysicalParameters vda5050pp::core::factsheet::gatherPhysicalParameters() {
  return Instance::ref().getConfig().getAgvDescription().physical_parameters;
}

vda5050::ProtocolFeatures vda5050pp::core::factsheet::gatherProtocolFeatures() {
  vda5050::ProtocolFeatures protocol_features;

  // Gather control actions
  auto control_action_list =
      std::make_shared<vda5050pp::core::events::FactsheetControlActionListEvent>();
  auto control_action_list_result = control_action_list->getFuture();
  Instance::ref().getFactsheetEventManager().synchronousDispatch(control_action_list);
  if (control_action_list_result.wait_for(0s) == std::future_status::timeout) {
    throw vda5050pp::VDA5050PPSynchronizedEventTimedOut(
        MK_FN_EX_CONTEXT("Internal FactsheetControlActionListEvent not handled."));
  }
  auto actions = control_action_list_result.get();

  // Gather user actions
  auto action_list = std::make_shared<vda5050pp::events::ActionList>();
  auto action_list_result = action_list->getFuture();
  Instance::ref().getActionEventManager().dispatch(action_list);
  if (action_list_result.wait_for(1s) == std::future_status::timeout) {
    throw vda5050pp::VDA5050PPSynchronizedEventTimedOut(
        MK_FN_EX_CONTEXT("ActionList Event timed out."));
  }

  if (auto ptr = action_list_result.get(); actions && ptr) {
    getFactsheetLogger()->info("Gathered {} Control-Actions and {} AGV-Actions", actions->size(),
                               ptr->size());
    actions->splice(actions->end(), *ptr);
  } else {
    throw vda5050pp::VDA5050PPNullPointer(MK_FN_EX_CONTEXT("ActionList contained a nullptr"));
  }

  protocol_features.agvActions = std::vector<vda5050::AgvAction>{actions->begin(), actions->end()};

  return protocol_features;
}

vda5050::ProtocolLimits vda5050pp::core::factsheet::gatherProtocolLimits() {
  vda5050::ProtocolLimits protocol_limits;

  const auto &desc = Instance::ref().getConfig().getAgvDescription();

  protocol_limits.maxArrayLens.stateLoads = desc.simple_protocol_limits.max_load_id_len;
  protocol_limits.maxStringLens.idLen = desc.simple_protocol_limits.max_id_len;
  protocol_limits.maxStringLens.idNumericalOnly = desc.simple_protocol_limits.id_numerical_only;
  protocol_limits.maxStringLens.loadIdLen = desc.simple_protocol_limits.max_load_id_len;

  protocol_limits.timing.minStateInterval = 0.0;
  protocol_limits.timing.minOrderInterval = 0.0;
  protocol_limits.timing.defaultStateInterval =
      float(std::chrono::duration_cast<std::chrono::milliseconds>(
                Instance::ref()
                    .getConfig()
                    .lookupModuleConfigAs<vda5050pp::config::StateUpdateTimerSubConfig>(
                        module_keys::k_state_update_timer_key)
                    ->getMaxUpdatePeriod())
                .count()) /
      1000.0;
  protocol_limits.timing.visualizationInterval =
      float(std::chrono::duration_cast<std::chrono::milliseconds>(
                Instance::ref()
                    .getConfig()
                    .lookupModuleConfigAs<vda5050pp::config::VisualizationTimerSubConfig>(
                        module_keys::k_visualization_timer_key)
                    ->getVisualizationPeriod())
                .count()) /
      1000.0;

  return protocol_limits;
}

vda5050::TypeSpecification vda5050pp::core::factsheet::gatherTypeSpecification() {
  return Instance::ref().getConfig().getAgvDescription().type_specification;
}