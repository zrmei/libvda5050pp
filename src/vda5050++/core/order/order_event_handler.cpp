//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/order/order_event_handler.h"

#include "vda5050++/core/common/exception.h"

using namespace vda5050pp::core::order;

void OrderEventHandler::handleYieldNormal(
    std::shared_ptr<vda5050pp::core::events::InterpreterEvent> evt) {
  if (evt == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT(""));
  }

  try {
    this->scheduler_->enqueue(evt);
  } catch (const vda5050pp::VDA5050PPError &e) {
    getOrderLogger()->error("Scheduler threw an exception: {}", e.dump());
    // TODO: global error state?
  }
}

void OrderEventHandler::handleYieldInstantActionGroup(
    std::shared_ptr<vda5050pp::core::events::YieldInstantActionGroup> evt) {
  if (evt == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT(""));
  }

  try {
    this->scheduler_->enqueueInterruptActions(evt);
  } catch (const vda5050pp::VDA5050PPError &e) {
    getOrderLogger()->error("Scheduler threw an exception: {}", e.dump());
    // TODO: global error state?
  }
}

void OrderEventHandler::handleYieldNewAction(
    std::shared_ptr<vda5050pp::core::events::YieldNewAction> evt) const noexcept(false) {
  if (evt == nullptr || evt->action == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("YieldNewAction is empty"));
  }

  // Prepare the action
  auto prepare_evt = std::make_shared<vda5050pp::events::ActionPrepare>();
  prepare_evt->action = evt->action;
  Instance::ref().getActionEventManager().dispatch(prepare_evt);
}

void OrderEventHandler::handleInterpreterDone(
    std::shared_ptr<vda5050pp::core::events::InterpreterDone> evt) {
  if (evt == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT(""));
  }

  try {
    this->scheduler_->update();
  } catch (const vda5050pp::VDA5050PPError &e) {
    getOrderLogger()->error("Scheduler threw an exception: {}", e.dump());
    // TODO: global error state?
  }
}

void OrderEventHandler::handleActionFailed(
    std::shared_ptr<vda5050pp::events::ActionStatusFailed> evt) {
  if (evt == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT(""));
  }

  try {
    this->scheduler_->actionTransition(evt->action_id, ActionTransition::isFailed());
  } catch (const vda5050pp::VDA5050PPError &e) {
    getOrderLogger()->error("Scheduler threw an exception: {}", e.dump());
    // TODO: global error state?
  }
}
void OrderEventHandler::handleActionFinished(
    std::shared_ptr<vda5050pp::events::ActionStatusFinished> evt) {
  if (evt == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT(""));
  }

  try {
    this->scheduler_->actionTransition(evt->action_id,
                                       ActionTransition::isFinished(evt->action_result));
  } catch (const vda5050pp::VDA5050PPError &e) {
    getOrderLogger()->error("Scheduler threw an exception: {}", e.dump());
    // TODO: global error state?
  }
}
void OrderEventHandler::handleActionInitializing(
    std::shared_ptr<vda5050pp::events::ActionStatusInitializing> evt) {
  if (evt == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT(""));
  }

  try {
    this->scheduler_->actionTransition(evt->action_id, ActionTransition::isInitializing());
  } catch (const vda5050pp::VDA5050PPError &e) {
    getOrderLogger()->error("Scheduler threw an exception: {}", e.dump());
    // TODO: global error state?
  }
}
void OrderEventHandler::handleActionPaused(
    std::shared_ptr<vda5050pp::events::ActionStatusPaused> evt) {
  if (evt == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT(""));
  }

  try {
    this->scheduler_->actionTransition(evt->action_id, ActionTransition::isPaused());
  } catch (const vda5050pp::VDA5050PPError &e) {
    getOrderLogger()->error("Scheduler threw an exception: {}", e.dump());
    // TODO: global error state?
  }
}
void OrderEventHandler::handleActionRunning(
    std::shared_ptr<vda5050pp::events::ActionStatusRunning> evt) {
  if (evt == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT(""));
  }

  try {
    this->scheduler_->actionTransition(evt->action_id, ActionTransition::isRunning());
  } catch (const vda5050pp::VDA5050PPError &e) {
    getOrderLogger()->error("Scheduler threw an exception: {}", e.dump());
    // TODO: global error state?
  }
}

