//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_ORDER_SCHEDULER_H_
#define VDA5050_2B_2B_CORE_ORDER_SCHEDULER_H_

#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>

#include "vda5050++/core/order/action_task.h"
#include "vda5050++/core/order/navigation_task.h"

namespace vda5050pp::core::order {

class Scheduler;

enum class SchedulerStateType {
  k_idle,
  k_idle_paused,
  k_active,
  k_canceling,
  k_resuming,
  k_pausing,
  k_paused,
  k_failed,
  k_interrupting,
};

class SchedulerState {
private:
  Scheduler &scheduler_;

protected:
  Scheduler &scheduler();

public:
  explicit SchedulerState(Scheduler &scheduler);
  virtual ~SchedulerState() = default;
  [[nodiscard]] virtual std::pair<std::unique_ptr<SchedulerState>, bool> cancel() = 0;
  [[nodiscard]] virtual std::pair<std::unique_ptr<SchedulerState>, bool> pause() = 0;
  [[nodiscard]] virtual std::pair<std::unique_ptr<SchedulerState>, bool> resume() = 0;
  [[nodiscard]] virtual std::pair<std::unique_ptr<SchedulerState>, bool> interrupt() = 0;
  [[nodiscard]] virtual std::pair<std::unique_ptr<SchedulerState>, bool> update() = 0;
  virtual std::string describe() = 0;
  virtual SchedulerStateType getState() = 0;
};

template <SchedulerStateType type> class SchedulerStateID : public SchedulerState {
public:
  explicit SchedulerStateID(Scheduler &scheduler) : SchedulerState(scheduler) {}
  SchedulerStateType getState() override { return type; }
};

class SchedulerIdle : public SchedulerStateID<SchedulerStateType::k_idle> {
public:
  SchedulerIdle(Scheduler &scheduler, bool notify = false);
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> cancel() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> pause() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> resume() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> interrupt() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> update() override;
  std::string describe() override;
};

class SchedulerIdlePaused : public SchedulerStateID<SchedulerStateType::k_idle_paused> {
public:
  SchedulerIdlePaused(Scheduler &scheduler, bool notify = false);
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> cancel() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> pause() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> resume() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> interrupt() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> update() override;
  std::string describe() override;
};

class SchedulerActive : public SchedulerStateID<SchedulerStateType::k_active> {
public:
  SchedulerActive(Scheduler &scheduler, bool notify = false);
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> cancel() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> pause() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> resume() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> interrupt() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> update() override;
  std::string describe() override;
};

class SchedulerCanceling : public SchedulerStateID<SchedulerStateType::k_canceling> {
public:
  SchedulerCanceling(Scheduler &scheduler, bool notify = false);
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> cancel() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> pause() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> resume() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> interrupt() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> update() override;
  std::string describe() override;
};

class SchedulerResuming : public SchedulerStateID<SchedulerStateType::k_resuming> {
public:
  SchedulerResuming(Scheduler &scheduler, bool notify = false);
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> cancel() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> pause() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> resume() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> interrupt() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> update() override;
  std::string describe() override;
};

class SchedulerPausing : public SchedulerStateID<SchedulerStateType::k_pausing> {
public:
  SchedulerPausing(Scheduler &scheduler, bool notify = false);
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> cancel() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> pause() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> resume() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> interrupt() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> update() override;
  std::string describe() override;
};

class SchedulerPaused : public SchedulerStateID<SchedulerStateType::k_paused> {
public:
  SchedulerPaused(Scheduler &scheduler, bool notify = false);
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> cancel() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> pause() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> resume() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> interrupt() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> update() override;
  std::string describe() override;
};

class SchedulerFailed : public SchedulerStateID<SchedulerStateType::k_failed> {
public:
  SchedulerFailed(Scheduler &scheduler, bool notify = false);
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> cancel() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> pause() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> resume() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> interrupt() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> update() override;
  std::string describe() override;
};

class SchedulerInterrupting : public SchedulerStateID<SchedulerStateType::k_interrupting> {
public:
  SchedulerInterrupting(Scheduler &scheduler, bool notify = false);
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> cancel() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> pause() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> resume() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> interrupt() override;
  [[nodiscard]] std::pair<std::unique_ptr<SchedulerState>, bool> update() override;
  std::string describe() override;
};

class Scheduler {
public:
  friend class SchedulerIdle;
  friend class SchedulerIdlePaused;
  friend class SchedulerActive;
  friend class SchedulerResuming;
  friend class SchedulerPausing;
  friend class SchedulerPaused;
  friend class SchedulerFailed;
  friend class SchedulerCanceling;
  friend class SchedulerInterrupting;
  friend class SchedulerInterrupted;

private:
  using Lock = std::unique_lock<std::mutex>;

