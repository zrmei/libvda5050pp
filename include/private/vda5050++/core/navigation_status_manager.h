//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PRIVATE_VDA5050_2B_2B_CORE_NAVIGATION_STATUS_MANAGER_H_
#define PRIVATE_VDA5050_2B_2B_CORE_NAVIGATION_STATUS_MANAGER_H_

#include <eventpp/eventqueue.h>
#include <eventpp/utilities/scopedremover.h>

#include <functional>
#include <memory>
#include <thread>

#include "vda5050++/config/event_manager_options.h"
#include "vda5050++/core/common/scoped_thread.h"
#include "vda5050++/events/navigation_event.h"

namespace vda5050pp::core {

using NavigationStatusQueue =
    eventpp::EventQueue<vda5050pp::events::NavigationStatusType,
                        void(std::shared_ptr<vda5050pp::events::NavigationStatus>)>;

class ScopedNavigationStatusSubscriber {
private:
  friend class NavigationStatusManager;
  eventpp::ScopedRemover<NavigationStatusQueue> remover_;

  explicit ScopedNavigationStatusSubscriber(NavigationStatusQueue &queue);

public:
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::NavigationStatusPosition>)>
                     &&callback) noexcept(true);
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::NavigationStatusVelocity>)>
                     &&callback) noexcept(true);
  void subscribe(
      std::function<void(std::shared_ptr<vda5050pp::events::NavigationStatusNodeReached>)>
          &&callback) noexcept(true);
  void subscribe(
      std::function<void(std::shared_ptr<vda5050pp::events::NavigationStatusDistanceSinceLastNode>)>
          &&callback) noexcept(true);
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::NavigationStatusDriving>)>
                     &&callback) noexcept(true);
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::NavigationStatusControl>)>
                     &&callback) noexcept(true);
};

class NavigationStatusManager {
private:
  NavigationStatusQueue navigation_status_queue_;

  const vda5050pp::config::EventManagerOptions &opts_;

  vda5050pp::core::common::ScopedThread<void()> thread_;

  void threadTask(vda5050pp::core::common::StopToken tkn) noexcept(true);

public:
  explicit NavigationStatusManager(const vda5050pp::config::EventManagerOptions &opts);

  void dispatch(std::shared_ptr<vda5050pp::events::NavigationStatusPosition> data) noexcept(true);
  void dispatch(std::shared_ptr<vda5050pp::events::NavigationStatusVelocity> data) noexcept(true);
  void dispatch(std::shared_ptr<vda5050pp::events::NavigationStatusNodeReached> data) noexcept(
      true);
  void dispatch(std::shared_ptr<vda5050pp::events::NavigationStatusDistanceSinceLastNode>
                    data) noexcept(true);
  void dispatch(std::shared_ptr<vda5050pp::events::NavigationStatusDriving> data) noexcept(true);
  void dispatch(std::shared_ptr<vda5050pp::events::NavigationStatusControl> data) noexcept(true);

  ScopedNavigationStatusSubscriber getScopedNavigationStatusSubscriber() noexcept(true);
};

}  // namespace vda5050pp::core

#endif  // PRIVATE_VDA5050_2B_2B_CORE_NAVIGATION_STATUS_MANAGER_H_
