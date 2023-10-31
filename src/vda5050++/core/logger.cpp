//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/logger.h"

#include "vda5050++/core/instance.h"

using namespace vda5050pp::core;

static const std::map<std::string_view, std::string_view, std::less<>> k_logger_by_module{
    {module_keys::k_action_event_handler_key, logger_names::k_agv_handler},
    {module_keys::k_factsheet_event_handler_key, logger_names::k_factsheet},
    {module_keys::k_navigation_event_handler_key, logger_names::k_agv_handler},
    {module_keys::k_message_event_handler_key, logger_names::k_messages},
    {module_keys::k_interpreter_event_handler_key, logger_names::k_interpreter},
    {module_keys::k_mqtt_key, logger_names::k_mqtt},
    {module_keys::k_order_event_handler_key, logger_names::k_order},
    {module_keys::k_state_event_handler_key, logger_names::k_state},
    {module_keys::k_state_update_timer_key, logger_names::k_state},
    {module_keys::k_validation_event_handler_key, logger_names::k_validation},
};

std::string_view vda5050pp::core::logger_names::remapModule(std::string_view key) {
  if (auto it = k_logger_by_module.find(key); it != k_logger_by_module.end()) {
    return it->second;
  } else {
    return key;
  }
}