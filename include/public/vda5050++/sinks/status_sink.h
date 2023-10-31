//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_SINKS_STATUS_SINK_H_
#define PUBLIC_VDA5050_2B_2B_SINKS_STATUS_SINK_H_

#include <functional>
#include <string_view>
#include <vector>

#include "vda5050/BatteryState.h"
#include "vda5050/Error.h"
#include "vda5050/Info.h"
#include "vda5050/Load.h"
#include "vda5050/OperatingMode.h"

namespace vda5050pp::sinks {

///
///\brief The StatusSink can be used to easily dump status related information into the
/// library.
///
class StatusSink {
public:
  ///
  ///\brief Add a new Load to the state.
  ///
  ///\param load the new load.
  ///\throws VDA5050PPNotInitialized if the library is not initialized.
  ///
  void addLoad(const vda5050::Load &load) const noexcept(false);

  ///
  ///\brief Remove a Load from the state.
  ///
  ///\param load_id the id of the load to remove.
  ///\throws VDA5050PPNotInitialized if the library is not initialized.
  ///
  void removeLoad(std::string_view load_id) const noexcept(false);

  ///
  ///\brief Get the current loads.
  ///
  ///\return std::vector<vda5050::Load>
  ///\throws VDA5050PPNotInitialized if the library is not initialized.
  ///
  std::vector<vda5050::Load> getLoads() const noexcept(false);

  ///
  ///\brief Alter the current loads vector directly.
  ///
  ///\param alter_function the alter function.
  ///\throws VDA5050PPNotInitialized if the library is not initialized.
  ///
  void alterLoads(std::function<void(std::vector<vda5050::Load> &)> &&alter_function) const
      noexcept(false);

  ///
  ///\brief Set the state.operatingMode.
  ///
  ///\param operating_mode the new OperatingMode.
  ///\throws VDA5050PPNotInitialized if the library is not initialized.
  ///
  void setOperatingMode(vda5050::OperatingMode operating_mode) const noexcept(false);

  ///
  ///\brief Get the current state.operatingMode.
  ///
  ///\return vda5050::OperatingMode
  ///\throws VDA5050PPNotInitialized if the library is not initialized.
  ///
  vda5050::OperatingMode getOperatingMode() const noexcept(false);

  ///
  ///\brief Alter the current state.operatingMode.
  ///
  ///\param alter_function the altering function.
  ///\throws VDA5050PPNotInitialized if the library is not initialized.
  ///
  void alterOperatingMode(
      std::function<vda5050::OperatingMode(vda5050::OperatingMode)> &&alter_function) const
      noexcept(false);

  ///
  ///\brief Set the state.batteryState.
  ///
  ///\param battery_state the new BatteryState.
  ///\throws VDA5050PPNotInitialized if the library is not initialized.
  ///
  void setBatteryState(const vda5050::BatteryState &battery_state) const noexcept(false);

  ///
  ///\brief Get the current state.batteryState.
  ///
  ///\return vda5050::BatteryState
  ///\throws VDA5050PPNotInitialized if the library is not initialized.
  ///
  vda5050::BatteryState getBatteryState() const noexcept(false);

  ///
  ///\brief Alter the current state.batteryState.
  ///
  ///\param alter_function the altering function.
  ///\throws VDA5050PPNotInitialized if the library is not initialized.
  ///
  void alterBatteryState(std::function<void(vda5050::BatteryState &)> &&alter_function) const
      noexcept(false);

  ///
  ///\brief Request a new base.
  ///
  ///\throws VDA5050PPNotInitialized if the library is not initialized.
  ///
  void requestNewBase() const noexcept(false);

  ///
  ///\brief Add a new error to state.errors.
  ///
  ///\param error the new error.
  ///\throws VDA5050PPNotInitialized if the library is not initialized.
  ///
  void addError(const vda5050::Error &error) const noexcept(false);

  ///
  ///\brief Alter the current state.errors vector.
  ///
  ///\param alter_function the altering function.
  ///\throws VDA5050PPNotInitialized if the library is not initialized.
  ///
  void alterErrors(std::function<void(std::vector<vda5050::Error> &)> &&alter_function) const
      noexcept(false);

  ///
  ///\brief Add a new info to state.informations.
  ///
  ///\param info the new info to add.
  ///\throws VDA5050PPNotInitialized if the library is not initialized.
  ///
  void addInfo(const vda5050::Info &info) const noexcept(false);

  ///
  ///\brief Alter the current state.informations vector.
  ///
  ///\param alter_function the altering function.
  ///\throws VDA5050PPNotInitialized if the library is not initialized.
  ///
  void alterInfos(std::function<void(std::vector<vda5050::Info> &)> &&alter_function) const
      noexcept(false);
};

}  // namespace vda5050pp::sinks

#endif  // PUBLIC_VDA5050_2B_2B_SINKS_STATUS_SINK_H_
