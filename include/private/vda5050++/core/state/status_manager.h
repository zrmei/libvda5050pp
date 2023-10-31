//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_STATE_STATUS_MANAGER_H_
#define VDA5050_2B_2B_CORE_STATE_STATUS_MANAGER_H_

#include <vda5050/BatteryState.h>
#include <vda5050/Error.h>
#include <vda5050/Info.h>
#include <vda5050/Load.h>
#include <vda5050/OperatingMode.h>
#include <vda5050/SafetyState.h>
#include <vda5050/State.h>

#include <cstdint>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>

#include "vda5050++/core/common/type_traits.h"

namespace vda5050pp::core::state {

class StatusManager {
private:
  mutable std::shared_mutex mutex_;

  std::optional<std::vector<vda5050::Load>> loads_;
  std::optional<bool> new_base_request_;
  vda5050::BatteryState battery_state_;
  vda5050::OperatingMode operating_mode_;
  std::vector<vda5050::Error> errors_;
  std::vector<vda5050::Info> informations_;
  vda5050::SafetyState safety_state_;
  vda5050::AGVPosition agv_position_;
  std::optional<vda5050::Velocity> velocity_;
  bool driving_ = false;
  std::optional<double> distance_since_last_node_;

public:
  void setAGVPosition(const vda5050::AGVPosition &agv_position);
  vda5050::AGVPosition getAGVPosition();
  void setVelocity(const vda5050::Velocity &velocity);
  void resetVelocity();
  std::optional<vda5050::Velocity> getVelocity() const;
  void setDriving(bool driving);
  void setDistanceSinceLastNode(double distance_since_last_node);
  void resetDistanceSinceLastNode();

  void addLoad(const vda5050::Load &load);
  void removeLoad(std::string_view load_id);
  const std::vector<vda5050::Load> &getLoads();
  void setOperatingMode(vda5050::OperatingMode operating_mode);
  vda5050::OperatingMode getOperatingMode();
  void setBatteryState(const vda5050::BatteryState &battery_state);
  const vda5050::BatteryState &getBatteryState();
  void requestNewBase();
  void addError(const vda5050::Error &error);
  void addInfo(const vda5050::Info &info);

  /// \brief Alter the loads vector
  /// \tparam FunctionT callable void(std::vector<vda5050::Load> &)
  /// \param alter_function the alter_function
  template <typename FunctionT> void loadsAlter(FunctionT alter_function) {
    static_assert(
        vda5050pp::core::common::is_signature<FunctionT, void(std::vector<vda5050::Load> &)>::value,
        "Expected type void(std::vector<vda5050::Load> &)");

    std::unique_lock lock(this->mutex_);

    if (!this->loads_.has_value()) {
      return;
    }

    alter_function(*this->loads_);
  }

  /// \brief Alter the operating mode
  /// \tparam FunctionT callable vda5050::OperatingMode(vda5050::OperatingMode)
  /// \param alter_function
  template <typename FunctionT> void operatingModeAlter(FunctionT alter_function) {
    static_assert(
        vda5050pp::core::common::is_signature<FunctionT, vda5050::OperatingMode(
                                                             vda5050::OperatingMode)>::value,
        "Expected type vda5050::OperatingMode(vda5050::OperatingMode)");
    std::unique_lock lock(this->mutex_);
    this->operating_mode_ = alter_function(this->operating_mode_);
  }

  /// \brief Alter the battery state
  /// \tparam FunctionT callable void(vda5050::BatteryState &)
  /// \param alter_function
  template <typename FunctionT> void alterBatteryState(FunctionT alter_function) {
    static_assert(
        vda5050pp::core::common::is_signature<FunctionT, void(vda5050::BatteryState &)>::value,
        "Expected type void(vda5050::BatteryState &)");
    std::unique_lock lock(this->mutex_);
    alter_function(this->battery_state_);
  }

  /// \brief Alter the errors vector
  /// \tparam FunctionT callable void(std::vector<vda5050::Error> &)
  /// \param alter_function the alter function
  template <typename FunctionT> void alterErrors(FunctionT alter_function) {
    static_assert(vda5050pp::core::common::is_signature<FunctionT,
                                                        void(std::vector<vda5050::Error> &)>::value,
                  "Expected type void(std::vector<vda5050::Error> &)");
    std::unique_lock lock(this->mutex_);
    alter_function(this->errors_);
  }

  /// \brief Alter the infos vector
  /// \tparam FunctionT callable void(std::vector<vda5050::Info> &)
  /// \param alter_function
  template <typename FunctionT> void alterInfos(FunctionT alter_function) {
    static_assert(
        vda5050pp::core::common::is_signature<FunctionT, void(std::vector<vda5050::Info> &)>::value,
        "Expected type void(std::vector<vda5050::Info> &)");
    std::unique_lock lock(this->mutex_);
    alter_function(this->informations_);
  }

  void dumpTo(vda5050::State &state);
};

}  // namespace vda5050pp::core::state

#endif  // VDA5050_2B_2B_CORE_STATE_STATUS_MANAGER_H_
