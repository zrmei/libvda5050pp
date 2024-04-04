//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/messages/mqtt_module.h"

#include <spdlog/fmt/fmt.h>
#include <vda5050/Connection.h>
#include <vda5050/InstantActions.h>
#include <vda5050/Order.h>
#include <vda5050/State.h>
#include <vda5050/Visualization.h>

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/events/message_event.h"
#include "vda5050++/core/logger.h"
#include "vda5050++/version.h"

using namespace vda5050pp::core::messages;
using namespace std::chrono_literals;

void MqttModule::fillHeaderConnection(vda5050::HeaderVDA5050 &header) {
  header.headerId = this->connection_seq_id_++;
  header.manufacturer = this->manufacturer_;
  header.serialNumber = this->serial_number_;
  header.version = version::getCurrentVersion();
  header.timestamp = std::chrono::system_clock::now();
}
void MqttModule::fillHeaderFactsheet(vda5050::HeaderVDA5050 &header) {
  header.headerId = this->factsheet_seq_id_++;
  header.manufacturer = this->manufacturer_;
  header.serialNumber = this->serial_number_;
  header.version = version::getCurrentVersion();
  header.timestamp = std::chrono::system_clock::now();
}
void MqttModule::fillHeaderState(vda5050::HeaderVDA5050 &header) {
  header.headerId = this->state_seq_id_++;
  header.manufacturer = this->manufacturer_;
  header.serialNumber = this->serial_number_;
  header.version = version::getCurrentVersion();
  header.timestamp = std::chrono::system_clock::now();
}
void MqttModule::fillHeaderVisualization(vda5050::HeaderVDA5050 &header) {
  header.headerId = this->visualization_seq_id_++;
  header.manufacturer = this->manufacturer_;
  header.serialNumber = this->serial_number_;
  header.version = version::getCurrentVersion();
  header.timestamp = std::chrono::system_clock::now();
}

mqtt::will_options MqttModule::getWill() {
  vda5050::Connection connection;
  connection.connectionState = vda5050::ConnectionState::CONNECTIONBROKEN;
  this->fillHeaderConnection(connection.header);

  vda5050::json j = connection;

  mqtt::will_options will;
  will.set_topic(this->connection_topic_);
  will.set_retained(true);
  will.set_qos(this->k_qos);
  will.set_payload(j.dump());

  return will;
}

void MqttModule::useMqttOptions(const vda5050pp::config::MqttOptions &opts) {
  this->server_ = opts.server;

  this->connect_opts_ = mqtt::connect_options();
  this->connect_opts_.set_mqtt_version(4);
  this->connect_opts_.set_clean_session(false);
  this->connect_opts_.set_user_name(opts.username.value_or(""));
  this->connect_opts_.set_password(opts.password.value_or(""));
  this->connect_opts_.set_automatic_reconnect(true);
  this->connect_opts_.set_automatic_reconnect(opts.min_retry_interval_.value_or(2s),
                                              opts.max_retry_interval_.value_or(16s));

  if (opts.keep_alive_interval_) {
    this->connect_opts_.set_keep_alive_interval(*opts.keep_alive_interval_);
  }
  if (opts.connect_timeout_) {
    this->connect_opts_.set_connect_timeout(*opts.connect_timeout_);
  }

  if (opts.use_ssl) {
    mqtt::ssl_options ssl;
    ssl.set_verify(opts.enable_cert_check);
    mqtt::ssl_options::error_handler handler = [this](const std::string &msg) {
      getMqttLogger()->error("MqttModule (ssl_error): {}", msg);
      this->state_ = State::k_error;
    };
    ssl.set_error_handler(handler);
    ssl.set_enable_server_cert_auth(opts.enable_cert_check);
    this->connect_opts_.set_ssl(ssl);
  }

  auto prefix = fmt::format("{}/{}", opts.interface, opts.version_overwrite.value_or("v2"));

  this->order_topic_ = prefix;
  this->connection_topic_ = prefix;
  this->factsheet_topic_ = prefix;
  this->visualization_topic_ = prefix;
  this->instant_actions_topic_ = prefix;
  this->state_topic_ = prefix;
}

