//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_EVENTS_SCOPED_NAVIGATION_EVENT_SUBSCRIBER_H_
#define PUBLIC_VDA5050_2B_2B_EVENTS_SCOPED_NAVIGATION_EVENT_SUBSCRIBER_H_

#include <functional>
#include <memory>

#include "vda5050++/events/navigation_event.h"

namespace vda5050pp::events {

///
///\brief The scoped NavigationEvent subscriber. This class can only be instantiated by the library.
/// As a user you can get an instance via the EventHandle. The subscribe functions can be set to
/// subscribe to library events. As the name implies, the lifetime of the subscription is RAII
/// based.
///
class ScopedNavigationEventSubscriber {
public:
  virtual ~ScopedNavigationEventSubscriber() = default;

  ///
  ///\brief Subscribe to a NavigationEvent.
  ///
  ///\param callback the subscriber function.
  ///
  virtual void subscribe(
      std::function<void(std::shared_ptr<vda5050pp::events::NavigationHorizonUpdate>)>
          &&callback) noexcept(true) = 0;

  ///
  ///\brief Subscribe to a NavigationEvent.
  ///
  ///\param callback the subscriber function.
  ///
  virtual void subscribe(
      std::function<void(std::shared_ptr<vda5050pp::events::NavigationBaseIncreased>)>
          &&callback) noexcept(true) = 0;

  ///
  ///\brief Subscribe to a NavigationEvent.
  ///
  ///\param callback the subscriber function.
  ///
  virtual void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::NavigationNextNode>)>
                             &&callback) noexcept(true) = 0;

  ///
  ///\brief Subscribe to a NavigationEvent.
  ///
  ///\param callback the subscriber function.
  ///
  virtual void subscribe(
      std::function<void(std::shared_ptr<vda5050pp::events::NavigationUpcomingSegment>)>
          &&callback) noexcept(true) = 0;

  ///
  ///\brief Subscribe to a NavigationEvent.
  ///
  ///\param callback the subscriber function.
  ///
  virtual void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::NavigationControl>)>
                             &&callback) noexcept(true) = 0;
};

}  // namespace vda5050pp::events

#endif  // PUBLIC_VDA5050_2B_2B_EVENTS_SCOPED_NAVIGATION_EVENT_SUBSCRIBER_H_
