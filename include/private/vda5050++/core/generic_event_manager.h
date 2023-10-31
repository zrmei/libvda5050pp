//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PRIVATE_VDA5050_2B_2B_CORE_GENERIC_EVENT_MANAGER_H_
#define PRIVATE_VDA5050_2B_2B_CORE_GENERIC_EVENT_MANAGER_H_

#include <eventpp/eventqueue.h>
#include <eventpp/utilities/argumentadapter.h>
#include <eventpp/utilities/scopedremover.h>

#include "vda5050++/config/event_manager_options.h"
#include "vda5050++/core/common/formatters.h"
#include "vda5050++/core/common/scoped_thread.h"
#include "vda5050++/core/common/type_traits.h"
#include "vda5050++/core/logger.h"
#include "vda5050++/events/event_type.h"

namespace vda5050pp::core {

/// @brief The GenericEventManager is a threaded dispatch/subscribe wrapper around the eventpp queue
/// @tparam EventType the managed event type (must derive from vda5050pp::events::Event)
template <typename EventType> class GenericEventManager {
private:
  static_assert(std::is_base_of_v<vda5050pp::events::EventBase, EventType>,
                "EventType must be derived from vda5050pp::events::Event");

  using EventQueueType =
      eventpp::EventQueue<typename EventType::EventIdType, void(std::shared_ptr<EventType>)>;

  EventQueueType event_queue_;
  const vda5050pp::config::EventManagerOptions &opts_;
  vda5050pp::core::common::ScopedThread<void()> thread_;

  void threadTask(vda5050pp::core::common::StopToken tkn) {
    if (this->opts_.synchronous_event_dispatch) {
      // This event loop is not needed
      return;
    }

    using namespace std::chrono_literals;
    try {
      while (!tkn.stopRequested()) {
        if (this->event_queue_.processUntil([&tkn] { return tkn.stopRequested(); })) {
          // no pause
        } else {
          std::this_thread::sleep_for(10ms);
        }
      }
    } catch (const vda5050pp::VDA5050PPError &err) {
      static const std::type_info &info = typeid(GenericEventManager<EventType>);
      getEventsLogger()->error("{} caught an exception, while processing events:\n {}",
                               common::demangle(info.name()), err);
      // TODO: Handle critical exceptions
    }
  }

public:
  /// @brief Construct a new GenericEventManager. Manages one thread during lifetime
  explicit GenericEventManager(const vda5050pp::config::EventManagerOptions &opts)
      : opts_(opts),
        thread_(std::bind(std::mem_fn(&GenericEventManager<EventType>::threadTask), this,
                          std::placeholders::_1)) {}

  /// @brief The ScopedSubscriber is an RAII base subscriber, which releases callbacks upon
  /// deconstruction
  class ScopedSubscriber {
  private:
    eventpp::ScopedRemover<EventQueueType> remover_;

    friend class GenericEventManager<EventType>;

    explicit ScopedSubscriber(EventQueueType &event_queue) : remover_(event_queue) {}

  public:
    /// @brief subscribe to a specific event
    /// @tparam SpecializedEvent the Event (must be derived from EventType template parameter)
    /// @param callback the callback function
    template <typename SpecializedEvent>
    void subscribe(std::function<void(std::shared_ptr<SpecializedEvent>)> &&callback) {
      static_assert(
          std::is_base_of_v<EventType, SpecializedEvent>,
          "The given callback does not accept an event deriving from the given EventType");
      this->remover_.appendListener(
          SpecializedEvent::id(),
          eventpp::argumentAdapter<void(std::shared_ptr<SpecializedEvent>)>(std::move(callback)));
    }
  };

  /// @brief Enqueue an event into the underlying eventpp queue (async, depending on this->opts_)
  /// @param event the event to dispatch
  void dispatch(std::shared_ptr<EventType> event) {
    getEventsLogger()->debug("Dispatching {} event with specialized ID={}",
                             common::demangle(typeid(*event).name()), int(event->getId()));
    if (this->opts_.synchronous_event_dispatch) {
      this->event_queue_.dispatch(event->getId(), event);
    } else {
      this->event_queue_.enqueue(event->getId(), event);
    }
  }

  /// @brief Synchronously dispatch an event (with the calling thread)
  /// @param event the event to dispatch
  void synchronousDispatch(std::shared_ptr<EventType> event) {
    getEventsLogger()->debug("Synchronously dispatching {} event with specialized ID={}",
                             common::demangle(typeid(*event).name()), int(event->getId()));
    this->event_queue_.dispatch(event->getId(), event);
  }

  /// @brief Get a new ScopedSubscriber associated with this Manager
  /// @return the new ScopedSubscriber
  ScopedSubscriber getScopedSubscriber() { return ScopedSubscriber(this->event_queue_); }
};

}  // namespace vda5050pp::core

#endif  // PRIVATE_VDA5050_2B_2B_CORE_GENERIC_EVENT_MANAGER_H_
