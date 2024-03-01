//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/state/status_manager.h"

#include <algorithm>
#include <mutex>

#include "vda5050++/core/common/exception.h"

using namespace vda5050pp::core::state;

void StatusManager::setAGVPosition(const vda5050::AGVPosition &agv_position) {
  std::unique_lock lock(this->mutex_);
  this->agv_position_ = agv_position;
}

vda5050::AGVPosition StatusManager::getAGVPosition() {
  std::shared_lock lock(this->mutex_);
  return this->agv_position_;
}

void StatusManager::setVelocity(const vda5050::Velocity &velocity) {
  std::unique_lock lock(this->mutex_);
  this->velocity_ = velocity;
}

std::optional<vda5050::Velocity> StatusManager::getVelocity() const {
  std::shared_lock lock(this->mutex_);
  return this->velocity_;
}

void StatusManager::resetVelocity() {
  std::unique_lock lock(this->mutex_);
  this->velocity_.reset();
}

void StatusManager::setDriving(bool driving) {
  std::unique_lock lock(this->mutex_);
  this->driving_ = driving;
}

void StatusManager::setDistanceSinceLastNode(double distance_since_last_node) {
  std::unique_lock lock(this->mutex_);
  this->distance_since_last_node_ = distance_since_last_node;
}

void StatusManager::resetDistanceSinceLastNode() {
  std::unique_lock lock(this->mutex_);
  this->distance_since_last_node_.reset();
}

void StatusManager::addLoad(const vda5050::Load &load) {
  std::unique_lock lock(this->mutex_);

  if (!this->loads_.has_value()) {
    this->loads_ = {load};
  } else {
    this->loads_->push_back(load);
  }
}

void StatusManager::removeLoad(std::string_view load_id) {
  std::unique_lock lock(this->mutex_);

  if (!this->loads_.has_value()) {
    return;
  }

  auto match_id = [load_id](const vda5050::Load &load) { return load.loadId == load_id; };

  this->loads_->erase(std::remove_if(this->loads_->begin(), this->loads_->end(), match_id),
                      this->loads_->end());
}

const std::vector<vda5050::Load> &StatusManager::getLoads() {
  std::shared_lock lock(this->mutex_);  // Ensure that loads is not being altered at the moment

  const static std::vector<vda5050::Load> empty;

  // value_or turns empty into a stack object, so this if block is required
  if (this->loads_.has_value()) {
    return *this->loads_;
  } else {
    return empty;
  }
}

void StatusManager::setOperatingMode(vda5050::OperatingMode operating_mode) {
  std::unique_lock lock(this->mutex_);
  this->operating_mode_ = operating_mode;
}

vda5050::OperatingMode StatusManager::getOperatingMode() {
  std::shared_lock lock(this->mutex_);  // Ensure that mode is not being altered at the moment
  return this->operating_mode_;
}

void StatusManager::setBatteryState(const vda5050::BatteryState &battery_state) {
  std::unique_lock lock(this->mutex_);
  this->battery_state_ = battery_state;
}

const vda5050::BatteryState &StatusManager::getBatteryState() {
  std::shared_lock lock(this->mutex_);  // Ensure that battery is not being altered at the moment
  return this->battery_state_;
}

void StatusManager::requestNewBase() {
  std::unique_lock lock(this->mutex_);
  this->new_base_request_ = true;
}

void StatusManager::addError(const vda5050::Error &error) {
  std::unique_lock lock(this->mutex_);
  this->errors_.push_back(error);
}

void StatusManager::addInfo(const vda5050::Info &info) {
  std::unique_lock lock(this->mutex_);
  this->information_.push_back(info);
}

void StatusManager::dumpTo(vda5050::State &state) {
  std::shared_lock lock(this->mutex_);
  state.agvPosition = this->agv_position_;
  state.batteryState = this->battery_state_;
  state.distanceSinceLastNode = this->distance_since_last_node_;
  state.driving = this->driving_;
  state.errors = this->errors_;
  state.information = this->information_;
  state.loads = this->loads_;
  state.newBaseRequest = this->new_base_request_;
  state.operatingMode = this->operating_mode_;
  state.safetyState = this->safety_state_;
  state.velocity = this->velocity_;
}
