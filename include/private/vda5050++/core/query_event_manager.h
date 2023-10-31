//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PRIVATE_VDA5050_2B_2B_CORE_QUERY_EVENT_MANAGER_H_
#define PRIVATE_VDA5050_2B_2B_CORE_QUERY_EVENT_MANAGER_H_

#include <eventpp/eventqueue.h>
#include <eventpp/utilities/scopedremover.h>

#include <functional>
#include <memory>
#include <thread>

#include "vda5050++/config/event_manager_options.h"
#include "vda5050++/core/common/scoped_thread.h"
#include "vda5050++/events/query_event.h"
#include "vda5050++/events/scoped_query_event_subscriber.h"

namespace vda5050pp::core {

using QueryEventQueue = eventpp::EventQueue<vda5050pp::events::QueryEventType,
                                            void(std::shared_ptr<vda5050pp::events::QueryEvent>)>;
class ScopedQueryEventSubscriber : public vda5050pp::events::ScopedQueryEventSubscriber {
private:
  friend class QueryEventManager;
  eventpp::ScopedRemover<QueryEventQueue> remover_;

  explicit ScopedQueryEventSubscriber(QueryEventQueue &queue);

public:
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::QueryPauseable>)>
                     &&callback) noexcept(true) override;
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::QueryResumable>)>
                     &&callback) noexcept(true) override;
  void subscribe(std::function<void(std::shared_ptr<vda5050pp::events::QueryAcceptZoneSet>)>
                     &&callback) noexcept(true) override;
};

class QueryEventManager {
private:
  QueryEventQueue query_event_queue_;

  const vda5050pp::config::EventManagerOptions &opts_;

  vda5050pp::core::common::ScopedThread<void()> thread_;

  void threadTask(vda5050pp::core::common::StopToken tkn) noexcept(true);

public:
  explicit QueryEventManager(const vda5050pp::config::EventManagerOptions &opts);

  void dispatch(std::shared_ptr<vda5050pp::events::QueryPauseable> data,
                bool synchronous = false) noexcept(true);
  void dispatch(std::shared_ptr<vda5050pp::events::QueryResumable> data,
                bool synchronous = false) noexcept(true);
  void dispatch(std::shared_ptr<vda5050pp::events::QueryAcceptZoneSet> data,
                bool synchronous = false) noexcept(true);

  ScopedQueryEventSubscriber getScopedQueryEventSubscriber() noexcept(true);
};

}  // namespace vda5050pp::core

#endif  // PRIVATE_VDA5050_2B_2B_CORE_QUERY_EVENT_MANAGER_H_
