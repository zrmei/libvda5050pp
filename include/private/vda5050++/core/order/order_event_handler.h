//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_ORDER_ORDER_EVENT_HANDLER_H_
#define VDA5050_2B_2B_CORE_ORDER_ORDER_EVENT_HANDLER_H_

#include "vda5050++/core/action_event_manager.h"
#include "vda5050++/core/events/interpreter_event.h"
#include "vda5050++/core/generic_event_manager.h"
#include "vda5050++/core/module.h"
#include "vda5050++/core/navigation_event_manager.h"
#include "vda5050++/core/order/scheduler.h"

namespace vda5050pp::core::order {

class OrderEventHandler : public vda5050pp::core::Module {
private:
  std::optional<vda5050pp::core::order::Scheduler> scheduler_;

  std::optional<GenericEventManager<vda5050pp::core::events::InterpreterEvent>::ScopedSubscriber>
      interpreter_subscriber_;
  std::optional<vda5050pp::core::ScopedActionStatusSubscriber> action_event_subscriber_;
  std::optional<vda5050pp::core::ScopedNavigationStatusSubscriber> navigation_event_subscriber_;

  void handleYieldNormal(std::shared_ptr<vda5050pp::core::events::InterpreterEvent> evt);
  void handleYieldInstantActionGroup(
      std::shared_ptr<vda5050pp::core::events::YieldInstantActionGroup> evt);
  void handleYieldNewAction(std::shared_ptr<vda5050pp::core::events::YieldNewAction> evt) const
      noexcept(false);

  void handleInterpreterDone(std::shared_ptr<vda5050pp::core::events::InterpreterDone> evt);

  void handleActionFailed(std::shared_ptr<vda5050pp::events::ActionStatusFailed> evt);
  void handleActionFinished(std::shared_ptr<vda5050pp::events::ActionStatusFinished> evt);
  void handleActionInitializing(std::shared_ptr<vda5050pp::events::ActionStatusInitializing> evt);
  void handleActionPaused(std::shared_ptr<vda5050pp::events::ActionStatusPaused> evt);
  void handleActionRunning(std::shared_ptr<vda5050pp::events::ActionStatusRunning> evt);

  void handleNavigationControl(std::shared_ptr<vda5050pp::events::NavigationStatusControl> evt);
  void handleNavigationNode(std::shared_ptr<vda5050pp::events::NavigationStatusNodeReached> evt);

  void handleOrderControl(
      std::shared_ptr<vda5050pp::core::events::InterpreterOrderControl> evt) noexcept(false);

public:
  void initialize(vda5050pp::core::Instance &instance) override;
  void deinitialize(vda5050pp::core::Instance &instance) override;
  std::string_view describe() const override;
};
}  // namespace vda5050pp::core::order

#endif  // VDA5050_2B_2B_CORE_ORDER_ORDER_EVENT_HANDLER_H_
