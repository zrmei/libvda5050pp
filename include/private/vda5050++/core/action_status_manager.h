//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PRIVATE_VDA5050_2B_2B_CORE_ACTION_STATUS_MANAGER_H_
#define PRIVATE_VDA5050_2B_2B_CORE_ACTION_STATUS_MANAGER_H_

#include <eventpp/eventqueue.h>
#include <eventpp/utilities/scopedremover.h>

#include <functional>
#include <memory>
#include <thread>

#include "vda5050++/config/event_manager_options.h"
#include "vda5050++/core/common/scoped_thread.h"
#include "vda5050++/events/action_event.h"

namespace vda5050pp::core {

using ActionStatusQueue =
    eventpp::EventQueue<vda5050pp::events::ActionStatusType,
                        void(std::shared_ptr<vda5050pp::events::ActionStatus>)>;

class ScopedActionStatusSubscriber {
private:
  friend class ActionStatusManager;
  eventpp::ScopedRemover<ActionStatusQueue> remover_;

  explicit ScopedActionStatusSubscriber(ActionStatusQueue &queue);

public:
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionStatusWaiting>)>
                     &&callback) noexcept(true);
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionStatusInitializing>)>
                     &&callback) noexcept(true);
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionStatusRunning>)>
                     &&callback) noexcept(true);
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionStatusPaused>)>
                     &&callback) noexcept(true);
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionStatusFinished>)>
                     &&callback) noexcept(true);
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ActionStatusFailed>)>
                     &&callback) noexcept(true);
};

class ActionStatusManager {
private:
  ActionStatusQueue action_status_queue_;

  const vda5050pp::config::EventManagerOptions &opts_;

  vda5050pp::core::common::ScopedThread<void()> thread_;

  void threadTask(vda5050pp::core::common::StopToken tkn) noexcept(true);

public:
  explicit ActionStatusManager(const vda5050pp::config::EventManagerOptions &opts);

  void dispatch(std::shared_ptr<vda5050pp::events::ActionStatusWaiting> data) noexcept(true);
  void dispatch(std::shared_ptr<vda5050pp::events::ActionStatusInitializing> data) noexcept(true);
  void dispatch(std::shared_ptr<vda5050pp::events::ActionStatusRunning> data) noexcept(true);
  void dispatch(std::shared_ptr<vda5050pp::events::ActionStatusPaused> data) noexcept(true);
  void dispatch(std::shared_ptr<vda5050pp::events::ActionStatusFinished> data) noexcept(true);
  void dispatch(std::shared_ptr<vda5050pp::events::ActionStatusFailed> data) noexcept(true);

  ScopedActionStatusSubscriber getScopedActionStatusSubscriber() noexcept(true);
};

}  // namespace vda5050pp::core

#endif  // PRIVATE_VDA5050_2B_2B_CORE_ACTION_STATUS_MANAGER_H_
