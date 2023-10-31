//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PRIVATE_VDA5050_2B_2B_CORE_NAVIGATION_EVENT_MANAGER_H_
#define PRIVATE_VDA5050_2B_2B_CORE_NAVIGATION_EVENT_MANAGER_H_

#include <eventpp/eventqueue.h>
#include <eventpp/utilities/scopedremover.h>

#include <functional>
#include <memory>

#include "vda5050++/config/event_manager_options.h"
#include "vda5050++/core/common/scoped_thread.h"
#include "vda5050++/events/navigation_event.h"
#include "vda5050++/events/scoped_navigation_event_subscriber.h"

namespace vda5050pp::core {

using NavigationEventQueue =
    eventpp::EventQueue<vda5050pp::events::NavigationEventType,
                        void(std::shared_ptr<vda5050pp::events::NavigationEvent>)>;
class ScopedNavigationEventSubscriber : public vda5050pp::events::ScopedNavigationEventSubscriber {
private:
  friend class NavigationEventManager;
  eventpp::ScopedRemover<NavigationEventQueue> remover_;

  explicit ScopedNavigationEventSubscriber(NavigationEventQueue &queue);

public:
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::NavigationHorizonUpdate>)>
                     &&callback) noexcept(true) override;
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::NavigationBaseIncreased>)>
                     &&callback) noexcept(true) override;
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::NavigationNextNode>)>
                     &&callback) noexcept(true) override;
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::NavigationUpcomingSegment>)>
                     &&callback) noexcept(true) override;
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::NavigationControl>)>
                     &&callback) noexcept(true) override;
};

class NavigationEventManager {
private:
  NavigationEventQueue navigation_event_queue_;

  const vda5050pp::config::EventManagerOptions &opts_;

  vda5050pp::core::common::ScopedThread<void()> thread_;

  void threadTask(vda5050pp::core::common::StopToken tkn) noexcept(true);

public:
  explicit NavigationEventManager(const vda5050pp::config::EventManagerOptions &opts);

  void dispatch(std::shared_ptr<vda5050pp::events::NavigationHorizonUpdate> data) noexcept(true);
  void dispatch(std::shared_ptr<vda5050pp::events::NavigationBaseIncreased> data) noexcept(true);
  void dispatch(std::shared_ptr<vda5050pp::events::NavigationNextNode> data) noexcept(true);
  void dispatch(std::shared_ptr<vda5050pp::events::NavigationUpcomingSegment> data) noexcept(true);
  void dispatch(std::shared_ptr<vda5050pp::events::NavigationControl> data) noexcept(true);

  ScopedNavigationEventSubscriber getScopedNavigationEventSubscriber() noexcept(true);
};

}  // namespace vda5050pp::core

#endif  // PRIVATE_VDA5050_2B_2B_CORE_NAVIGATION_EVENT_MANAGER_H_
