//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PRIVATE_VDA5050_2B_2B_CORE_LOGGER_H_
#define PRIVATE_VDA5050_2B_2B_CORE_LOGGER_H_

#include <spdlog/spdlog.h>

#include <memory>

#include "vda5050++/core/common/exception.h"

namespace vda5050pp::core {

namespace logger_names {
constexpr auto k_agv_handler = "agv_handler";
constexpr auto k_interpreter = "interpreter";
constexpr auto k_messages = "messages";
constexpr auto k_order = "order";
constexpr auto k_state = "state";
constexpr auto k_mqtt = "mqtt";
constexpr auto k_validation = "validation";
constexpr auto k_events = "events";
constexpr auto k_instance = "instance";
constexpr auto k_factsheet = "factsheet";

std::string_view remapModule(std::string_view key);
}  // namespace logger_names

///
///\brief Get a spdlog logger from the registry and guarantee, that it is not nullptr (default)
///
///\tparam String string type
///\param name the logger name
///\param ensure create temporary default logger if name does not exist
///\return std::shared_ptr<spdlog::logger> non-nullptr, either ::get or ::default:logger
///
template <typename String>
inline std::shared_ptr<spdlog::logger> getLogger(String name, bool ensure = true) {
  auto ptr = spdlog::get(static_cast<std::string>(name));
  if (ptr == nullptr && ensure) {
    ptr = spdlog::default_logger();
  }
  return ptr;
}

///
///\brief Get a spdlog logger from the registry and guarantee, that it is not nullptr (default)
///
///\tparam String string type
///\param name the logger name (will be remapped with remapModule)
///\return std::shared_ptr<spdlog::logger> non-nullptr, either ::get or ::default:logger
///
template <typename String> inline std::shared_ptr<spdlog::logger> getRemappedLogger(String name) {
  return (getLogger(logger_names::remapModule(name)));
}

///
///\brief Re-create a logger, first drop, then create
///
///\tparam Sink the sink type
///\tparam String the string type
///\tparam Args the argument types for the sink
///\param name logger name
///\param args sink arguments
///\return std::shared_ptr<spdlog::logger> pointer to the logger
///
template <typename Sink, typename String, typename... Args>
inline std::shared_ptr<spdlog::logger> recreateLogger(const String &name, Args &&...args) {
  spdlog::drop(static_cast<std::string>(name));
  return spdlog::create<Sink>(static_cast<std::string>(name), std::forward<Args>(args)...);
}

/// @brief Get non-nullptr logger for AGVHandler
/// @return non-nullptr logger
inline std::shared_ptr<spdlog::logger> getAGVHandlerLogger() {
  return getLogger(logger_names::k_agv_handler);
}
/// @brief Get non-nullptr logger for Interpreter
/// @return non-nullptr logger
inline std::shared_ptr<spdlog::logger> getInterpreterLogger() {
  return getLogger(logger_names::k_interpreter);
}
/// @brief Get non-nullptr logger for Messages
/// @return non-nullptr logger
inline std::shared_ptr<spdlog::logger> getMessagesLogger() {
  return getLogger(logger_names::k_messages);
}
/// @brief Get non-nullptr logger for Order
/// @return non-nullptr logger
inline std::shared_ptr<spdlog::logger> getOrderLogger() { return getLogger(logger_names::k_order); }
/// @brief Get non-nullptr logger for State
/// @return non-nullptr logger
inline std::shared_ptr<spdlog::logger> getStateLogger() { return getLogger(logger_names::k_state); }
/// @brief Get non-nullptr logger for MQTT
/// @return non-nullptr logger
inline std::shared_ptr<spdlog::logger> getMqttLogger() { return getLogger(logger_names::k_mqtt); }
/// @brief Get non-nullptr logger for Validation
/// @return non-nullptr logger
inline std::shared_ptr<spdlog::logger> getValidationLogger() {
  return getLogger(logger_names::k_validation);
}
/// @brief Get non-nullptr logger for Events
/// @return non-nullptr logger
inline std::shared_ptr<spdlog::logger> getEventsLogger() {
  return getLogger(logger_names::k_events);
}
/// @brief Get non-nullptr logger for Instance
/// @param ensure ensure a non-nullptr logger
/// @return non-nullptr logger
inline std::shared_ptr<spdlog::logger> getInstanceLogger(bool ensure = true) {
  return getLogger(logger_names::k_instance, ensure);
}

}  // namespace vda5050pp::core

#endif  // PRIVATE_VDA5050_2B_2B_CORE_LOGGER_H_
