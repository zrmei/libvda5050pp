// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/observer/message_observer.h"

#include "vda5050++/core/instance.h"

using namespace vda5050pp::observer;

struct OpaqueState {
  vda5050pp::core::GenericEventManager<vda5050pp::core::events::MessageEvent>::ScopedSubscriber
      message_event_subscriber;
};

inline OpaqueState &getOpaqueState(vda5050pp::misc::AnyPtr &opaque_state) {
  return *opaque_state.get<OpaqueState>();
}

MessageObserver::MessageObserver() {
  auto opaque_state = std::make_shared<OpaqueState>(
      OpaqueState{vda5050pp::core::Instance::ref().getMessageEventManager().getScopedSubscriber()});

  opaque_state->message_event_subscriber.subscribe<vda5050pp::core::events::ValidOrderMessageEvent>(
      [this](std::shared_ptr<vda5050pp::core::events::ValidOrderMessageEvent>) {
        this->received_order_messages_++;
      });
  opaque_state->message_event_subscriber
      .subscribe<vda5050pp::core::events::ValidInstantActionMessageEvent>(
          [this](std::shared_ptr<vda5050pp::core::events::ValidInstantActionMessageEvent>) {
            this->received_instant_actions_messages_++;
          });
  opaque_state->message_event_subscriber
      .subscribe<vda5050pp::core::events::SendFactsheetMessageEvent>(
          [this](std::shared_ptr<vda5050pp::core::events::SendFactsheetMessageEvent>) {
            this->sent_factsheet_messages_++;
          });
  opaque_state->message_event_subscriber.subscribe<vda5050pp::core::events::SendStateMessageEvent>(
      [this](std::shared_ptr<vda5050pp::core::events::SendStateMessageEvent>) {
        this->sent_state_messages_++;
      });
  opaque_state->message_event_subscriber
      .subscribe<vda5050pp::core::events::SendVisualizationMessageEvent>(
          [this](std::shared_ptr<vda5050pp::core::events::SendVisualizationMessageEvent>) {
            this->sent_visualization_messages_++;
          });
  opaque_state->message_event_subscriber.subscribe<vda5050pp::core::events::ConnectionChangedEvent>(
      [this](std::shared_ptr<vda5050pp::core::events::ConnectionChangedEvent> event) {
        std::unique_lock lock(this->connection_status_mutex_);
        this->connection_status_ = event->status;
      });
  opaque_state->message_event_subscriber.subscribe<vda5050pp::core::events::MessageErrorEvent>(
      [this](std::shared_ptr<vda5050pp::core::events::MessageErrorEvent> event) {
        this->errors_++;
        std::unique_lock lock(this->last_error_message_mutex_);
        this->last_error_message_ = std::make_pair(event->error_type, event->description);
      });

  this->opaque_state_ = opaque_state;
}

uint32_t MessageObserver::getReceivedOrderMessages() const {
  return this->received_order_messages_;
}

uint32_t MessageObserver::getReceivedInstantActionsMessages() const {
  return this->received_instant_actions_messages_;
}

uint32_t MessageObserver::getSentFactsheetMessages() const {
  return this->sent_factsheet_messages_;
}

uint32_t MessageObserver::getSentStateMessages() const { return this->sent_state_messages_; }

uint32_t MessageObserver::getSentVisualizationMessages() const {
  return this->sent_visualization_messages_;
}

uint32_t MessageObserver::getErrors() const { return this->errors_; }

std::optional<std::pair<vda5050pp::misc::MessageErrorType, std::string>>
MessageObserver::getLastError() const {
  std::shared_lock lock(this->last_error_message_mutex_);
  return this->last_error_message_;
}

std::optional<vda5050pp::misc::ConnectionStatus> MessageObserver::getConnectionStatus() const {
  std::shared_lock lock(this->connection_status_mutex_);
  return this->connection_status_;
}

void MessageObserver::onValidOrderMessage(
    std::function<void(std::shared_ptr<const vda5050::Order>)> callback) {
  getOpaqueState(this->opaque_state_)
      .message_event_subscriber.subscribe<vda5050pp::core::events::ValidOrderMessageEvent>(
          [callback](std::shared_ptr<vda5050pp::core::events::ValidOrderMessageEvent> event) {
            callback(event->valid_order);
          });
}

void MessageObserver::onValidInstantActionsMessage(
    std::function<void(std::shared_ptr<const vda5050::InstantActions>)> callback) {
  getOpaqueState(this->opaque_state_)
      .message_event_subscriber.subscribe<vda5050pp::core::events::ValidInstantActionMessageEvent>(
          [callback](
              std::shared_ptr<vda5050pp::core::events::ValidInstantActionMessageEvent> event) {
            callback(event->valid_instant_actions);
          });
}

void MessageObserver::onSentFactsheetMessage(
    std::function<void(std::shared_ptr<const vda5050::AgvFactsheet>)> callback) {
  getOpaqueState(this->opaque_state_)
      .message_event_subscriber.subscribe<vda5050pp::core::events::SendFactsheetMessageEvent>(
          [callback](std::shared_ptr<vda5050pp::core::events::SendFactsheetMessageEvent> event) {
            callback(event->factsheet);
          });
}

void MessageObserver::onSentStateMessage(
    std::function<void(std::shared_ptr<const vda5050::State>)> callback) {
  getOpaqueState(this->opaque_state_)
      .message_event_subscriber.subscribe<vda5050pp::core::events::SendStateMessageEvent>(
          [callback](std::shared_ptr<vda5050pp::core::events::SendStateMessageEvent> event) {
            callback(event->state);
          });
}

void MessageObserver::onSentVisualizationMessage(
    std::function<void(std::shared_ptr<const vda5050::Visualization>)> callback) {
  getOpaqueState(this->opaque_state_)
      .message_event_subscriber.subscribe<vda5050pp::core::events::SendVisualizationMessageEvent>(
          [callback](
              std::shared_ptr<vda5050pp::core::events::SendVisualizationMessageEvent> event) {
            callback(event->visualization);
          });
}

void MessageObserver::onConnectionChanged(
    std::function<void(vda5050pp::misc::ConnectionStatus)> callback) {
  getOpaqueState(this->opaque_state_)
      .message_event_subscriber.subscribe<vda5050pp::core::events::ConnectionChangedEvent>(
          [callback](std::shared_ptr<vda5050pp::core::events::ConnectionChangedEvent> event) {
            callback(event->status);
          });
}

void MessageObserver::onMessageError(
    std::function<void(std::pair<vda5050pp::misc::MessageErrorType, std::string_view>)> callback) {
  getOpaqueState(this->opaque_state_)
      .message_event_subscriber.subscribe<vda5050pp::core::events::MessageErrorEvent>(
          [callback](std::shared_ptr<vda5050pp::core::events::MessageErrorEvent> event) {
            callback(std::make_pair(event->error_type, event->description));
          });
}