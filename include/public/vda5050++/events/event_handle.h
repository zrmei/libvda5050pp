//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_EVENTS_EVENT_HANDLE_H_
#define PUBLIC_VDA5050_2B_2B_EVENTS_EVENT_HANDLE_H_

#include <memory>

#include "vda5050++/events/scoped_action_event_subscriber.h"
#include "vda5050++/events/scoped_navigation_event_subscriber.h"
#include "vda5050++/events/scoped_query_event_subscriber.h"
#include "vda5050++/events/status_event.h"

namespace vda5050pp::events {

///
///\brief This class can be instantiated to send/receive Events manually. (Alternative to
/// vda5050pp::handler and vda505pp::sinks).
///
class EventHandle {
public:
  ///
  ///\brief Is this EventHandle valid i.e. is the library initialized?
  ///
  ///\return valid?
  ///
  bool isValid() const noexcept(true);

  ///
  ///\brief Get a new ScopedActionEventSubscriber.
  ///
  ///\return std::shared_ptr<ScopedActionEventSubscriber>
  ///\throws VDA5050PPNotInitialized if (this->isValid() == false)
  ///
  std::shared_ptr<ScopedActionEventSubscriber> getScopedActionEventSubscriber() const
      noexcept(false);

  ///
  ///\brief Get a new ScopedNavigationEventSubscriber.
  ///
  ///\return std::shared_ptr<ScopedNavigationEventSubscriber>
  ///\throws VDA5050PPNotInitialized if (this->isValid() == false)
  ///
  ///
  std::shared_ptr<ScopedNavigationEventSubscriber> getScopedNavigationEventSubscriber() const
      noexcept(false);

  ///
  ///\brief Get a new ScopedQueryEventSubscriber.
  ///
  ///\return std::shared_ptr<ScopedQueryEventSubscriber>
  ///\throws VDA5050PPNotInitialized if (this->isValid() == false)
  ///
  std::shared_ptr<ScopedQueryEventSubscriber> getScopedQueryEventSubscriber() const noexcept(false);

  ///
  ///\brief Dispatch an event.
  ///
  ///\param data The event to dispatch
  ///\throws VDA5050PPNotInitialized if (this->isValid() == false)
  ///
  void dispatch(std::shared_ptr<vda5050pp::events::ActionStatusWaiting> data) const noexcept(false);

  ///
  ///\brief Dispatch an event.
  ///
  ///\param data The event to dispatch
  ///\throws VDA5050PPNotInitialized if (this->isValid() == false)
  ///
  void dispatch(std::shared_ptr<vda5050pp::events::ActionStatusInitializing> data) const
      noexcept(false);

  ///
  ///\brief Dispatch an event.
  ///
  ///\param data The event to dispatch
  ///\throws VDA5050PPNotInitialized if (this->isValid() == false)
  ///
  void dispatch(std::shared_ptr<vda5050pp::events::ActionStatusRunning> data) const noexcept(false);

  ///
  ///\brief Dispatch an event.
  ///
  ///\param data The event to dispatch
  ///\throws VDA5050PPNotInitialized if (this->isValid() == false)
  ///
  void dispatch(std::shared_ptr<vda5050pp::events::ActionStatusPaused> data) const noexcept(false);

  ///
  ///\brief Dispatch an event.
  ///
  ///\param data The event to dispatch
  ///\throws VDA5050PPNotInitialized if (this->isValid() == false)
  ///
  void dispatch(std::shared_ptr<vda5050pp::events::ActionStatusFinished> data) const
      noexcept(false);

  ///
  ///\brief Dispatch an event.
  ///
  ///\param data The event to dispatch
  ///\throws VDA5050PPNotInitialized if (this->isValid() == false)
  ///
  void dispatch(std::shared_ptr<vda5050pp::events::ActionStatusFailed> data) const noexcept(false);

  ///
  ///\brief Dispatch an event.
  ///
  ///\param data The event to dispatch
  ///\throws VDA5050PPNotInitialized if (this->isValid() == false)
  ///
  void dispatch(std::shared_ptr<vda5050pp::events::NavigationStatusPosition> data) const
      noexcept(false);

  ///
  ///\brief Dispatch an event.
  ///
  ///\param data The event to dispatch
  ///\throws VDA5050PPNotInitialized if (this->isValid() == false)
  ///
  void dispatch(std::shared_ptr<vda5050pp::events::NavigationStatusVelocity> data) const
      noexcept(false);

  ///
  ///\brief Dispatch an event.
  ///
  ///\param data The event to dispatch
  ///\throws VDA5050PPNotInitialized if (this->isValid() == false)
  ///
  void dispatch(std::shared_ptr<vda5050pp::events::NavigationStatusDriving> data) const
      noexcept(false);

  ///
  ///\brief Dispatch an event.
  ///
  ///\param data The event to dispatch
  ///\throws VDA5050PPNotInitialized if (this->isValid() == false)
  ///
  void dispatch(std::shared_ptr<vda5050pp::events::NavigationStatusNodeReached> data) const
      noexcept(false);

  ///
  ///\brief Dispatch an event.
  ///
  ///\param data The event to dispatch
  ///\throws VDA5050PPNotInitialized if (this->isValid() == false)
  ///
  void dispatch(std::shared_ptr<vda5050pp::events::NavigationStatusDistanceSinceLastNode> data)
      const noexcept(false);

  ///
  ///\brief Dispatch an event.
  ///
  ///\param data The event to dispatch
  ///\throws VDA5050PPNotInitialized if (this->isValid() == false)
  ///
  void dispatch(std::shared_ptr<vda5050pp::events::NavigationStatusControl> data)
      const noexcept(false);

  ///
  ///\brief Dispatch an event.
  ///
  ///\param data The event to dispatch
  ///\throws VDA5050PPNotInitialized if (this->isValid() == false)
  ///
  void dispatch(std::shared_ptr<vda5050pp::events::StatusEvent> data) const noexcept(false);
};

}  // namespace vda5050pp::events

#endif  // PUBLIC_VDA5050_2B_2B_EVENTS_EVENT_HANDLE_H_
