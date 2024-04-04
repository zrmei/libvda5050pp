//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/state/state_update_timer.h"

#include <functional>

#include "vda5050++/config/state_update_timer_subconfig.h"
#include "vda5050++/core/common/exception.h"

using namespace vda5050pp::core::state;
using namespace std::chrono_literals;

static std::shared_ptr<spdlog::logger> getStateUpdateTimerLogger() {
  static std::mutex mutex;
  static std::optional<std::string_view> cached_name;
  std::unique_lock lock(mutex);
  if (!cached_name) {
    cached_name = vda5050pp::core::logger_names::remapModule(
        vda5050pp::core::module_keys::k_state_update_timer_key);
  }
  return vda5050pp::core::getLogger(*cached_name);
}

void StateUpdateTimer::timerRoutine(vda5050pp::core::common::StopToken stop_token) {
  getStateUpdateTimerLogger()->debug("entering timerRoutine()");

  DurationT max_update_period = Instance::ref()
                                    .getConfig()
                                    .lookupModuleConfig(module_keys::k_state_update_timer_key)
                                    ->as<vda5050pp::config::StateUpdateTimerSubConfig>()
                                    .getMaxUpdatePeriod();

  stop_token.onStopRequested([this] { this->timer_.disable(); });

  while (!stop_token.stopRequested()) {
    TimePointT wakeup_time_point = this->last_sent_ + max_update_period;

    // If there was any request for an earlier update, use it.
    if (this->next_scheduled_update_.has_value()) {
      wakeup_time_point = std::min(wakeup_time_point, *this->next_scheduled_update_);
    }

    auto status = this->timer_.sleepUntil(wakeup_time_point);

    if (status == vda5050pp::core::common::InterruptableTimerStatus::k_ok) {
      // Timer was not interrupted, so there is no new update time point
      this->doStateUpdate();

      this->last_sent_ = std::chrono::system_clock::now();
      this->next_scheduled_update_.reset();
    } else {
      // Timer was interrupted, so there is a new update time point,
      // update the wakeup time point (in the next iteration) and go to sleep again.
    }
  }

  getStateUpdateTimerLogger()->debug("exiting timerRoutine()");
}

void StateUpdateTimer::handleRequestStateUpdateEvent(
    std::shared_ptr<vda5050pp::core::events::RequestStateUpdateEvent> data) noexcept(false) {
  if (data == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(
        MK_EX_CONTEXT("RequestStateUpdate Event has no data"));
  }

  this->requestUpdate(data->urgency);
  if (auto tkn = data->acquireResultToken(); tkn.isAcquired() && data->urgency.isImmediate()) {
    tkn.setValue();
  }
}

void StateUpdateTimer::doStateUpdate() const {
  auto &instance = vda5050pp::core::Instance::ref();

  auto event = std::make_shared<vda5050pp::core::events::SendStateMessageEvent>();
  event->state = std::make_shared<vda5050::State>();

  instance.getOrderManager().dumpTo(*event->state);
  instance.getStatusManager().dumpTo(*event->state);

  getStateUpdateTimerLogger()->debug("Dispatching SendStateMessageEvent");
  instance.getMessageEventManager().dispatch(event);
}

StateUpdateTimer::StateUpdateTimer()
    : thread_(std::bind(std::mem_fn(&StateUpdateTimer::timerRoutine), this, std::placeholders::_1),
              false) {}

void StateUpdateTimer::initialize(vda5050pp::core::Instance &instance) {
  getStateUpdateTimerLogger()->flush_on(spdlog::level::debug);
  auto cfg = instance.getConfig().lookupModuleConfig(module_keys::k_state_update_timer_key);
  this->state_subscriber_ = instance.getStateEventManager().getScopedSubscriber();
  this->state_subscriber_->subscribe<vda5050pp::core::events::RequestStateUpdateEvent>(std::bind(
      std::mem_fn(&StateUpdateTimer::handleRequestStateUpdateEvent), this, std::placeholders::_1));

  this->last_sent_ = std::chrono::system_clock::now();
  getStateUpdateTimerLogger()->debug("thread_.start()");
  this->thread_.start();
}

void StateUpdateTimer::deinitialize(vda5050pp::core::Instance &) {
  this->last_sent_ = TimePointT();
  this->next_scheduled_update_ = TimePointT();
  this->state_subscriber_.reset();
  getStateUpdateTimerLogger()->debug("thread_.stop()");
  this->thread_.stop();
  if (this->thread_.joinable()) {
    getStateUpdateTimerLogger()->debug("thread_.join()");
    this->thread_.join();
  }
  getStateUpdateTimerLogger()->debug("thread_.reset()");
  this->thread_.reset(
      std::bind(std::mem_fn(&StateUpdateTimer::timerRoutine), this, std::placeholders::_1));
}

std::string_view StateUpdateTimer::describe() const { return "StateUpdateTimer"; }

std::shared_ptr<vda5050pp::config::ModuleSubConfig> StateUpdateTimer::generateSubConfig() const {
  return std::make_shared<vda5050pp::config::StateUpdateTimerSubConfig>();
}

void StateUpdateTimer::requestUpdate(StateUpdateUrgency urgency) noexcept(true) {
  auto update_time_point = std::chrono::system_clock::now() + urgency.getMaxDelay();

  if (this->next_scheduled_update_.has_value()) {
    update_time_point = std::min(update_time_point, *this->next_scheduled_update_);
  }

  getStateUpdateTimerLogger()->debug("Requesting Update");

  if (urgency.isImmediate()) {
    // Immediate requires blocking until the state is actually sent
    // -> Send it synchronously
    this->doStateUpdate();
    this->last_sent_ = std::chrono::system_clock::now();
    this->next_scheduled_update_.reset();
    this->timer_.interruptAll();  // cancel current sleep

  } else {
    // Set new update timepoint and interrupt
    this->next_scheduled_update_ = update_time_point;
    this->timer_.interruptAll();
  }
}
