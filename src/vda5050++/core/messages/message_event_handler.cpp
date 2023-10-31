//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/messages/message_event_handler.h"

#include <fmt/ranges.h>

#include <chrono>

#include "vda5050++/core/checks/action.h"
#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/common/formatters.h"
#include "vda5050++/core/logger.h"

using namespace vda5050pp::core::messages;
using namespace std::chrono_literals;

void MessageEventHandler::handleOrderMessage(
    std::shared_ptr<const vda5050pp::core::events::ReceiveOrderMessageEvent> evt) const {
  getMessagesLogger()->info("Handling OrderMessageEvent");

  if (evt == nullptr || evt->order == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("ReceiveOrderMessageEvent is empty"));
  }

  // Ignore duplicate orders
  if (auto [id, update_id] = Instance::ref().getOrderManager().getOrderId();
      evt->order->orderId == id && evt->order->orderUpdateId == update_id) {
    getMessagesLogger()->info(
        "Discarding duplicate order (headerId={}, orderId={}, orderUpdateId={})",
        evt->order->header.headerId, evt->order->orderId, evt->order->orderUpdateId);
    return;
  }

  // Dispatch validate order event
  getMessagesLogger()->debug("Dispatching ValidateOrderEvent");
  auto vo_evt = std::make_shared<vda5050pp::core::events::ValidateOrderEvent>();
  auto vo_future = vo_evt->getFuture();
  vo_evt->order = evt->order;
  vda5050pp::core::Instance::ref().getValidationEventManager().dispatch(vo_evt);

  if (vo_future.wait_for(1s) == std::future_status::timeout) {
    throw vda5050pp::VDA5050PPSynchronizedEventTimedOut(MK_EX_CONTEXT("ValidateOrder timed out"));
  }

  // Evaluate validate order result
  auto vo_res = vo_future.get();
  if (!vo_res.empty()) {
    getMessagesLogger()->info("Order (headerId={}) contains {} errors: {}",
                              evt->order->header.headerId, vo_res.size(), vo_res);
    getMessagesLogger()->warn("Order {}@{} will be discarded, because it contains errors.",
                              evt->order->orderId, evt->order->orderUpdateId);

    // Notify about new errors
    auto &mgr = vda5050pp::core::Instance::ref().getStatusEventManager();
    for (const auto &error : vo_res) {
      auto e_evt = std::make_shared<vda5050pp::events::ErrorAdd>();
      e_evt->error = error;
      mgr.dispatch(e_evt);
    }
  } else {
    // Notify about a new valid order
    auto o_evt = std::make_shared<vda5050pp::core::events::ValidOrderMessageEvent>();
    o_evt->valid_order = evt->order;
    vda5050pp::core::Instance::ref().getMessageEventManager().dispatch(o_evt);
  }
}

void MessageEventHandler::handleInstantActionsMessage(
    std::shared_ptr<const vda5050pp::core::events::ReceiveInstantActionMessageEvent> evt) const {
  if (evt == nullptr || evt->instant_actions == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(
        MK_EX_CONTEXT("ReceiveInstantActionMessage is empty"));
  }

  auto v_evt = std::make_shared<vda5050pp::core::events::ValidateInstantActionsEvent>();
  v_evt->instant_actions = evt->instant_actions;
  auto v_future = v_evt->getFuture();
  Instance::ref().getValidationEventManager().synchronousDispatch(v_evt);

  if (v_future.wait_for(0s) == std::future_status::timeout) {
    throw vda5050pp::VDA5050PPSynchronizedEventTimedOut(
        MK_EX_CONTEXT("ValidateInstantActions timed out"));
  }

  auto v_res = v_future.get();
  if (!v_res.empty()) {
    getMessagesLogger()->info("InstantActions (headerId={}) contains {} errors: {}", v_res.size(),
                              v_res);
    getMessagesLogger()->warn(
        "InstantActions (headerId={}) will be discarded, because it contains errors.",
        evt->instant_actions->header.headerId);

    // Notify about new errors
    auto &mgr = vda5050pp::core::Instance::ref().getStatusEventManager();
    for (const auto &error : v_res) {
      auto e_evt = std::make_shared<vda5050pp::events::ErrorAdd>();
      e_evt->error = error;
      mgr.dispatch(e_evt);
    }
  } else {
    // Notify about a new valid instant actions
    auto i_evt = std::make_shared<vda5050pp::core::events::ValidInstantActionMessageEvent>();
    i_evt->valid_instant_actions = evt->instant_actions;
    vda5050pp::core::Instance::ref().getMessageEventManager().dispatch(i_evt);
  }
}

void MessageEventHandler::initialize(vda5050pp::core::Instance &instance) {
  this->subscriber_ = instance.getMessageEventManager().getScopedSubscriber();

  this->subscriber_->subscribe<vda5050pp::core::events::ReceiveOrderMessageEvent>(
      [this](auto arg) { this->handleOrderMessage(arg); });
  this->subscriber_->subscribe<vda5050pp::core::events::ReceiveInstantActionMessageEvent>(
      [this](auto arg) { this->handleInstantActionsMessage(arg); });
}

void MessageEventHandler::deinitialize(vda5050pp::core::Instance &) { this->subscriber_.reset(); }

std::string_view MessageEventHandler::describe() const { return "MessageEventHandler"; }