void MqttModule::on_failure(const mqtt::token &tkn) {
  auto evt = std::make_shared<vda5050pp::core::events::MessageErrorEvent>();
  evt->error_type = vda5050pp::misc::MessageErrorType::k_delivery;
  evt->description = fmt::format("Could not deliver message (id={})", tkn.get_message_id());

  getMqttLogger()->warn(evt->description);

  vda5050pp::core::Instance::ref().getMessageEventManager().dispatch(evt);
}

void MqttModule::on_success(const mqtt::token &) {
  // We do not care about successful deliveries
}

void MqttModule::connected(const std::string & /*cause*/) {
  this->mqtt_client_->subscribe(this->order_topic_, this->k_qos);
  this->mqtt_client_->subscribe(this->instant_actions_topic_, this->k_qos);

  getMqttLogger()->info("MqttModule: online");
  this->state_ = State::k_online;
  auto evt = std::make_shared<vda5050pp::core::events::ConnectionChangedEvent>();
  evt->status = vda5050pp::misc::ConnectionStatus::k_online;
  vda5050pp::core::Instance::ref().getMessageEventManager().dispatch(evt);

  vda5050::Connection connection;
  connection.connectionState = vda5050::ConnectionState::ONLINE;
  this->fillHeaderConnection(connection.header);
  this->sendConnection(connection);
}

void MqttModule::connection_lost(const std::string &cause) {
  getMqttLogger()->warn("MqttModule: connection lost ({})", cause);
  if (this->state_ == State::k_online) {
    auto evt = std::make_shared<vda5050pp::core::events::ConnectionChangedEvent>();
    evt->status = vda5050pp::misc::ConnectionStatus::k_offline;
    vda5050pp::core::Instance::ref().getMessageEventManager().dispatch(evt);
  }
  this->state_ = State::k_offline;
}

void MqttModule::message_arrived(mqtt::const_message_ptr msg) {
  std::shared_ptr<vda5050pp::core::events::MessageEvent> event;

  try {
    auto j = vda5050::json::parse(msg->get_payload());

    if (msg->get_topic() == this->order_topic_) {
      auto order_event = std::make_shared<vda5050pp::core::events::ReceiveOrderMessageEvent>();
      order_event->order = std::make_shared<vda5050::Order>(j);
      getMqttLogger()->debug("Received Order (headerId={})", order_event->order->header.headerId);
      event = order_event;
    } else if (msg->get_topic() == this->instant_actions_topic_) {
      auto ia_event = std::make_shared<vda5050pp::core::events::ReceiveInstantActionMessageEvent>();
      ia_event->instant_actions = std::make_shared<vda5050::InstantActions>(j);
      getMqttLogger()->debug("Received InstantActions (headerId={})",
                             ia_event->instant_actions->header.headerId);
      event = ia_event;
    } else {
      getMqttLogger()->warn("MqttModule received a message on an unknown topic \"{}\"",
                            msg->get_topic());
    }
  } catch (const vda5050::json::exception &e) {
    auto error_event = std::make_shared<vda5050pp::core::events::MessageErrorEvent>();
    error_event->error_type = vda5050pp::misc::MessageErrorType::k_json_deserialization;
    error_event->description = fmt::format(
        "MqttModule could not parse message on topic \"{}\" ({})", msg->get_topic(), e.what());

    getMqttLogger()->warn(error_event->description);
    event = error_event;
  }

  Instance::ref().getMessageEventManager().dispatch(event);
}

void MqttModule::delivery_complete(mqtt::delivery_token_ptr /*tok*/) {
  // We do not care about complete deliveries
}

void MqttModule::connect() {
  switch (this->state_) {
    case State::k_constructed:
      throw VDA5050PPMqttError(MK_EX_CONTEXT("MqttModule not initialized"));
    case State::k_online:
      throw VDA5050PPMqttError(MK_EX_CONTEXT("MqttModule already online"));
    case State::k_error:
      throw VDA5050PPMqttError(MK_EX_CONTEXT("MqttModule is in an error state"));
    case State::k_initialized:
      [[fallthrough]];
    case State::k_offline:
      getMqttLogger()->info("MqttModule: connecting");
      this->mqtt_client_->connect(this->connect_opts_, nullptr, *this);
      break;
    default:
      throw VDA5050PPMqttError(MK_EX_CONTEXT("MqttModule is in an unknown state"));
  }
}

