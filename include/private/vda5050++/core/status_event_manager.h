//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PRIVATE_VDA5050_2B_2B_CORE_STATUS_EVENT_MANAGER_H_
#define PRIVATE_VDA5050_2B_2B_CORE_STATUS_EVENT_MANAGER_H_

#include <eventpp/eventqueue.h>
#include <eventpp/utilities/scopedremover.h>

#include <functional>
#include <memory>

#include "vda5050++/config/event_manager_options.h"
#include "vda5050++/core/common/scoped_thread.h"
#include "vda5050++/events/status_event.h"

namespace vda5050pp::core {

using StatusEventQueue = eventpp::EventQueue<vda5050pp::events::StatusEventType,
                                             void(std::shared_ptr<vda5050pp::events::StatusEvent>)>;
class ScopedStatusEventSubscriber {
private:
  friend class StatusEventManager;
  eventpp::ScopedRemover<StatusEventQueue> remover_;

  explicit ScopedStatusEventSubscriber(StatusEventQueue &queue);

public:
  void subscribe(
      std::function<void(std::shared_ptr<vda5050pp::events::LoadAdd>)> &&callback) noexcept(true);
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::LoadRemove>)>
                     &&callback) noexcept(true);
  void subscribe(
      std::function<void(std::shared_ptr<vda5050pp::events::LoadsGet>)> &&callback) noexcept(true);
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::LoadsAlter>)>
                     &&callback) noexcept(true);
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::OperatingModeSet>)>
                     &&callback) noexcept(true);
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::OperatingModeGet>)>
                     &&callback) noexcept(true);
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::OperatingModeAlter>)>
                     &&callback) noexcept(true);
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::BatteryStateSet>)>
                     &&callback) noexcept(true);
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::BatteryStateGet>)>
                     &&callback) noexcept(true);
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::BatteryStateAlter>)>
                     &&callback) noexcept(true);
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::RequestNewBase>)>
                     &&callback) noexcept(true);
  void subscribe(
      std::function<void(std::shared_ptr<vda5050pp::events::ErrorAdd>)> &&callback) noexcept(true);
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::ErrorsAlter>)>
                     &&callback) noexcept(true);
  void subscribe(
      std::function<void(std::shared_ptr<vda5050pp::events::InfoAdd>)> &&callback) noexcept(true);
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::InfosAlter>)>
                     &&callback) noexcept(true);
};

class StatusEventManager {
private:
  StatusEventQueue status_event_queue_;

  const vda5050pp::config::EventManagerOptions &opts_;

  vda5050pp::core::common::ScopedThread<void()> thread_;

  void threadTask(vda5050pp::core::common::StopToken tkn) noexcept(true);

public:
  explicit StatusEventManager(const vda5050pp::config::EventManagerOptions &opts);

  void dispatch(std::shared_ptr<vda5050pp::events::StatusEvent> data,
                bool synchronous = false) noexcept(false);

  ScopedStatusEventSubscriber getScopedStatusEventSubscriber() noexcept(true);
};

}  // namespace vda5050pp::core

#endif  // PRIVATE_VDA5050_2B_2B_CORE_STATUS_EVENT_MANAGER_H_
