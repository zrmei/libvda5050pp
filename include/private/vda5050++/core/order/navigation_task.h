//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_ORDER_NAVIGATION_TASK_H_
#define VDA5050_2B_2B_CORE_ORDER_NAVIGATION_TASK_H_

#include <spdlog/fmt/fmt.h>
#include <vda5050/Edge.h>
#include <vda5050/Node.h>

#include <memory>
#include <string>

namespace vda5050pp::core::order {

class NavigationTransition {
public:
  enum class Type {
    k_do_start,
    k_is_resumed,
    k_do_pause,
    k_is_paused,
    k_do_resume,
    k_to_seq_id,
    k_do_cancel,
    k_is_failed,
  };

private:
  Type type_;
  decltype(vda5050::Node::sequenceId) seq_id_;

  NavigationTransition(Type type, decltype(vda5050::Node::sequenceId) seq_id);

public:
  static NavigationTransition doStart();
  static NavigationTransition isResumed();
  static NavigationTransition doPause();
  static NavigationTransition isPaused();
  static NavigationTransition doResume();
  static NavigationTransition toSeqId(decltype(vda5050::Node::sequenceId) seq_id);
  static NavigationTransition doCancel();
  static NavigationTransition isFailed();

  Type type() const;
  decltype(vda5050::Node::sequenceId) seqId() const;
  std::string describe() const;
};

class NavigationTask;

class NavigationState {
private:
  NavigationTask &task_;

protected:
  NavigationTask &task();

public:
  explicit NavigationState(NavigationTask &task);
  virtual ~NavigationState() = default;
  [[nodiscard]] virtual std::unique_ptr<NavigationState> transfer(
      NavigationTransition transition) = 0;
  virtual void effect() = 0;
  virtual bool isTerminal() = 0;
  virtual bool isPaused() = 0;
  virtual std::string describe() = 0;
};

template <bool terminal, bool paused> class NavigationStateT : public NavigationState {
public:
  explicit NavigationStateT(NavigationTask &task) : NavigationState(task) {}
  ~NavigationStateT() override = default;
  bool isTerminal() override { return terminal; }
  bool isPaused() override { return paused; }
};

class NavigationWaiting : public NavigationStateT<false, false> {
public:
  explicit NavigationWaiting(NavigationTask &task);
  [[nodiscard]] std::unique_ptr<NavigationState> transfer(NavigationTransition transition) override;
  void effect() override;
  std::string describe() override;
};

class NavigationFirstInProgress : public NavigationStateT<false, false> {
public:
  explicit NavigationFirstInProgress(NavigationTask &task);
  [[nodiscard]] std::unique_ptr<NavigationState> transfer(NavigationTransition transition) override;
  void effect() override;
  std::string describe() override;
};

class NavigationInProgress : public NavigationStateT<false, false> {
public:
  explicit NavigationInProgress(NavigationTask &task);
  [[nodiscard]] std::unique_ptr<NavigationState> transfer(NavigationTransition transition) override;
  void effect() override;
  std::string describe() override;
};

class NavigationPausing : public NavigationStateT<false, false> {
public:
  explicit NavigationPausing(NavigationTask &task);
  [[nodiscard]] std::unique_ptr<NavigationState> transfer(NavigationTransition transition) override;
  void effect() override;
  std::string describe() override;
};

class NavigationPaused : public NavigationStateT<false, true> {
public:
  explicit NavigationPaused(NavigationTask &task);
  [[nodiscard]] std::unique_ptr<NavigationState> transfer(NavigationTransition transition) override;
  void effect() override;
  std::string describe() override;
};

class NavigationResuming : public NavigationStateT<false, true> {
public:
  explicit NavigationResuming(NavigationTask &task);
  [[nodiscard]] std::unique_ptr<NavigationState> transfer(NavigationTransition transition) override;
  void effect() override;
  std::string describe() override;
};

class NavigationCanceling : public NavigationStateT<false, false> {
public:
  explicit NavigationCanceling(NavigationTask &task);
  [[nodiscard]] std::unique_ptr<NavigationState> transfer(NavigationTransition transition) override;
  void effect() override;
  std::string describe() override;
};

class NavigationFailed : public NavigationStateT<true, false> {
public:
  explicit NavigationFailed(NavigationTask &task);
  [[nodiscard]] std::unique_ptr<NavigationState> transfer(NavigationTransition transition) override;
  void effect() override;
  std::string describe() override;
};

class NavigationDone : public NavigationStateT<true, false> {
public:
  explicit NavigationDone(NavigationTask &task);
  [[nodiscard]] std::unique_ptr<NavigationState> transfer(NavigationTransition transition) override;
  void effect() override;
  std::string describe() override;
};

class NavigationTask {
public:
  friend class NavigationWaiting;
  friend class NavigationFirstInProgress;
  friend class NavigationInProgress;
  friend class NavigationPausing;
  friend class NavigationPaused;
  friend class NavigationResuming;
  friend class NavigationCanceling;
  friend class NavigationFailed;
  friend class NavigationDone;

private:
  std::unique_ptr<NavigationState> state_;
  std::optional<std::pair<decltype(vda5050::Node::sequenceId), decltype(vda5050::Node::sequenceId)>>
      segment_;
  std::shared_ptr<const vda5050::Node> goal_;
  std::shared_ptr<const vda5050::Edge> via_edge_;

protected:
  std::shared_ptr<const vda5050::Node> getGoal() const;
  std::shared_ptr<const vda5050::Edge> getViaEdge() const;
  std::optional<std::pair<decltype(vda5050::Node::sequenceId), decltype(vda5050::Node::sequenceId)>>
  getSegment();

public:
  NavigationTask(std::shared_ptr<const vda5050::Node> goal,
                 std::shared_ptr<const vda5050::Edge> via_edge);
  void setSegment(
      std::pair<decltype(vda5050::Node::sequenceId), decltype(vda5050::Node::sequenceId)> segment);
  void transition(NavigationTransition transition);

  bool isTerminal() const;
  bool isPaused() const;

  std::string describe() const;
};

}  // namespace vda5050pp::core::order

template <>
struct fmt::formatter<vda5050pp::core::order::NavigationTransition>
    : fmt::formatter<fmt::string_view> {
  template <typename FormatContext>
  auto format(vda5050pp::core::order::NavigationTransition t, FormatContext &ctx) const {
    return fmt::formatter<fmt::string_view>::format(t.describe(), ctx);
  }
};

#endif  // VDA5050_2B_2B_CORE_ORDER_NAVIGATION_TASK_H_