void OrderEventHandler::handleNavigationControl(
    std::shared_ptr<vda5050pp::events::NavigationStatusControl> evt) {
  if (evt == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT(""));
  }

  std::optional<NavigationTransition> transition;
  switch (evt->type) {
    case vda5050pp::events::NavigationStatusControlType::k_failed:
      transition = NavigationTransition::isFailed();
      break;
    case vda5050pp::events::NavigationStatusControlType::k_paused:
      transition = NavigationTransition::isPaused();
      break;
    case vda5050pp::events::NavigationStatusControlType::k_resumed:
      transition = NavigationTransition::isResumed();
      break;
    default:
      throw vda5050pp::VDA5050PPInvalidArgument(
          MK_EX_CONTEXT(fmt::format("Unknown NavigationStatusControlType ({})", (int)evt->type)));
  }

  try {
    this->scheduler_->navigationTransition(*transition);
  } catch (const vda5050pp::VDA5050PPError &e) {
    getOrderLogger()->error("Scheduler threw an exception: {}", e.dump());
    // TODO: global error state?
  }
}
void OrderEventHandler::handleNavigationNode(
    std::shared_ptr<vda5050pp::events::NavigationStatusNodeReached> evt) {
  if (evt == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT(""));
  }

  if (!evt->node_seq_id.has_value()) {
    getOrderLogger()->debug("handleNavigationNode(): node_seq_id is empty -> skip");
    return;
  }

  try {
    this->scheduler_->navigationTransition(NavigationTransition::toSeqId(*evt->node_seq_id));
  } catch (const vda5050pp::VDA5050PPError &e) {
    getOrderLogger()->error("Scheduler threw an exception: {}", e.dump());
    // TODO: global error state?
  }
}

template <typename QueryType> inline vda5050pp::events::QueryPauseResumeResult queryPauseResume() {
  using namespace std::chrono_literals;

  auto evt = std::make_shared<QueryType>();
  auto future = evt->getFuture();

  vda5050pp::core::Instance::ref().getQueryEventManager().dispatch(evt, true);

  if (future.wait_for(0s) == std::future_status::timeout) {
    throw vda5050pp::VDA5050PPSynchronizedEventTimedOut(MK_FN_EX_CONTEXT("QueryEvent timed out."));
  }

  return future.get();
}

inline void dispatchErrors(const std::list<vda5050::Error> &errors) {
  for (const auto &e : errors) {
    auto evt = std::make_shared<vda5050pp::events::ErrorAdd>();
    evt->error = e;
    vda5050pp::core::Instance::ref().getStatusEventManager().dispatch(evt, true);
  }
}

inline void setFailed(std::string_view action_id) {
  auto evt = std::make_shared<vda5050pp::core::events::OrderActionStatusChanged>();
  evt->action_id = action_id;
  evt->action_status = vda5050::ActionStatus::FAILED;
  vda5050pp::core::Instance::ref().getOrderEventManager().synchronousDispatch(evt);
}

