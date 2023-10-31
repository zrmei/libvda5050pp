// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_EVENTS_EVENT_CONTROL_BLOCKS_H_
#define VDA5050_2B_2B_CORE_EVENTS_EVENT_CONTROL_BLOCKS_H_

#include <functional>
#include <list>
#include <memory>

#include "vda5050++/core/generic_event_manager.h"
#include "vda5050++/events/event_type.h"

namespace vda5050pp::core::events {

class EventControlBlock {
private:
  std::function<void()> teardown_;

public:
  virtual ~EventControlBlock() = default;

  template <typename Teardown> void setTeardown(Teardown &&teardown) {
    this->teardown_ = std::forward<Teardown>(teardown);
  }

  virtual void enable() = 0;
  virtual void teardown();
};

class EventControlChain : public EventControlBlock {
private:
  std::list<std::shared_ptr<EventControlBlock>> blocks_;

protected:
  void next() noexcept(false);

public:
  void enable() noexcept(false) override;
  void add(std::shared_ptr<EventControlBlock> block) noexcept(false);
};

class EventControlAlternative : public EventControlBlock {
private:
  std::vector<std::shared_ptr<EventControlBlock>> blocks_;

public:
  void enable() noexcept(false) override;
  void add(std::shared_ptr<EventControlBlock> block) noexcept(false);
};

template <typename EventT> class EventLatch : public EventControlBlock {
public:
  using Subscriber =
      typename vda5050pp::core::GenericEventManager<typename EventT::EventBase>::ScopedSubscriber;

private:
  Subscriber sub_;

public:
  virtual bool predicate(std::shared_ptr<EventT> evt) = 0;
  virtual void done() = 0;

  explicit EventLatch(Subscriber &&sub) : sub_(std::move(sub)) {}

  void enable() override {
    this->sub_.template subscribe<EventT>([this](auto evt) {
      if (this->predicate(evt)) {
        this->done();
        this->teardown();
      }
    });
  }
};

template <typename EventT> class LambdaEventLatch : public EventLatch<EventT> {
private:
  std::function<bool(std::shared_ptr<EventT>)> pred_;
  std::function<void()> done_;

public:
  bool predicate(std::shared_ptr<EventT> evt) override { return this->pred_(evt); }
  void done() override { this->done_(); }

  template <typename PredFn, typename DoneFn>
  LambdaEventLatch(typename EventLatch<EventT>::Subscriber &&sub, PredFn &&pred, DoneFn &&done)
      : EventLatch<EventT>(std::move(sub)),
        pred_(std::forward<PredFn>(pred)),
        done_(std::forward<DoneFn>(done)) {}
};

class FunctionBlock : public EventControlBlock {
private:
  std::function<void()> fn_;

public:
  template <typename Fn> void setFunction(Fn &&fn) { this->fn_ = std::forward<Fn>(fn); }

  void enable() override;
};

}  // namespace vda5050pp::core::events

#endif  // VDA5050_2B_2B_CORE_EVENTS_EVENT_CONTROL_BLOCKS_H_
