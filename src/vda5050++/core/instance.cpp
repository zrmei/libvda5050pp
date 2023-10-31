//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/core/instance.h"

#include <spdlog/sinks/stdout_color_sinks.h>

#include <future>

#include "vda5050++/core/agv_handler/action_event_handler.h"
#include "vda5050++/core/agv_handler/navigation_event_handler.h"
#include "vda5050++/core/agv_handler/node_reached_handler.h"
#include "vda5050++/core/agv_handler/query_event_handler.h"
#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/factsheet/factsheet_event_handler.h"
#include "vda5050++/core/interpreter/interpreter_event_handler.h"
#include "vda5050++/core/logger.h"
#include "vda5050++/core/messages/message_event_handler.h"
#include "vda5050++/core/messages/mqtt_module.h"
#include "vda5050++/core/module.h"
#include "vda5050++/core/order/order_event_handler.h"
#include "vda5050++/core/state/state_event_handler.h"
#include "vda5050++/core/state/state_update_timer.h"
#include "vda5050++/core/state/visualization_timer.h"
#include "vda5050++/core/validation/validation_event_handler.h"

using namespace vda5050pp::core;
using namespace std::chrono_literals;

std::shared_mutex Instance::instance_mutex_;
std::shared_ptr<Instance> Instance::instance_;

std::shared_mutex Instance::modules_mutex_;
std::map<std::string, std::shared_ptr<vda5050pp::core::Module>, std::less<>> Instance::modules_;

static const vda5050pp::core::AutoRegisterModule<vda5050pp::core::agv_handler::ActionEventHandler,
                                                 module_keys::k_action_event_handler_key>
    _auto_register_action_event_handler;

static const vda5050pp::core::AutoRegisterModule<
    vda5050pp::core::agv_handler::NavigationEventHandler,
    module_keys::k_navigation_event_handler_key>
    _auto_register_navigation_event_handler;

static const vda5050pp::core::AutoRegisterModule<vda5050pp::core::messages::MessageEventHandler,
                                                 module_keys::k_message_event_handler_key>
    _auto_register_message_event_handler;

static const vda5050pp::core::AutoRegisterModule<vda5050pp::core::messages::MqttModule,
                                                 module_keys::k_mqtt_key>
    _auto_register_mqtt;

static const vda5050pp::core::AutoRegisterModule<
    vda5050pp::core::interpreter::InterpreterEventHandler,
    module_keys::k_interpreter_event_handler_key>
    _auto_register_interpreter_event_handler;

static const vda5050pp::core::AutoRegisterModule<vda5050pp::core::order::OrderEventHandler,
                                                 module_keys::k_order_event_handler_key>
    _auto_register_order_event_handler;

static const vda5050pp::core::AutoRegisterModule<vda5050pp::core::agv_handler::NodeReachedHandler,
                                                 module_keys::k_node_reached_handler_key>
    _auto_register_node_reached_handler;

static const vda5050pp::core::AutoRegisterModule<vda5050pp::core::agv_handler::QueryEventHandler,
                                                 module_keys::k_query_event_handler_key>
    _auto_register_query_event_handler;

static const vda5050pp::core::AutoRegisterModule<vda5050pp::core::state::StateUpdateTimer,
                                                 module_keys::k_state_update_timer_key>
    _auto_register_state_update_timer;

static const vda5050pp::core::AutoRegisterModule<vda5050pp::core::state::StateEventHandler,
                                                 module_keys::k_state_event_handler_key>
    _auto_register_state_event_handler;

static const vda5050pp::core::AutoRegisterModule<
    vda5050pp::core::validation::ValidationEventHandler,
    module_keys::k_validation_event_handler_key>
    _auto_register_validation_event_handler;

static const vda5050pp::core::AutoRegisterModule<vda5050pp::core::state::VisualizationTimer,
                                                 module_keys::k_visualization_timer_key>
    _auto_register_visualization_timer;