void MqttModule::disconnect() {
  switch (this->state_) {
    case State::k_constructed:
      throw VDA5050PPMqttError(MK_EX_CONTEXT("MqttModule not initialized"));
    case State::k_initialized:
      throw VDA5050PPMqttError(MK_EX_CONTEXT("MqttModule not online"));
    case State::k_offline:
      throw VDA5050PPMqttError(MK_EX_CONTEXT("MqttModule already offline"));
    case State::k_error:
      throw VDA5050PPMqttError(MK_EX_CONTEXT("MqttModule is in an error state"));
    case State::k_online: {
      getMqttLogger()->info("MqttModule: disconnecting");
      vda5050::Connection connection;
      connection.connectionState = vda5050::ConnectionState::OFFLINE;
      this->fillHeaderConnection(connection.header);
      this->sendConnection(connection);
      this->mqtt_client_->disconnect()->wait();
      this->state_ = State::k_offline;
      auto evt = std::make_shared<vda5050pp::core::events::ConnectionChangedEvent>();
      evt->status = vda5050pp::misc::ConnectionStatus::k_offline;
      vda5050pp::core::Instance::ref().getMessageEventManager().dispatch(evt);
    } break;
    default:
      throw VDA5050PPMqttError(MK_EX_CONTEXT("MqttModule is in an unknown state"));
  }
}

void MqttModule::sendState(const vda5050::State &state) const {
  if (this->state_ != State::k_online) {
    throw vda5050pp::VDA5050PPMqttError(MK_EX_CONTEXT("MqttModule is not online."));
  }

  getMqttLogger()->debug("sendState(headerId={})", state.header.headerId);

  vda5050::json j = state;

  auto msg = std::make_shared<mqtt::message>();
  msg->set_topic(this->state_topic_);
  msg->set_payload(j.dump());
  msg->set_qos(this->k_qos);
  this->mqtt_client_->publish(msg);
}
void MqttModule::sendVisualization(const vda5050::Visualization &visualization) const {
  if (this->state_ != State::k_online) {
    throw vda5050pp::VDA5050PPMqttError(MK_EX_CONTEXT("MqttModule is not online."));
  }

  getMqttLogger()->debug("sendVisualization(headerId={})", visualization.header.headerId);

  vda5050::json j = visualization;

  auto msg = std::make_shared<mqtt::message>();
  msg->set_topic(this->visualization_topic_);
  msg->set_payload(j.dump());
  msg->set_qos(this->k_qos);
  this->mqtt_client_->publish(msg);
}

void MqttModule::sendConnection(const vda5050::Connection &connection) const {
  if (this->state_ != State::k_online) {
    throw vda5050pp::VDA5050PPMqttError(MK_EX_CONTEXT("MqttModule is not online."));
  }

  getMqttLogger()->debug("sendConnection(headerId={})", connection.header.headerId);

  vda5050::json j = connection;

  auto msg = std::make_shared<mqtt::message>();
  msg->set_topic(this->connection_topic_);
  msg->set_payload(j.dump());
  msg->set_qos(this->k_qos);
  msg->set_retained(true);
  this->mqtt_client_->publish(msg);
}

void MqttModule::sendFactsheet(const vda5050::AgvFactsheet &factsheet) const {
  if (this->state_ != State::k_online) {
    throw vda5050pp::VDA5050PPMqttError(MK_EX_CONTEXT("MqttModule is not online."));
  }

  getMqttLogger()->debug("sendFactsheet(headerId={})", factsheet.header.headerId);

  vda5050::json j = factsheet;

  auto msg = std::make_shared<mqtt::message>();
  msg->set_topic(this->factsheet_topic_);
  msg->set_payload(j.dump());
  msg->set_qos(this->k_qos);
  msg->set_retained(true);
  this->mqtt_client_->publish(msg);
}

