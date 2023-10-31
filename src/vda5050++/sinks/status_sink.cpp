//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/sinks/status_sink.h"

#include <chrono>

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/instance.h"
using namespace vda5050pp::sinks;
using namespace std::chrono_literals;

void StatusSink::addLoad(const vda5050::Load &load) const noexcept(false) {
  auto event = std::make_shared<vda5050pp::events::LoadAdd>();
  event->load = load;

  vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(event);
}

void StatusSink::removeLoad(std::string_view load_id) const noexcept(false) {
  auto event = std::make_shared<vda5050pp::events::LoadRemove>();
  event->load_id = load_id;

  vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(event);
}

std::vector<vda5050::Load> StatusSink::getLoads() const noexcept(false) {
  auto event = std::make_shared<vda5050pp::events::LoadsGet>();
  auto future = event->getFuture();

  vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(event);

  if (future.wait_for(1s) != std::future_status::ready) {
    throw vda5050pp::VDA5050PPSynchronizedEventTimedOut(MK_EX_CONTEXT(""));
  }

  return future.get();
}

void StatusSink::alterLoads(
    std::function<void(std::vector<vda5050::Load> &)> &&alter_function) const noexcept(false) {
  auto event = std::make_shared<vda5050pp::events::LoadsAlter>();
  event->alter_function = std::move(alter_function);

  vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(event);
}

void StatusSink::setOperatingMode(vda5050::OperatingMode operating_mode) const noexcept(false) {
  auto event = std::make_shared<vda5050pp::events::OperatingModeSet>();
  event->operating_mode = operating_mode;

  vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(event);
}

vda5050::OperatingMode StatusSink::getOperatingMode() const noexcept(false) {
  auto event = std::make_shared<vda5050pp::events::OperatingModeGet>();
  auto future = event->getFuture();

  vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(event);

  if (future.wait_for(1s) != std::future_status::ready) {
    throw vda5050pp::VDA5050PPSynchronizedEventTimedOut(MK_EX_CONTEXT(""));
  }

  return future.get();
}

void StatusSink::alterOperatingMode(
    std::function<vda5050::OperatingMode(vda5050::OperatingMode)> &&alter_function) const
    noexcept(false) {
  auto event = std::make_shared<vda5050pp::events::OperatingModeAlter>();
  event->alter_function = std::move(alter_function);

  vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(event);
}

void StatusSink::setBatteryState(const vda5050::BatteryState &battery_state) const noexcept(false) {
  auto event = std::make_shared<vda5050pp::events::BatteryStateSet>();
  event->battery_state = battery_state;

  vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(event);
}

vda5050::BatteryState StatusSink::getBatteryState() const noexcept(false) {
  auto event = std::make_shared<vda5050pp::events::BatteryStateGet>();
  auto future = event->getFuture();

  vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(event);

  if (future.wait_for(1s) != std::future_status::ready) {
    throw vda5050pp::VDA5050PPSynchronizedEventTimedOut(MK_EX_CONTEXT(""));
  }

  return future.get();
}

void StatusSink::alterBatteryState(
    std::function<void(vda5050::BatteryState &)> &&alter_function) const noexcept(false) {
  auto event = std::make_shared<vda5050pp::events::BatteryStateAlter>();
  event->alter_function = std::move(alter_function);

  vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(event);
}

void StatusSink::requestNewBase() const noexcept(false) {
  auto event = std::make_shared<vda5050pp::events::RequestNewBase>();

  vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(event);
}

void StatusSink::addError(const vda5050::Error &error) const noexcept(false) {
  auto event = std::make_shared<vda5050pp::events::ErrorAdd>();
  event->error = error;

  vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(event);
}

void StatusSink::alterErrors(
    std::function<void(std::vector<vda5050::Error> &)> &&alter_function) const noexcept(false) {
  auto event = std::make_shared<vda5050pp::events::ErrorsAlter>();
  event->alter_function = std::move(alter_function);

  vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(event);
}

void StatusSink::addInfo(const vda5050::Info &info) const noexcept(false) {
  auto event = std::make_shared<vda5050pp::events::InfoAdd>();
  event->info = info;

  vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(event);
}

void StatusSink::alterInfos(
    std::function<void(std::vector<vda5050::Info> &)> &&alter_function) const noexcept(false) {
  auto event = std::make_shared<vda5050pp::events::InfosAlter>();
  event->alter_function = std::move(alter_function);

  vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(event);
}