static const vda5050pp::core::AutoRegisterModule<vda5050pp::core::factsheet::FactsheetEventHandler,
                                                 module_keys::k_factsheet_event_handler_key>
    _auto_register_factsheet_event_handler;

constexpr spdlog::level::level_enum convertLogLevel(vda5050pp::config::LogLevel level) {
  switch (level) {
    case vda5050pp::config::LogLevel::k_debug:
      return spdlog::level::debug;
    case vda5050pp::config::LogLevel::k_info:
      return spdlog::level::info;
    case vda5050pp::config::LogLevel::k_warn:
      return spdlog::level::warn;
    case vda5050pp::config::LogLevel::k_error:
      return spdlog::level::err;
    case vda5050pp::config::LogLevel::k_off:
      return spdlog::level::off;
    default:
      return spdlog::level::info;
  }
}

void Instance::initializeComponents() {
  const auto &config = this->getConfig();

  // Setup loggers
  // TODO: add file support
  spdlog::set_automatic_registration(true);
  auto global_level = config.getGlobalConfig().getLogLevel().value_or(config::LogLevel::k_info);
  for (const auto &key : Instance::registeredModules()) {
    auto cfg = config.lookupModuleConfig(key);
    auto logger =
        recreateLogger<spdlog::sinks::stdout_color_sink_mt>(logger_names::remapModule(key));
    logger->set_level(convertLogLevel(cfg->getLogLevel().value_or(global_level)));
  }
  recreateLogger<spdlog::sinks::stdout_color_sink_mt>(logger_names::k_instance)
      ->set_level(convertLogLevel(global_level));
  recreateLogger<spdlog::sinks::stdout_color_sink_mt>(logger_names::k_events)
      ->set_level(convertLogLevel(global_level));

  // Each one relies on Instance member construction
  for (const auto &[name, module_ptr] : Instance::modules_) {
    if (module_ptr == nullptr) {
      getInstanceLogger()->error("Module {} is nullptr, cannot initialize", name);
    } else if (this->config_.getGlobalConfig().isListedModule(name)) {
      getInstanceLogger()->info("Initializing {}", name);
      module_ptr->initialize(*this);
    } else {
      getInstanceLogger()->info("Module {} is disabled by black/white list", name);
    }
  }
}

void Instance::deInitializeComponents() {
  // When global dtors are called, this might yield a nullptr
  auto logger = getInstanceLogger(false);

  // Each one relies on live members
  for (const auto &[name, module_ptr] : Instance::modules_) {
    if (module_ptr == nullptr) {
      if (logger) {
        logger->error("Module {} is nullptr, cannot de-initialize", name);
      }
    } else {
      if (logger) {
        logger->info("De-initializing {}", name);
      }
      module_ptr->deinitialize(*this);
    }
  }
}

Instance::Instance(const vda5050pp::Config &config)
    : config_(config),
      action_event_manager_(config_.getGlobalConfig().getEventManagerOptions()),
      action_status_manager_(config_.getGlobalConfig().getEventManagerOptions()),
      navigation_event_manager_(config_.getGlobalConfig().getEventManagerOptions()),
      navigation_status_manager_(config_.getGlobalConfig().getEventManagerOptions()),
      status_event_manager_(config_.getGlobalConfig().getEventManagerOptions()),
      query_event_manager_(config_.getGlobalConfig().getEventManagerOptions()),
      control_event_manager_(config_.getGlobalConfig().getEventManagerOptions()),
      factsheet_event_manager_(config_.getGlobalConfig().getEventManagerOptions()),
      interpreter_event_manager_(config_.getGlobalConfig().getEventManagerOptions()),
      message_event_manager_(config_.getGlobalConfig().getEventManagerOptions()),
      order_event_manager_(config_.getGlobalConfig().getEventManagerOptions()),
      state_event_manager_(config_.getGlobalConfig().getEventManagerOptions()),
      validation_event_manager_(config_.getGlobalConfig().getEventManagerOptions()) {}

