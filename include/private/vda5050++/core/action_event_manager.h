//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PRIVATE_VDA5050_2B_2B_CORE_ACTION_EVENT_MANAGER_H_
#define PRIVATE_VDA5050_2B_2B_CORE_ACTION_EVENT_MANAGER_H_

#include <eventpp/eventqueue.h>
#include <eventpp/utilities/scopedremover.h>

#include <functional>
#include <memory>
#include <thread>

#include "vda5050++/config/event_manager_options.h"
#include "vda5050++/core/common/scoped_thread.h"
#include "vda5050++/events/action_event.h"
#include "vda5050++/events/scoped_action_event_subscriber.h"

namespace vda5050pp::core {

using ActionEventQueue = eventpp::EventQueue<vda5050pp::events::ActionEventType,
                                             void(std::shared_ptr<vda5050pp::events::ActionEvent>)>;
class ScopedActionEventSubscriber : public vda5050pp::events::ScopedActionEventSubscriber {
private:
  friend class ActionEventManager;
  eventpp::ScopedRemover<ActionEventQueue> remover_;

  explicit ScopedActionEventSubscriber(ActionEventQueue &queue);

public:
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionList>)>
                     &&callback) noexcept(true) override;
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionValidate>)>
                     &&callback) noexcept(true) override;
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionPrepare>)>
                     &&callback) noexcept(true) override;
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionStart>)>
                     &&callback) noexcept(true) override;
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionPause>)>
                     &&callback) noexcept(true) override;
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionResume>)>
                     &&callback) noexcept(true) override;
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionCancel>)>
                     &&callback) noexcept(true) override;
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionForget>)>
                     &&callback) noexcept(true) override;
};

class ActionEventManager {
private:
  ActionEventQueue action_event_queue_;

  const vda5050pp::config::EventManagerOptions &opts_;

  vda5050pp::core::common::ScopedThread<void()> thread_;

  void threadTask(vda5050pp::core::common::StopToken tkn) noexcept(true);

public:
  explicit ActionEventManager(const vda5050pp::config::EventManagerOptions &opts);

  void dispatch(std::shared_ptr<vda5050pp::events::ActionList> data,
                bool synchronous = false) noexcept(false);
  void dispatch(std::shared_ptr<vda5050pp::events::ActionValidate> data,
                bool synchronous = false) noexcept(false);
  void dispatch(std::shared_ptr<vda5050pp::events::ActionPrepare> data,
                bool synchronous = false) noexcept(false);
  void dispatch(std::shared_ptr<vda5050pp::events::ActionStart> data,
                bool synchronous = false) noexcept(false);
  void dispatch(std::shared_ptr<vda5050pp::events::ActionPause> data,
                bool synchronous = false) noexcept(false);
  void dispatch(std::shared_ptr<vda5050pp::events::ActionResume> data,
                bool synchronous = false) noexcept(false);
  void dispatch(std::shared_ptr<vda5050pp::events::ActionCancel> data,
                bool synchronous = false) noexcept(false);
  void dispatch(std::shared_ptr<vda5050pp::events::ActionForget> data,
                bool synchronous = false) noexcept(false);

  ScopedActionEventSubscriber getScopedActionEventSubscriber() noexcept(true);
};

}  // namespace vda5050pp::core

#endif  // PRIVATE_VDA5050_2B_2B_CORE_ACTION_EVENT_MANAGER_H_