  std::mutex access_mutex_;
  std::unique_ptr<SchedulerState> state_;
  std::deque<std::shared_ptr<vda5050pp::core::events::InterpreterEvent>> rcv_evt_queue_;
  std::deque<std::shared_ptr<vda5050pp::core::events::YieldInstantActionGroup>>
      rcv_interrupt_queue_;
  vda5050::BlockingType current_action_blocking_type_ = vda5050::BlockingType::NONE;
  std::optional<std::pair<decltype(vda5050::Node::sequenceId), decltype(vda5050::Node::sequenceId)>>
      current_segment_;
  std::map<std::string, std::shared_ptr<ActionTask>, std::less<>> active_action_tasks_by_id_;
  std::map<std::string, std::shared_ptr<ActionTask>, std::less<>> running_action_tasks_by_id_;
  std::map<std::string, std::shared_ptr<ActionTask>, std::less<>> paused_action_tasks_by_id_;
  std::map<std::string, std::shared_ptr<ActionTask>, std::less<>>
      nav_interrupting_action_tasks_by_id_;
  std::shared_ptr<NavigationTask> navigation_task_;

  inline Lock ensureLock(std::optional<Lock> &&lock) {
    if (!lock.has_value() || !lock->owns_lock() || lock->mutex() != &this->access_mutex_) {
      return Lock(this->access_mutex_);
    }

    return std::move(lock.value());
  }

protected:
  std::map<std::string, std::shared_ptr<ActionTask>, std::less<>> &getActiveActionTasksById();
  std::map<std::string, std::shared_ptr<ActionTask>, std::less<>> &getRunningActionTasksById();
  std::map<std::string, std::shared_ptr<ActionTask>, std::less<>> &getPausedActionTasksById();
  std::map<std::string, std::shared_ptr<ActionTask>, std::less<>>
      &getNavInterruptingActionTasksById();
  const std::deque<std::shared_ptr<vda5050pp::core::events::YieldInstantActionGroup>>
      &getRcvInterruptQueue() const;
  std::shared_ptr<NavigationTask> &getNavigationTask();

  void clearQueues();
  void updateTasks();
  void updateTasksInterruptMapping();
  void updateFetchNext();
  void updateFetchNext(std::shared_ptr<vda5050pp::core::events::YieldActionGroupEvent> evt);
  void updateFetchNext(std::shared_ptr<vda5050pp::core::events::YieldNavigationStepEvent> evt);
  void doPatchSegment();
  void doInterrupt();
  void updateFetchNextInterrupt();

public:
  Scheduler();
  void cancel(std::optional<Lock> lock = std::nullopt);
  void pause(std::optional<Lock> lock = std::nullopt);
  void resume(std::optional<Lock> lock = std::nullopt);
  void update(std::optional<Lock> lock = std::nullopt);
  SchedulerStateType getState(std::optional<Lock> lock = std::nullopt);
  void actionTransition(std::string_view action_id, const ActionTransition &transition,
                        std::optional<Lock> lock = std::nullopt);
  void navigationTransition(NavigationTransition transition,
                            std::optional<Lock> lock = std::nullopt);
  void enqueueInterruptActions(
      std::shared_ptr<vda5050pp::core::events::YieldInstantActionGroup> evt,
      std::optional<Lock> lock = std::nullopt);
  void enqueue(std::shared_ptr<vda5050pp::core::events::InterpreterEvent> evt,
               std::optional<Lock> lock = std::nullopt);

  std::string describe() const;
};

}  // namespace vda5050pp::core::order

#endif  // VDA5050_2B_2B_CORE_ORDER_SCHEDULER_H_