std::weak_ptr<Instance> Instance::init(const vda5050pp::Config &config) noexcept(true) {
  class InstanceConstructible final : public Instance {
  public:
    explicit InstanceConstructible(const vda5050pp::Config &config) noexcept(true)
        : Instance(config){};
    ~InstanceConstructible() override = default;
  };

  std::unique_lock i_lock(Instance::instance_mutex_);

  Instance::instance_ = std::make_shared<InstanceConstructible>(config);
  Instance::instance_->initializeComponents();
  return Instance::instance_;
}

void Instance::reset() noexcept(true) {
  std::unique_lock i_lock(Instance::instance_mutex_);
  if (Instance::instance_) {
    Instance::instance_->deInitializeComponents();
  }
  Instance::instance_.reset();
}

std::weak_ptr<Instance> Instance::get() noexcept(true) { return Instance::instance_; }

Instance &Instance::ref() noexcept(false) {
  auto ptr = Instance::get().lock();

  if (ptr == nullptr) {
    throw vda5050pp::VDA5050PPNotInitialized(MK_FN_EX_CONTEXT(""));
  }

  return *ptr;
}

void Instance::registerModule(std::string_view key,
                              std::shared_ptr<vda5050pp::core::Module> module_ptr) noexcept(false) {
  std::shared_lock i_lock(Instance::instance_mutex_);
  std::unique_lock m_lock(Instance::modules_mutex_);

  if (module_ptr == nullptr) {
    throw vda5050pp::VDA5050PPNullPointer(
        MK_FN_EX_CONTEXT("Module pointer is not allowed to be nullptr"));
  }

  // Add module
  if (const auto &[_, ok] = Instance::modules_.try_emplace(std::string(key), module_ptr); !ok) {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_FN_EX_CONTEXT(fmt::format("Module with Key {} already registered.", key)));
  }

  // Check if initialization is required
  if (Instance::instance_ != nullptr) {
    if (Instance::instance_->getConfig().getGlobalConfig().isListedModule(key)) {
      getInstanceLogger()->info("Initializing {} during runtime", key);
      module_ptr->initialize(Instance::ref());
    } else {
      getInstanceLogger()->info("Module {} is disabled by black/white list", key);
    }
  }
}

void Instance::unregisterModule(std::string_view key) noexcept(false) {
  std::shared_lock i_lock(Instance::instance_mutex_);
  std::unique_lock m_lock(Instance::modules_mutex_);

  // Remove module
  auto it = Instance::modules_.find(key);
  if (it == Instance::modules_.end()) {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_FN_EX_CONTEXT(fmt::format("Module with Key {} is not registered.", key)));
  }
  auto m = it->second;
  Instance::modules_.erase(it);

  // Check if deinitialization is required
  if (Instance::instance_ != nullptr && m != nullptr) {
    m->deinitialize(Instance::ref());
  }
}

std::weak_ptr<vda5050pp::core::Module> Instance::lookupModule(std::string_view key) noexcept(
    false) {
  std::shared_lock m_lock(Instance::modules_mutex_);

  auto it = Instance::modules_.find(key);
  if (it == Instance::modules_.end()) {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_FN_EX_CONTEXT(fmt::format("Module with Key {} is not registered.", key)));
  }

  return it->second;
}

std::shared_ptr<vda5050pp::config::ModuleSubConfig> Instance::generateConfig(
    std::string_view key) noexcept(false) {
  return Instance::lookupModule(key).lock()->generateSubConfig();
}

std::list<std::string_view> Instance::registeredModules() {
  std::shared_lock m_lock(Instance::modules_mutex_);

  std::list<std::string_view> ret;

  for (const auto &[key, _] : Instance::modules_) {
    ret.push_back(key);
  }

  return ret;
}

void Instance::start() {
  // Go online
  auto evt = std::make_shared<vda5050pp::core::events::ControlMessagesEvent>();
  evt->type = vda5050pp::core::events::ControlMessagesEvent::Type::k_connect;
  this->getControlEventManager().dispatch(evt);

  // Send state
  auto evt_state = std::make_shared<vda5050pp::core::events::RequestStateUpdateEvent>();
  evt_state->urgency = state::StateUpdateUrgency::immediate();
  this->getStateEventManager().dispatch(evt_state);
}

