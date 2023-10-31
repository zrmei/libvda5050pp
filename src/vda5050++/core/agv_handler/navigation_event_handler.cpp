//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/core/agv_handler/navigation_event_handler.h"

#include <functional>

#include "vda5050++/core/instance.h"
#include "vda5050++/core/logger.h"

using namespace vda5050pp::core::agv_handler;

void NavigationEventHandler::handleHorizonUpdate(
    std::shared_ptr<vda5050pp::events::NavigationHorizonUpdate> data) const noexcept(false) {
  auto handler = vda5050pp::core::Instance::ref().getNavigationHandler().lock();

  if (data == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("HorizonUpdate contains no data"));
  }

  if (handler == nullptr) {
    getAGVHandlerLogger()->info(
        "No NavigationHandler set. The HorizonUpdateEvent must be handled by the user.");
    return;
  }

  handler->horizonUpdated(data->horz_replace_nodes, data->horz_replace_edges);
}

void NavigationEventHandler::handleBaseIncreased(
    std::shared_ptr<vda5050pp::events::NavigationBaseIncreased> data) const noexcept(false) {
  auto handler = vda5050pp::core::Instance::ref().getNavigationHandler().lock();

  if (data == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("BaseIncreased contains no data"));
  }

  if (handler == nullptr) {
    getAGVHandlerLogger()->info(
        "No NavigationHandler set. The BaseIncreasedEvent must be handled by the user.");
    return;
  }

  handler->baseIncreased(data->base_expand_nodes, data->base_expand_edges);
}

void NavigationEventHandler::handleNextNode(
    std::shared_ptr<vda5050pp::events::NavigationNextNode> data) const noexcept(false) {
  auto handler = vda5050pp::core::Instance::ref().getNavigationHandler().lock();

  if (data == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("NextNode contains no data"));
  }

  if (handler == nullptr) {
    getAGVHandlerLogger()->info(
        "No NavigationHandler set. The NextNodeEvent must be handled by the user.");
    return;
  }

  handler->navigateToNextNode(data->next_node, data->via_edge);
}

void NavigationEventHandler::handleUpcomingSegment(
    std::shared_ptr<vda5050pp::events::NavigationUpcomingSegment> data) const noexcept(false) {
  auto handler = vda5050pp::core::Instance::ref().getNavigationHandler().lock();

  if (data == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("UpcomingSegment contains no data"));
  }

  if (handler == nullptr) {
    getAGVHandlerLogger()->info(
        "No NavigationHandler set. The UpcomingSegment must be handled by the user.");
    return;
  }

  handler->upcomingSegment(data->begin_seq, data->end_seq);
}

void NavigationEventHandler::handleControl(
    std::shared_ptr<vda5050pp::events::NavigationControl> data) const noexcept(false) {
  auto handler = vda5050pp::core::Instance::ref().getNavigationHandler().lock();

  if (data == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("NavigationControl contains no data"));
  }

  if (handler == nullptr) {
    getAGVHandlerLogger()->info(
        "No NavigationHandler set. The NavigationControl must be handled by the user.");
    return;
  }

  switch (data->type) {
    case vda5050pp::events::NavigationControlType::k_cancel:
      handler->cancel();
      break;
    case vda5050pp::events::NavigationControlType::k_pause:
      handler->pause();
      break;
    case vda5050pp::events::NavigationControlType::k_resume:
      handler->resume();
      break;
    default:
      throw vda5050pp::VDA5050PPInvalidEventData(
          MK_EX_CONTEXT(fmt::format("Unknown EventType ({})", int(data->type))));
  }
}

void NavigationEventHandler::initialize(vda5050pp::core::Instance &instance) {
  this->subscriber_ = instance.getNavigationEventManager().getScopedNavigationEventSubscriber();

  this->subscriber_->subscribe(std::bind(std::mem_fn(&NavigationEventHandler::handleHorizonUpdate),
                                         this, std::placeholders::_1));
  this->subscriber_->subscribe(std::bind(std::mem_fn(&NavigationEventHandler::handleBaseIncreased),
                                         this, std::placeholders::_1));
  this->subscriber_->subscribe(
      std::bind(std::mem_fn(&NavigationEventHandler::handleNextNode), this, std::placeholders::_1));
  this->subscriber_->subscribe(std::bind(
      std::mem_fn(&NavigationEventHandler::handleUpcomingSegment), this, std::placeholders::_1));
  this->subscriber_->subscribe(
      std::bind(std::mem_fn(&NavigationEventHandler::handleControl), this, std::placeholders::_1));
}

void NavigationEventHandler::deinitialize(vda5050pp::core::Instance &) {
  this->subscriber_.reset();
}

std::string_view NavigationEventHandler::describe() const { return "NavigationEventHandler"; }