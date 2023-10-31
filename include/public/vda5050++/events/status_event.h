//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_EVENTS_STATUS_EVENT_H_
#define PUBLIC_VDA5050_2B_2B_EVENTS_STATUS_EVENT_H_

#include <functional>
#include <vector>

#include "vda5050++/events/synchronized_event.h"
#include "vda5050/BatteryState.h"
#include "vda5050/Error.h"
#include "vda5050/Info.h"
#include "vda5050/Load.h"
#include "vda5050/OperatingMode.h"

namespace vda5050pp::events {

///
///\brief The StatusEvent ID type.
///
enum class StatusEventType {
  k_none,
  k_load_add,
  k_load_remove,
  k_loads_get,
  k_loads_alter,
  k_operating_mode_set,
  k_operating_mode_get,
  k_operating_mode_alter,
  k_battery_state_set,
  k_battery_state_get,
  k_battery_state_alter,
  k_request_new_base,
  k_error_add,
  k_errors_alter,
  k_info_add,
  k_infos_alter,
};

///
///\brief The StatusEvent base type.
///
struct StatusEvent {
  ///
  ///\brief An identifier field (convenience field for type identification after downcasting).
  ///
  StatusEventType type = StatusEventType::k_none;
};

///
///\brief Convenience template for each concrete StatusEvent
///
///\tparam t the identifier key
///
template <StatusEventType t> struct StatusEventId : public StatusEvent {
  ///
  ///\brief This constructor sets the type of the StatusEvent base class for runtime types.
  ///
  StatusEventId() { StatusEvent::type = t; }

  ///
  ///\brief Obtain the ID at compile time
  ///
  ///\return constexpr StatusEventType
  ///
  static constexpr StatusEventType getType() { return t; }
};

///
///\brief This event can be dispatched by the user to add a load to the state.
///
struct LoadAdd : public StatusEventId<StatusEventType::k_load_add> {
  vda5050::Load load;
};

///
///\brief This event can be dispatched by the user to remove a load from the state.
///
struct LoadRemove : public StatusEventId<StatusEventType::k_load_remove> {
  std::string load_id;
};

///
///\brief This event can be dispatched by the user to get all loads from the state.
/// The library will set the result, such that this->getFuture() will eventually contain the result.
///
struct LoadsGet : public StatusEventId<StatusEventType::k_loads_get>,
                  SynchronizedEvent<std::vector<vda5050::Load>> {};

///
///\brief This event can be dispatched by the user to alter the loads vector in the state directly.
///
struct LoadsAlter : public StatusEventId<StatusEventType::k_loads_alter> {
  std::function<void(std::vector<vda5050::Load> &)> alter_function;
};

///
///\brief This even can be dispatched by the user to set the OperatingMode in the state.
///
struct OperatingModeSet : public StatusEventId<StatusEventType::k_operating_mode_set> {
  vda5050::OperatingMode operating_mode;
};

///
///\brief This even can be dispatched by the user to get the current operating mode from the state.
/// The library will set the result, such that this->getFuture() will eventually contain the result.
///
struct OperatingModeGet : public StatusEventId<StatusEventType::k_operating_mode_get>,
                          SynchronizedEvent<vda5050::OperatingMode> {};

///
///\brief This event can be dispatched by the user to alter the OperatingMode directly.
///
struct OperatingModeAlter : public StatusEventId<StatusEventType::k_operating_mode_alter> {
  std::function<vda5050::OperatingMode(vda5050::OperatingMode)> alter_function;
};

///
///\brief This event can be dispatched by the user to set the current BatteryState.
///
struct BatteryStateSet : public StatusEventId<StatusEventType::k_battery_state_set> {
  vda5050::BatteryState battery_state;
};

///
///\brief This event can be dispatched by the user to obtain the current BatteryState.
/// The library will set the result, such that this->getFuture() will eventually contain the result.
///
struct BatteryStateGet : public StatusEventId<StatusEventType::k_battery_state_get>,
                         SynchronizedEvent<vda5050::BatteryState> {};

///
///\brief This event can be dispatched by the user to alter the BatteryState directly.
///
struct BatteryStateAlter : public StatusEventId<StatusEventType::k_battery_state_alter> {
  std::function<void(vda5050::BatteryState &)> alter_function;
};

///
///\brief This event can be dispatched by the user to request a new base from the MC.
///
struct RequestNewBase : public StatusEventId<StatusEventType::k_request_new_base> {};

///
///\brief This event can be dispatched by the user to add an Error to the state.
///
struct ErrorAdd : public StatusEventId<StatusEventType::k_error_add> {
  vda5050::Error error;
};

///
///\brief This event can be dispatched by the user to alter the errors vector directly.
///
struct ErrorsAlter : public StatusEventId<StatusEventType::k_errors_alter> {
  std::function<void(std::vector<vda5050::Error> &)> alter_function;
};

///
///\brief This event can be dispatched by the user to add a Info to the state.
///
struct InfoAdd : public StatusEventId<StatusEventType::k_info_add> {
  vda5050::Info info;
};

///
///\brief This event can be dispatched by the user to alter the Infos vector directly.
///
struct InfosAlter : public StatusEventId<StatusEventType::k_infos_alter> {
  std::function<void(std::vector<vda5050::Info> &)> alter_function;
};

}  // namespace vda5050pp::events

#endif  // PUBLIC_VDA5050_2B_2B_EVENTS_STATUS_EVENT_H_