void Instance::stop() {
  // Go offline
  auto evt = std::make_shared<vda5050pp::core::events::ControlMessagesEvent>();
  evt->type = vda5050pp::core::events::ControlMessagesEvent::Type::k_disconnect;
  auto future = evt->getFuture();
  this->getControlEventManager().dispatch(evt);

  if (auto fs = future.wait_for(1s); fs == std::future_status::timeout) {
    getInstanceLogger()->info("stop(): disconnect timed out");
  }
}

const vda5050pp::Config &Instance::getConfig() const { return this->config_; }

ActionEventManager &Instance::getActionEventManager() noexcept(true) {
  return this->action_event_manager_;
}

ActionStatusManager &Instance::getActionStatusManager() noexcept(true) {
  return this->action_status_manager_;
}

NavigationEventManager &Instance::getNavigationEventManager() noexcept(true) {
  return this->navigation_event_manager_;
}

NavigationStatusManager &Instance::getNavigationStatusManager() noexcept(true) {
  return this->navigation_status_manager_;
}

StatusEventManager &Instance::getStatusEventManager() noexcept(true) {
  return this->status_event_manager_;
}

QueryEventManager &Instance::getQueryEventManager() noexcept(true) {
  return this->query_event_manager_;
}

GenericEventManager<vda5050pp::core::events::ControlEvent>
    &Instance::getControlEventManager() noexcept(true) {
  return this->control_event_manager_;
}

GenericEventManager<vda5050pp::core::events::FactsheetEvent>
    &Instance::getFactsheetEventManager() noexcept(true) {
  return this->factsheet_event_manager_;
}

GenericEventManager<vda5050pp::core::events::MessageEvent>
    &Instance::getMessageEventManager() noexcept(true) {
  return this->message_event_manager_;
}

GenericEventManager<vda5050pp::core::events::OrderEvent> &Instance::getOrderEventManager() noexcept(
    true) {
  return this->order_event_manager_;
}

GenericEventManager<vda5050pp::core::events::ValidationEvent>
    &Instance::getValidationEventManager() noexcept(true) {
  return this->validation_event_manager_;
}

GenericEventManager<vda5050pp::core::events::InterpreterEvent>
    &Instance::getInterpreterEventManager() noexcept(true) {
  return this->interpreter_event_manager_;
}

GenericEventManager<vda5050pp::core::events::StateEvent> &Instance::getStateEventManager() noexcept(
    true) {
  return this->state_event_manager_;
}

void Instance::addActionHandler(
    std::shared_ptr<vda5050pp::handler::BaseActionHandler> action_handler) noexcept(true) {
  if (action_handler != nullptr) {
    this->action_handler_.insert(action_handler);
  }
}

const std::set<std::shared_ptr<vda5050pp::handler::BaseActionHandler>> &Instance::getActionHandler()
    const noexcept(true) {
  return this->action_handler_;
}

void Instance::setNavigationHandler(
    std::shared_ptr<vda5050pp::handler::BaseNavigationHandler> navigation_handler) noexcept(true) {
  this->navigation_handler_ = navigation_handler;
}

std::weak_ptr<vda5050pp::handler::BaseNavigationHandler> Instance::getNavigationHandler() const
    noexcept(true) {
  return this->navigation_handler_;
}

void Instance::setQueryHandler(
    std::shared_ptr<vda5050pp::handler::BaseQueryHandler> query_handler) noexcept(true) {
  this->query_handler_ = query_handler;
}

std::weak_ptr<vda5050pp::handler::BaseQueryHandler> Instance::getQueryHandler() const
    noexcept(true) {
  return this->query_handler_;
}

vda5050pp::core::state::OrderManager &Instance::getOrderManager() { return this->order_manager_; }

vda5050pp::core::state::StatusManager &Instance::getStatusManager() {
  return this->status_manager_;
}