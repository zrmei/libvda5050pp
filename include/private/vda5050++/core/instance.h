//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PRIVATE_VDA5050_2B_2B_CORE_INSTANCE_H_
#define PRIVATE_VDA5050_2B_2B_CORE_INSTANCE_H_

#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <shared_mutex>

#include "vda5050++/config.h"
#include "vda5050++/core/action_event_manager.h"
#include "vda5050++/core/action_status_manager.h"
#include "vda5050++/core/events/control_event.h"
#include "vda5050++/core/events/factsheet_event.h"
#include "vda5050++/core/events/interpreter_event.h"
#include "vda5050++/core/events/message_event.h"
#include "vda5050++/core/events/order_event.h"
#include "vda5050++/core/events/state_event.h"
#include "vda5050++/core/events/validation_event.h"
#include "vda5050++/core/generic_event_manager.h"
#include "vda5050++/core/navigation_event_manager.h"
#include "vda5050++/core/navigation_status_manager.h"
#include "vda5050++/core/query_event_manager.h"
#include "vda5050++/core/state/order_manager.h"
#include "vda5050++/core/state/status_manager.h"
#include "vda5050++/core/status_event_manager.h"
#include "vda5050++/handler/base_action_handler.h"
#include "vda5050++/handler/base_navigation_handler.h"
#include "vda5050++/handler/base_query_handler.h"

namespace vda5050pp::core {

namespace module_keys {
static constexpr std::string_view k_action_event_handler_key = "ActionEventHandler";
static constexpr std::string_view k_factsheet_event_handler_key = "Factsheet";
static constexpr std::string_view k_interpreter_event_handler_key = "InterpreterEventHandler";
static constexpr std::string_view k_navigation_event_handler_key = "NavigationEventHandler";
static constexpr std::string_view k_message_event_handler_key = "MessageEventHandler";
static constexpr std::string_view k_mqtt_key = "Mqtt";
static constexpr std::string_view k_node_reached_handler_key = "NodeReachedHandler";
static constexpr std::string_view k_order_event_handler_key = "OrderEventHandler";
static constexpr std::string_view k_query_event_handler_key = "QueryEventHandler";
static constexpr std::string_view k_state_event_handler_key = "StateEventHandler";
static constexpr std::string_view k_state_update_timer_key = "StateUpdateTimer";
static constexpr std::string_view k_validation_event_handler_key = "ValidationEventHandler";
static constexpr std::string_view k_visualization_timer_key = "VisualizationTimer";
}  // namespace module_keys

class Module;

class Instance {
private:
  static std::shared_ptr<Instance> instance_;
  static std::shared_mutex instance_mutex_;

  vda5050pp::Config config_;

  ActionEventManager action_event_manager_;
  ActionStatusManager action_status_manager_;
  NavigationEventManager navigation_event_manager_;
  NavigationStatusManager navigation_status_manager_;
  StatusEventManager status_event_manager_;
  QueryEventManager query_event_manager_;
  GenericEventManager<vda5050pp::core::events::ControlEvent> control_event_manager_;
  GenericEventManager<vda5050pp::core::events::FactsheetEvent> factsheet_event_manager_;
  GenericEventManager<vda5050pp::core::events::InterpreterEvent> interpreter_event_manager_;
  GenericEventManager<vda5050pp::core::events::MessageEvent> message_event_manager_;
  GenericEventManager<vda5050pp::core::events::OrderEvent> order_event_manager_;
  GenericEventManager<vda5050pp::core::events::StateEvent> state_event_manager_;
  GenericEventManager<vda5050pp::core::events::ValidationEvent> validation_event_manager_;

  static std::map<std::string, std::shared_ptr<vda5050pp::core::Module>, std::less<>> modules_;
  static std::shared_mutex modules_mutex_;

  std::set<std::shared_ptr<vda5050pp::handler::BaseActionHandler>> action_handler_;
  std::shared_ptr<vda5050pp::handler::BaseNavigationHandler> navigation_handler_;
  std::shared_ptr<vda5050pp::handler::BaseQueryHandler> query_handler_;

  vda5050pp::core::state::OrderManager order_manager_;
  vda5050pp::core::state::StatusManager status_manager_;

protected:
  explicit Instance(const vda5050pp::Config &config);

  void initializeComponents();
  void deInitializeComponents();

public:
  virtual ~Instance() = default;

  static std::weak_ptr<Instance> init(const vda5050pp::Config &config) noexcept(true);

  static std::weak_ptr<Instance> get() noexcept(true);

  static Instance &ref() noexcept(false);

  static void reset() noexcept(true);

  static void registerModule(std::string_view key,
                             std::shared_ptr<vda5050pp::core::Module> module_ptr) noexcept(false);

  static void unregisterModule(std::string_view key) noexcept(false);

  static std::weak_ptr<vda5050pp::core::Module> lookupModule(std::string_view key) noexcept(false);
  static std::shared_ptr<vda5050pp::config::ModuleSubConfig> generateConfig(
      std::string_view key) noexcept(false);
  static std::list<std::string_view> registeredModules();

  void start();
  void stop();

  const vda5050pp::Config &getConfig() const;

  ActionEventManager &getActionEventManager() noexcept(true);

  ActionStatusManager &getActionStatusManager() noexcept(true);

  NavigationEventManager &getNavigationEventManager() noexcept(true);

  NavigationStatusManager &getNavigationStatusManager() noexcept(true);

  StatusEventManager &getStatusEventManager() noexcept(true);

  QueryEventManager &getQueryEventManager() noexcept(true);

  GenericEventManager<vda5050pp::core::events::ControlEvent> &getControlEventManager() noexcept(
      true);

  GenericEventManager<vda5050pp::core::events::FactsheetEvent> &getFactsheetEventManager() noexcept(
      true);

  GenericEventManager<vda5050pp::core::events::MessageEvent> &getMessageEventManager() noexcept(
      true);

  GenericEventManager<vda5050pp::core::events::OrderEvent> &getOrderEventManager() noexcept(true);

  GenericEventManager<vda5050pp::core::events::ValidationEvent>
      &getValidationEventManager() noexcept(true);

  GenericEventManager<vda5050pp::core::events::InterpreterEvent>
      &getInterpreterEventManager() noexcept(true);

  GenericEventManager<vda5050pp::core::events::StateEvent> &getStateEventManager() noexcept(true);

  void addActionHandler(
      std::shared_ptr<vda5050pp::handler::BaseActionHandler> action_handler) noexcept(true);

  const std::set<std::shared_ptr<vda5050pp::handler::BaseActionHandler>> &getActionHandler() const
      noexcept(true);

  void setNavigationHandler(
      std::shared_ptr<vda5050pp::handler::BaseNavigationHandler> navigation_handler) noexcept(true);

  std::weak_ptr<vda5050pp::handler::BaseNavigationHandler> getNavigationHandler() const
      noexcept(true);

  void setQueryHandler(
      std::shared_ptr<vda5050pp::handler::BaseQueryHandler> query_handler) noexcept(true);

  std::weak_ptr<vda5050pp::handler::BaseQueryHandler> getQueryHandler() const noexcept(true);

  vda5050pp::core::state::OrderManager &getOrderManager();
  vda5050pp::core::state::StatusManager &getStatusManager();
};

}  // namespace vda5050pp::core

#endif  // PRIVATE_VDA5050_2B_2B_CORE_INSTANCE_H_
