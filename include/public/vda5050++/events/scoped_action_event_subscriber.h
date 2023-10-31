//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_EVENTS_SCOPED_ACTION_EVENT_SUBSCRIBER_H_
#define PUBLIC_VDA5050_2B_2B_EVENTS_SCOPED_ACTION_EVENT_SUBSCRIBER_H_

#include <functional>
#include <memory>

#include "vda5050++/events/action_event.h"

namespace vda5050pp::events {

///
///\brief The scoped ActionEvent subscriber. This class can only be instantiated by the library.
/// As a user you can get an instance via the EventHandle. The subscribe functions can be set to
/// subscribe to library events. As the name implies, the lifetime of the subscription is RAII
/// based.
///
class ScopedActionEventSubscriber {
public:
  virtual ~ScopedActionEventSubscriber() = default;

  ///
  ///\brief Subscribe an ActionEvent.
  ///
  ///\param callback the handler to set.
  ///
  virtual void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionList>)>
                             &&callback) noexcept(true) = 0;

  ///
  ///\brief Subscribe an ActionEvent.
  ///
  ///\param callback the handler to set.
  ///
  virtual void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionValidate>)>
                             &&callback) noexcept(true) = 0;

  ///
  ///\brief Subscribe an ActionEvent.
  ///
  ///\param callback the handler to set.
  ///
  virtual void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionPrepare>)>
                             &&callback) noexcept(true) = 0;

  ///
  ///\brief Subscribe an ActionEvent.
  ///
  ///\param callback the handler to set.
  ///
  virtual void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionStart>)>
                             &&callback) noexcept(true) = 0;

  ///
  ///\brief Subscribe an ActionEvent.
  ///
  ///\param callback the handler to set.
  ///
  virtual void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionPause>)>
                             &&callback) noexcept(true) = 0;

  ///
  ///\brief Subscribe an ActionEvent.
  ///
  ///\param callback the handler to set.
  ///
  virtual void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionResume>)>
                             &&callback) noexcept(true) = 0;

  ///
  ///\brief Subscribe an ActionEvent.
  ///
  ///\param callback the handler to set.
  ///
  virtual void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionCancel>)>
                             &&callback) noexcept(true) = 0;

  ///
  ///\brief Subscribe an ActionEvent.
  ///
  ///\param callback the handler to set.
  ///
  virtual void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionForget>)>
                             &&callback) noexcept(true) = 0;
};

}  // namespace vda5050pp::events

#endif  // PUBLIC_VDA5050_2B_2B_EVENTS_SCOPED_ACTION_EVENT_SUBSCRIBER_H_
