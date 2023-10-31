//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/events/event_handle.h"

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/instance.h"

using namespace vda5050pp::events;

bool EventHandle::isValid() const noexcept(true) {
  return !vda5050pp::core::Instance::get().expired();
}

std::shared_ptr<ScopedActionEventSubscriber> EventHandle::getScopedActionEventSubscriber() const
    noexcept(false) {
  auto ptr = vda5050pp::core::Instance::get().lock();

  if (ptr == nullptr) {
    throw vda5050pp::VDA5050PPNotInitialized(MK_EX_CONTEXT(""));
  }

  return std::make_shared<vda5050pp::core::ScopedActionEventSubscriber>(
      ptr->getActionEventManager().getScopedActionEventSubscriber());
}

std::shared_ptr<ScopedNavigationEventSubscriber> EventHandle::getScopedNavigationEventSubscriber()
    const noexcept(false) {
  auto ptr = vda5050pp::core::Instance::get().lock();

  if (ptr == nullptr) {
    throw vda5050pp::VDA5050PPNotInitialized(MK_EX_CONTEXT(""));
  }

  return std::make_shared<vda5050pp::core::ScopedNavigationEventSubscriber>(
      ptr->getNavigationEventManager().getScopedNavigationEventSubscriber());
}

std::shared_ptr<ScopedQueryEventSubscriber> EventHandle::getScopedQueryEventSubscriber() const
    noexcept(false) {
  auto ptr = vda5050pp::core::Instance::get().lock();

  if (ptr == nullptr) {
    throw vda5050pp::VDA5050PPNotInitialized(MK_EX_CONTEXT(""));
  }

  return std::make_shared<vda5050pp::core::ScopedQueryEventSubscriber>(
      ptr->getQueryEventManager().getScopedQueryEventSubscriber());
}

void EventHandle::dispatch(std::shared_ptr<vda5050pp::events::ActionStatusWaiting> data) const {
  vda5050pp::core::Instance::ref().getActionStatusManager().dispatch(data);
}
void EventHandle::dispatch(
    std::shared_ptr<vda5050pp::events::ActionStatusInitializing> data) const {
  vda5050pp::core::Instance::ref().getActionStatusManager().dispatch(data);
}
void EventHandle::dispatch(std::shared_ptr<vda5050pp::events::ActionStatusRunning> data) const {
  vda5050pp::core::Instance::ref().getActionStatusManager().dispatch(data);
}
void EventHandle::dispatch(std::shared_ptr<vda5050pp::events::ActionStatusPaused> data) const {
  vda5050pp::core::Instance::ref().getActionStatusManager().dispatch(data);
}
void EventHandle::dispatch(std::shared_ptr<vda5050pp::events::ActionStatusFinished> data) const {
  vda5050pp::core::Instance::ref().getActionStatusManager().dispatch(data);
}
void EventHandle::dispatch(std::shared_ptr<vda5050pp::events::ActionStatusFailed> data) const {
  vda5050pp::core::Instance::ref().getActionStatusManager().dispatch(data);
}

void EventHandle::dispatch(
    std::shared_ptr<vda5050pp::events::NavigationStatusPosition> data) const {
  vda5050pp::core::Instance::ref().getNavigationStatusManager().dispatch(data);
}
void EventHandle::dispatch(
    std::shared_ptr<vda5050pp::events::NavigationStatusVelocity> data) const {
  vda5050pp::core::Instance::ref().getNavigationStatusManager().dispatch(data);
}
void EventHandle::dispatch(std::shared_ptr<vda5050pp::events::NavigationStatusDriving> data) const {
  vda5050pp::core::Instance::ref().getNavigationStatusManager().dispatch(data);
}
void EventHandle::dispatch(
    std::shared_ptr<vda5050pp::events::NavigationStatusNodeReached> data) const {
  vda5050pp::core::Instance::ref().getNavigationStatusManager().dispatch(data);
}
void EventHandle::dispatch(
    std::shared_ptr<vda5050pp::events::NavigationStatusDistanceSinceLastNode> data) const {
  vda5050pp::core::Instance::ref().getNavigationStatusManager().dispatch(data);
}

void EventHandle::dispatch(std::shared_ptr<vda5050pp::events::NavigationStatusControl> data) const
    noexcept(false) {
  vda5050pp::core::Instance::ref().getNavigationStatusManager().dispatch(data);
}

void EventHandle::dispatch(std::shared_ptr<vda5050pp::events::StatusEvent> data) const
    noexcept(false) {
  vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(data);
}