void OrderEventHandler::handleOrderControl(
    std::shared_ptr<vda5050pp::core::events::InterpreterOrderControl> evt) noexcept(false) {
  if (evt == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT(""));
  }

  using Status = vda5050pp::core::events::InterpreterOrderControl::Status;
  switch (evt->status) {
    case Status::k_cancel:
      this->scheduler_->cancel();
      break;
    case Status::k_pause: {
      auto result = queryPauseResume<vda5050pp::events::QueryPauseable>();
      if (!result.errors.empty()) {
        dispatchErrors(result.errors);
        setFailed(evt->associated_action->actionId);
        getOrderLogger()->info("QueryPauseable returned errors. Scheduler won't pause");
        return;
      }
      this->scheduler_->pause();
      break;
    }
    case Status::k_resume: {
      auto result = queryPauseResume<vda5050pp::events::QueryResumable>();
      if (!result.errors.empty()) {
        dispatchErrors(result.errors);
        setFailed(evt->associated_action->actionId);
        getOrderLogger()->info("QueryResumable returned errors. Scheduler won't resume");
        return;
      }
      this->scheduler_->resume();
      break;
    }
    default:
      throw vda5050pp::VDA5050PPInvalidArgument(
          MK_EX_CONTEXT(fmt::format("Invalid Status {}", int(evt->status))));
  }
}

void OrderEventHandler::initialize(vda5050pp::core::Instance &instance) {
  this->scheduler_.emplace();

  this->action_event_subscriber_ =
      instance.getActionStatusManager().getScopedActionStatusSubscriber();
  this->navigation_event_subscriber_ =
      instance.getNavigationStatusManager().getScopedNavigationStatusSubscriber();
  this->interpreter_subscriber_ = instance.getInterpreterEventManager().getScopedSubscriber();

  this->interpreter_subscriber_->subscribe<vda5050pp::core::events::YieldActionGroupEvent>(
      std::bind(std::mem_fn(&OrderEventHandler::handleYieldNormal), this, std::placeholders::_1));
  this->interpreter_subscriber_->subscribe<vda5050pp::core::events::YieldNavigationStepEvent>(
      std::bind(std::mem_fn(&OrderEventHandler::handleYieldNormal), this, std::placeholders::_1));
  this->interpreter_subscriber_->subscribe<vda5050pp::core::events::YieldInstantActionGroup>(
      std::bind(std::mem_fn(&OrderEventHandler::handleYieldInstantActionGroup), this,
                std::placeholders::_1));
  this->interpreter_subscriber_->subscribe<vda5050pp::core::events::YieldNewAction>(std::bind(
      std::mem_fn(&OrderEventHandler::handleYieldNewAction), this, std::placeholders::_1));
  this->interpreter_subscriber_->subscribe<vda5050pp::core::events::InterpreterDone>(std::bind(
      std::mem_fn(&OrderEventHandler::handleInterpreterDone), this, std::placeholders::_1));
  this->interpreter_subscriber_->subscribe<vda5050pp::core::events::InterpreterOrderControl>(
      std::bind(std::mem_fn(&OrderEventHandler::handleOrderControl), this, std::placeholders::_1));

  this->action_event_subscriber_->subscribe(
      std::bind(std::mem_fn(&OrderEventHandler::handleActionFailed), this, std::placeholders::_1));
  this->action_event_subscriber_->subscribe(std::bind(
      std::mem_fn(&OrderEventHandler::handleActionFinished), this, std::placeholders::_1));
  this->action_event_subscriber_->subscribe(std::bind(
      std::mem_fn(&OrderEventHandler::handleActionInitializing), this, std::placeholders::_1));
  this->action_event_subscriber_->subscribe(
      std::bind(std::mem_fn(&OrderEventHandler::handleActionPaused), this, std::placeholders::_1));
  this->action_event_subscriber_->subscribe(
      std::bind(std::mem_fn(&OrderEventHandler::handleActionRunning), this, std::placeholders::_1));

  this->navigation_event_subscriber_->subscribe(std::bind(
      std::mem_fn(&OrderEventHandler::handleNavigationControl), this, std::placeholders::_1));
  this->navigation_event_subscriber_->subscribe(std::bind(
      std::mem_fn(&OrderEventHandler::handleNavigationNode), this, std::placeholders::_1));
}

void OrderEventHandler::deinitialize(vda5050pp::core::Instance &) {
  this->scheduler_.reset();
  this->interpreter_subscriber_.reset();
  this->action_event_subscriber_.reset();
  this->navigation_event_subscriber_.reset();
}

std::string_view OrderEventHandler::describe() const { return "OrderEventHandler"; }