// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains the implementation of the VisualizationTimer module
//
//

#include "vda5050++/core/state/visualization_timer.h"

#include "vda5050++/config/visualization_timer_subconfig.h"
#include "vda5050++/core/logger.h"

using namespace vda5050pp::core::state;
using namespace std::chrono_literals;

inline std::shared_ptr<spdlog::logger> getVisualizationTimerLogger() {
  return vda5050pp::core::getRemappedLogger(
      vda5050pp::core::module_keys::k_visualization_timer_key);
}

void VisualizationTimer::timerRoutine(vda5050pp::core::common::StopToken stop_token) const {
  getVisualizationTimerLogger()->debug("timerRoutine(): enter");

  while (!stop_token.stopRequested()) {
    switch (this->timer_.sleepFor(this->update_period_)) {
      case vda5050pp::core::common::InterruptableTimerStatus::k_ok:
        // visualization update
        this->sendVisualization();
        break;
      case vda5050pp::core::common::InterruptableTimerStatus::k_interrupted:
        // don't do anything
        break;
      case vda5050pp::core::common::InterruptableTimerStatus::k_disabled:
        getVisualizationTimerLogger()->debug("timerRoutine(): status=k_disabled");
        // exit routine
        return;
      default:
        getVisualizationTimerLogger()->error("timerRoutine(): invalid status=k_disabled");
        // invalid
        return;
    }
  }

  getVisualizationTimerLogger()->debug("timerRoutine(): exit");
}

void VisualizationTimer::sendVisualization() const {
  auto evt = std::make_shared<vda5050pp::core::events::SendVisualizationMessageEvent>();
  evt->visualization = std::make_shared<vda5050::Visualization>();
  evt->visualization->agvPosition = Instance::ref().getStatusManager().getAGVPosition();
  if (auto maybe_vel = Instance::ref().getStatusManager().getVelocity(); maybe_vel) {
    evt->visualization->velocity = *maybe_vel;
    Instance::ref().getStatusManager().resetVelocity();
  }

  Instance::ref().getMessageEventManager().dispatch(evt);
}

VisualizationTimer::VisualizationTimer()
    : thread_(
          std::bind(std::mem_fn(&VisualizationTimer::timerRoutine), this, std::placeholders::_1),
          false) {}

void VisualizationTimer::initialize(vda5050pp::core::Instance &instance) {
  this->update_period_ = instance.getConfig()
                             .lookupModuleConfigAs<vda5050pp::config::VisualizationTimerSubConfig>(
                                 module_keys::k_visualization_timer_key)
                             ->getVisualizationPeriod();
  this->timer_.enable();
  this->thread_.start();
}

void VisualizationTimer::deinitialize(vda5050pp::core::Instance &) {
  this->timer_.disable();
  this->thread_.stop();
  if (this->thread_.joinable()) {
    this->thread_.join();
  }
  this->thread_.reset(
      std::bind(std::mem_fn(&VisualizationTimer::timerRoutine), this, std::placeholders::_1));
}

std::string_view VisualizationTimer::describe() const { return "VisualizationTimer"; }

std::shared_ptr<vda5050pp::config::ModuleSubConfig> VisualizationTimer::generateSubConfig() const {
  return std::make_shared<vda5050pp::config::VisualizationTimerSubConfig>();
}