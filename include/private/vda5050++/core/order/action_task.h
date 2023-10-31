//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_ORDER_ACTION_TASK_H_
#define VDA5050_2B_2B_CORE_ORDER_ACTION_TASK_H_

#include <fmt/format.h>
#include <vda5050/Action.h>

#include <memory>
#include <string>

#include "vda5050++/core/instance.h"

namespace vda5050pp::core::order {

class ActionTransition {
public:
  enum class Type {
    k_do_start,
    k_is_initializing,
    k_is_running,
    k_do_pause,
    k_is_paused,
    k_do_resume,
    k_is_failed,
    k_is_finished,
    k_do_cancel,
  };

private:
  Type type_;
  std::optional<std::string> result_;

protected:
  ActionTransition(Type t, std::optional<std::string_view> result) : type_(t), result_(result) {}

public:
  static ActionTransition doStart();
  static ActionTransition isInitializing();
  static ActionTransition isRunning();
  static ActionTransition doPause();
  static ActionTransition isPaused();
  static ActionTransition doResume();
  static ActionTransition isFailed();
  static ActionTransition isFinished(std::optional<std::string_view> result = std::nullopt);
  static ActionTransition doCancel();

  Type getType() const;
  std::optional<std::string_view> getResult() const;
};

class ActionTask;

class ActionState {
private:
  ActionTask &task_;

protected:
  ActionTask &task();

public:
  explicit ActionState(ActionTask &task);
  virtual ~ActionState() = default;
  [[nodiscard]] virtual std::unique_ptr<ActionState> transition(
      const ActionTransition &transition) = 0;
  virtual bool isTerminal() = 0;
  virtual bool isPaused() = 0;
  virtual void effect() = 0;
  virtual std::string describe() = 0;
};

template <bool terminal, bool paused> class ActionStateT : public ActionState {
public:
  explicit ActionStateT(ActionTask &task) : ActionState(task) {}
  ~ActionStateT() override = default;
  bool isTerminal() override { return terminal; }
  bool isPaused() override { return paused; }
};

class ActionWaiting : public ActionStateT<false, false> {
public:
  explicit ActionWaiting(ActionTask &task);
  [[nodiscard]] std::unique_ptr<ActionState> transition(
      const ActionTransition &transition) override;
  void effect() override;
  std::string describe() override;
};

class ActionInitializing : public ActionStateT<false, false> {
public:
  explicit ActionInitializing(ActionTask &task);
  [[nodiscard]] std::unique_ptr<ActionState> transition(
      const ActionTransition &transition) override;
  void effect() override;
  std::string describe() override;
};

class ActionInitializingNoEffect : public ActionStateT<false, false> {
public:
  explicit ActionInitializingNoEffect(ActionTask &task);
  [[nodiscard]] std::unique_ptr<ActionState> transition(
      const ActionTransition &transition) override;
  void effect() override;
  std::string describe() override;
};

class ActionRunning : public ActionStateT<false, false> {
public:
  explicit ActionRunning(ActionTask &task);
  [[nodiscard]] std::unique_ptr<ActionState> transition(
      const ActionTransition &transition) override;
  void effect() override;
  std::string describe() override;
};

class ActionPausing : public ActionStateT<false, false> {
public:
  explicit ActionPausing(ActionTask &task);
  [[nodiscard]] std::unique_ptr<ActionState> transition(
      const ActionTransition &transition) override;
  void effect() override;
  std::string describe() override;
};

class ActionResuming : public ActionStateT<false, true> {
public:
  explicit ActionResuming(ActionTask &task);
  [[nodiscard]] std::unique_ptr<ActionState> transition(
      const ActionTransition &transition) override;
  void effect() override;
  std::string describe() override;
};

class ActionPaused : public ActionStateT<false, true> {
public:
  explicit ActionPaused(ActionTask &task);
  [[nodiscard]] std::unique_ptr<ActionState> transition(
      const ActionTransition &transition) override;
  void effect() override;
  std::string describe() override;
};

class ActionCanceling : public ActionStateT<false, false> {
public:
  explicit ActionCanceling(ActionTask &task);
  [[nodiscard]] std::unique_ptr<ActionState> transition(
      const ActionTransition &transition) override;
  void effect() override;
  std::string describe() override;
};

class ActionFailed : public ActionStateT<true, false> {
public:
  explicit ActionFailed(ActionTask &task);
  [[nodiscard]] std::unique_ptr<ActionState> transition(
      const ActionTransition &transition) override;
  void effect() override;
  std::string describe() override;
};

class ActionFinished : public ActionStateT<true, false> {
private:
  std::optional<std::string> result_;

public:
  explicit ActionFinished(ActionTask &task, std::optional<std::string_view> result);
  [[nodiscard]] std::unique_ptr<ActionState> transition(
      const ActionTransition &transition) override;
  void effect() override;
  std::string describe() override;
};

class ActionTask {
public:
  friend class ActionWaiting;
  friend class ActionInitializing;
  friend class ActionRunning;
  friend class ActionPausing;
  friend class ActionResuming;
  friend class ActionPaused;
  friend class ActionCanceling;
  friend class ActionFailed;
  friend class ActionFinished;

private:
  std::unique_ptr<ActionState> state_;
  std::shared_ptr<const vda5050::Action> action_;

public:
  explicit ActionTask(std::shared_ptr<const vda5050::Action> action);

  void transition(const ActionTransition &transition);

  bool isTerminal() const;
  bool isPaused() const;

  const vda5050::Action &getAction() const;

  std::string describe() const;
};

}  // namespace vda5050pp::core::order

template <>
struct fmt::formatter<vda5050pp::core::order::ActionTransition> : fmt::formatter<fmt::string_view> {
  template <typename FormatContext>
  auto format(const vda5050pp::core::order::ActionTransition &t, FormatContext &ctx) const {
    fmt::string_view name;
    switch (t.getType()) {
      case vda5050pp::core::order::ActionTransition::Type::k_do_start:
        name = "k_do_start";
        break;
      case vda5050pp::core::order::ActionTransition::Type::k_is_initializing:
        name = "k_is_initializing";
        break;
      case vda5050pp::core::order::ActionTransition::Type::k_is_running:
        name = "k_is_running";
        break;
      case vda5050pp::core::order::ActionTransition::Type::k_do_pause:
        name = "k_do_pause";
        break;
      case vda5050pp::core::order::ActionTransition::Type::k_is_paused:
        name = "k_is_paused";
        break;
      case vda5050pp::core::order::ActionTransition::Type::k_do_resume:
        name = "k_do_resume";
        break;
      case vda5050pp::core::order::ActionTransition::Type::k_is_failed:
        name = "k_is_failed";
        break;
      case vda5050pp::core::order::ActionTransition::Type::k_is_finished:
        name = "k_is_finished";
        break;
      case vda5050pp::core::order::ActionTransition::Type::k_do_cancel:
        name = "k_do_cancel";
        break;
      default:
        name = "unknown";
        break;
    }
    return fmt::formatter<fmt::string_view>::format(name, ctx);
  }
};

#endif  // VDA5050_2B_2B_CORE_ORDER_ACTION_TASK_H_
