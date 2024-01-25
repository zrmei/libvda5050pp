// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/observer/order_observer.h"

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/events/order_event.h"
#include "vda5050++/core/instance.h"

using namespace vda5050pp::observer;

///
///\brief This class contains private dependencies of the library and must be stored with type
/// vanishing.
/// Otherwise this would cludder the public interface of the library.
///
struct OpaqueState {
  vda5050pp::core::GenericEventManager<vda5050pp::core::events::OrderEvent>::ScopedSubscriber
      order_event_subscriber;
};

inline OpaqueState &getOpaqueState(vda5050pp::misc::AnyPtr &ptr) { return *ptr.get<OpaqueState>(); }

OrderObserver::OrderObserver() {
  auto opaque_state = std::make_shared<OpaqueState>(
      OpaqueState{vda5050pp::core::Instance::ref().getOrderEventManager().getScopedSubscriber()});

  opaque_state->order_event_subscriber.subscribe<vda5050pp::core::events::OrderNewLastNodeId>(
      [this](std::shared_ptr<vda5050pp::core::events::OrderNewLastNodeId> event) {
        std::unique_lock lock(this->last_node_mutex_);
        this->last_node_id_ = event->last_node_id;
        this->last_seq_id_ = event->seq_id;
      });
  opaque_state->order_event_subscriber.subscribe<vda5050pp::core::events::OrderActionStatusChanged>(
      [this](std::shared_ptr<vda5050pp::core::events::OrderActionStatusChanged> event) {
        std::unique_lock lock(this->action_status_mutex_);
        this->action_status_[event->action_id] = event->action_status;
      });
  opaque_state->order_event_subscriber.subscribe<vda5050pp::core::events::OrderStatus>(
      [this](std::shared_ptr<vda5050pp::core::events::OrderStatus> event) {
        std::unique_lock lock(this->order_status_mutex_);
        this->order_status_ = event->status;
      });

  this->opaque_state_ = opaque_state;
}

std::optional<std::pair<std::string, decltype(vda5050::Node::sequenceId)>>
OrderObserver::getLastNode() const {
  std::shared_lock lock(this->last_node_mutex_);

  if (!this->last_node_id_ || !this->last_seq_id_) {
    return std::nullopt;
  }

  return {{this->last_node_id_.value(), this->last_seq_id_.value()}};
}

std::optional<vda5050::ActionStatus> OrderObserver::getActionStatus(
    std::string_view action_id) const {
  std::shared_lock lock(this->action_status_mutex_);

  if (auto it = this->action_status_.find(action_id); it != this->action_status_.end()) {
    return it->second;
  } else {
    return std::nullopt;
  }
}

std::optional<vda5050pp::misc::OrderStatus> OrderObserver::getOrderStatus() const {
  std::shared_lock lock(this->order_status_mutex_);
  return this->order_status_;
}

void OrderObserver::onOrderStatusChanged(
    std::function<void(vda5050pp::misc::OrderStatus)> callback) {
  getOpaqueState(this->opaque_state_)
      .order_event_subscriber.subscribe<vda5050pp::core::events::OrderStatus>(
          [callback](std::shared_ptr<vda5050pp::core::events::OrderStatus> event) {
            callback(event->status);
          });
}

void OrderObserver::onActionStatusChanged(std::string_view action_id,
                                          std::function<void(vda5050::ActionStatus)> callback) {
  getOpaqueState(this->opaque_state_)
      .order_event_subscriber.subscribe<vda5050pp::core::events::OrderActionStatusChanged>(
          [a_id = std::string(action_id),
           callback](std::shared_ptr<vda5050pp::core::events::OrderActionStatusChanged> event) {
            if (a_id == event->action_id) {
              callback(event->action_status);
            }
          });
}

void OrderObserver::onLastNodeIdChanged(
    std::function<void(std::pair<std::string_view, decltype(vda5050::Node::sequenceId)>)>
        callback) {
  getOpaqueState(this->opaque_state_)
      .order_event_subscriber.subscribe<vda5050pp::core::events::OrderNewLastNodeId>(
          [callback](std::shared_ptr<vda5050pp::core::events::OrderNewLastNodeId> event) {
            callback({event->last_node_id, event->seq_id});
          });
}