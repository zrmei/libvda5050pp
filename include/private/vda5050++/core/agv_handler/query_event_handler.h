// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains the QueryEventHandler.
//
//

#ifndef VDA5050_2B_2B_CORE_AGV_HANDLER_QUERY_EVENT_HANDLER_H_
#define VDA5050_2B_2B_CORE_AGV_HANDLER_QUERY_EVENT_HANDLER_H_

#include <optional>

#include "vda5050++/core/instance.h"
#include "vda5050++/core/module.h"

namespace vda5050pp::core::agv_handler {

class QueryEventHandler : public vda5050pp::core::Module {
private:
  std::optional<vda5050pp::core::ScopedQueryEventSubscriber> subscriber_;

  void handleQueryPauseableEvent(std::shared_ptr<vda5050pp::events::QueryPauseable> data) const
      noexcept(false);
  void handleQueryResumableEvent(std::shared_ptr<vda5050pp::events::QueryResumable> data) const
      noexcept(false);
  void handleQueryAcceptZoneSet(std::shared_ptr<vda5050pp::events::QueryAcceptZoneSet> data) const
      noexcept(false);

public:
  void initialize(vda5050pp::core::Instance &instance) override;
  void deinitialize(vda5050pp::core::Instance &instance) override;
  std::string_view describe() const override;
  std::shared_ptr<vda5050pp::config::ModuleSubConfig> generateSubConfig() const override;
};

}  // namespace vda5050pp::core::agv_handler

#endif  // VDA5050_2B_2B_CORE_AGV_HANDLER_QUERY_EVENT_HANDLER_H_