void MqttModule::initialize(vda5050pp::core::Instance &instance) {
  const auto &desc = instance.getConfig().getAgvDescription();

  this->connection_seq_id_ = 0;
  this->factsheet_seq_id_ = 0;
  this->state_seq_id_ = 0;
  this->visualization_seq_id_ = 0;

  this->useMqttOptions(instance.getConfig().getMqttSubConfig().getOptions());
  this->manufacturer_ = desc.manufacturer;
  this->serial_number_ = desc.serial_number;

  auto fill_topic = [&desc](auto &str, auto &topic) {
    str = fmt::format("{}/{}/{}/{}", str, desc.manufacturer, desc.serial_number, topic);
  };

  fill_topic(this->connection_topic_, "connection");
  fill_topic(this->factsheet_topic_, "factsheet");
  fill_topic(this->instant_actions_topic_, "instantActions");
  fill_topic(this->order_topic_, "order");
  fill_topic(this->state_topic_, "state");
  fill_topic(this->visualization_topic_, "visualization");

  this->connect_opts_.set_will(this->getWill());

  auto client_id = fmt::format("libvda5050++(agv_id={})", desc.agv_id);
  this->mqtt_client_ = std::make_unique<mqtt::async_client>(this->server_, client_id);
  this->mqtt_client_->set_callback(*this);

  this->m_subscriber_ = instance.getMessageEventManager().getScopedSubscriber();
  this->m_subscriber_->subscribe<vda5050pp::core::events::SendFactsheetMessageEvent>(
      [this](std::shared_ptr<vda5050pp::core::events::SendFactsheetMessageEvent> evt_ptr) {
        if (evt_ptr == nullptr || evt_ptr->factsheet == nullptr) {
          throw VDA5050PPInvalidEventData(MK_EX_CONTEXT("SendFactsheet event is empty"));
        }
        this->fillHeaderFactsheet(evt_ptr->factsheet->header);
        try {
          this->sendFactsheet(*evt_ptr->factsheet);
        } catch (vda5050pp::VDA5050PPError &e) {
          getMessagesLogger()->warn("Could not send Factsheet: {}", e);
        }
      });
  this->m_subscriber_->subscribe<vda5050pp::core::events::SendStateMessageEvent>(
      [this](std::shared_ptr<vda5050pp::core::events::SendStateMessageEvent> evt_ptr) {
        if (evt_ptr == nullptr || evt_ptr->state == nullptr) {
          throw VDA5050PPInvalidEventData(MK_EX_CONTEXT("SendState event is empty"));
        }
        this->fillHeaderState(evt_ptr->state->header);
        try {
          this->sendState(*evt_ptr->state);
        } catch (vda5050pp::VDA5050PPError &e) {
          getMessagesLogger()->warn("Could not send State: {}", e);
        }
      });
  this->m_subscriber_->subscribe<vda5050pp::core::events::SendVisualizationMessageEvent>(
      [this](std::shared_ptr<vda5050pp::core::events::SendVisualizationMessageEvent> evt_ptr) {
        if (evt_ptr == nullptr || evt_ptr->visualization == nullptr) {
          throw VDA5050PPInvalidEventData(MK_EX_CONTEXT("SendVisualization event is empty"));
        }
        this->fillHeaderVisualization(evt_ptr->visualization->header);
        try {
          this->sendVisualization(*evt_ptr->visualization);
        } catch (vda5050pp::VDA5050PPError &e) {
          getMessagesLogger()->warn("Could not send Visualization: {}", e);
        }
      });
  this->c_subscriber_ = instance.getControlEventManager().getScopedSubscriber();
  this->c_subscriber_->subscribe<vda5050pp::core::events::ControlMessagesEvent>(
      [this](std::shared_ptr<vda5050pp::core::events::ControlMessagesEvent> evt_ptr) {
        if (evt_ptr == nullptr) {
          throw VDA5050PPInvalidEventData(MK_EX_CONTEXT("ControlMessages event is empty"));
        }
        if (evt_ptr->type == vda5050pp::core::events::ControlMessagesEvent::Type::k_connect) {
          this->connect();
        } else {
          this->disconnect();
        }
        evt_ptr->acquireResultToken().setValue();
      });

  this->state_ = State::k_initialized;
}

void MqttModule::deinitialize(vda5050pp::core::Instance &) {
  this->m_subscriber_.reset();
  this->c_subscriber_.reset();
  this->mqtt_client_.reset();
  this->state_ = State::k_constructed;
}

std::string_view MqttModule::describe() const { return "MqttModule"; }

std::shared_ptr<vda5050pp::config::ModuleSubConfig> MqttModule::generateSubConfig() const {
  return std::make_shared<vda5050pp::config::MqttSubConfig>();
}