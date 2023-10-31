//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_AGV_HANDLER_NAVIGATION_EVENT_HANDLER_H_
#define VDA5050_2B_2B_CORE_AGV_HANDLER_NAVIGATION_EVENT_HANDLER_H_

#include <memory>
#include <optional>

#include "vda5050++/core/module.h"
#include "vda5050++/core/navigation_event_manager.h"

namespace vda5050pp::core::agv_handler {

class NavigationEventHandler : public Module {
private:
  std::optional<vda5050pp::core::ScopedNavigationEventSubscriber> subscriber_;

  void handleHorizonUpdate(std::shared_ptr<vda5050pp::events::NavigationHorizonUpdate> data) const
      noexcept(false);
  void handleBaseIncreased(std::shared_ptr<vda5050pp::events::NavigationBaseIncreased> data) const
      noexcept(false);
  void handleNextNode(std::shared_ptr<vda5050pp::events::NavigationNextNode> data) const
      noexcept(false);
  void handleUpcomingSegment(
      std::shared_ptr<vda5050pp::events::NavigationUpcomingSegment> data) const noexcept(false);
  void handleControl(std::shared_ptr<vda5050pp::events::NavigationControl> data) const
      noexcept(false);

public:
  void initialize(vda5050pp::core::Instance &instance) override;
  void deinitialize(vda5050pp::core::Instance &instance) override;
  std::string_view describe() const override;
};

}  // namespace vda5050pp::core::agv_handler

#endif  // VDA5050_2B_2B_CORE_AGV_HANDLER_NAVIGATION_EVENT_HANDLER_H_
