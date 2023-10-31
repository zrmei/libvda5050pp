//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_AGV_HANDLER_NODE_REACHED_HANDLER_H_
#define VDA5050_2B_2B_CORE_AGV_HANDLER_NODE_REACHED_HANDLER_H_

#include <optional>

#include "vda5050++/config/node_reached_subconfig.h"
#include "vda5050++/core/module.h"
#include "vda5050++/core/navigation_status_manager.h"

namespace vda5050pp::core::agv_handler {

class NodeReachedHandler : public vda5050pp::core::Module {
  void handleNavigationStatusPosition(
      std::shared_ptr<vda5050pp::events::NavigationStatusPosition> evt) const;

  std::optional<vda5050pp::core::ScopedNavigationStatusSubscriber> navigation_status_subscriber_;
  std::shared_ptr<vda5050pp::config::NodeReachedSubConfig> sub_config_;

public:
  void initialize(Instance &instance) override;
  void deinitialize(Instance &instance) override;
  std::string_view describe() const override;
  std::shared_ptr<config::ModuleSubConfig> generateSubConfig() const override;
};

}  // namespace vda5050pp::core::agv_handler

#endif  // VDA5050_2B_2B_CORE_AGV_HANDLER_NODE_REACHED_HANDLER_H_
