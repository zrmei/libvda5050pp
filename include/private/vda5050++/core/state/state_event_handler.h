//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_STATE_STATE_EVENT_HANDLER_H_
#define VDA5050_2B_2B_CORE_STATE_STATE_EVENT_HANDLER_H_

#include <optional>

#include "vda5050++/core/events/interpreter_event.h"
#include "vda5050++/core/module.h"

namespace vda5050pp::core::state {

class StateEventHandler : public vda5050pp::core::Module {
private:
  std::optional<vda5050pp::core::GenericEventManager<
      vda5050pp::core::events::InterpreterEvent>::ScopedSubscriber>
      interpreter_subscriber_;

  std::optional<
      vda5050pp::core::GenericEventManager<vda5050pp::core::events::OrderEvent>::ScopedSubscriber>
      order_subscriber_;

  std::optional<vda5050pp::core::ScopedNavigationStatusSubscriber> navigation_subscriber_;

  std::optional<vda5050pp::core::ScopedStatusEventSubscriber> status_subscriber_;

  void handleGraphExtensionEvent(
      std::shared_ptr<vda5050pp::core::events::YieldGraphExtension> data) const noexcept(false);
  void handleGraphReplacementEvent(
      std::shared_ptr<vda5050pp::core::events::YieldGraphReplacement> data) const noexcept(false);
  void handleNewActionEvent(std::shared_ptr<vda5050pp::core::events::YieldNewAction> data) const
      noexcept(false);
  void handleClearActions(std::shared_ptr<vda5050pp::core::events::YieldClearActions> data) const
      noexcept(false);

  void handleOrderNewLastNodeId(
      std::shared_ptr<vda5050pp::core::events::OrderNewLastNodeId> data) const noexcept(false);
  void handleOrderActionStatusChanged(
      std::shared_ptr<vda5050pp::core::events::OrderActionStatusChanged> data) const
      noexcept(false);
  void handleOrderStatus(std::shared_ptr<vda5050pp::core::events::OrderStatus> data) const
      noexcept(false);

  void handleOrderClearAfterCancel(
      std::shared_ptr<vda5050pp::core::events::OrderClearAfterCancel> data) const;

  void handleNavigationStatusPosition(
      std::shared_ptr<vda5050pp::events::NavigationStatusPosition> data) const noexcept(false);
  void handleNavigationStatusVelocity(
      std::shared_ptr<vda5050pp::events::NavigationStatusVelocity> data) const noexcept(false);
  void handleNavigationStatusDistanceSinceLastNode(
      std::shared_ptr<vda5050pp::events::NavigationStatusDistanceSinceLastNode> data) const
      noexcept(false);
  void handleNavigationStatusDriving(
      std::shared_ptr<vda5050pp::events::NavigationStatusDriving> data) const noexcept(false);
  void handleNavigationStatusNodeReached(
      std::shared_ptr<vda5050pp::events::NavigationStatusNodeReached> data) const noexcept(false);

  void handleLoadAdd(std::shared_ptr<vda5050pp::events::LoadAdd> data) const noexcept(false);

  void handleLoadRemove(std::shared_ptr<vda5050pp::events::LoadRemove> data) const noexcept(false);

  void handleLoadsGet(std::shared_ptr<vda5050pp::events::LoadsGet> data) const noexcept(false);

  void handleLoadsAlter(std::shared_ptr<vda5050pp::events::LoadsAlter> data) const noexcept(false);

  void handleOperatingModeSet(std::shared_ptr<vda5050pp::events::OperatingModeSet> data) const
      noexcept(false);

  void handleOperatingModeGet(std::shared_ptr<vda5050pp::events::OperatingModeGet> data) const
      noexcept(false);

  void handleOperatingModeAlter(std::shared_ptr<vda5050pp::events::OperatingModeAlter> data) const
      noexcept(false);

  void handleBatteryStateSet(std::shared_ptr<vda5050pp::events::BatteryStateSet> data) const
      noexcept(false);

  void handleBatteryStateGet(std::shared_ptr<vda5050pp::events::BatteryStateGet> data) const
      noexcept(false);
  void handleBatteryStateAlter(std::shared_ptr<vda5050pp::events::BatteryStateAlter> data) const
      noexcept(false);

  void handleRequestNewBase(std::shared_ptr<vda5050pp::events::RequestNewBase> data) const
      noexcept(false);
  void handleErrorAdd(std::shared_ptr<vda5050pp::events::ErrorAdd> data) const noexcept(false);

  void handleErrorsAlter(std::shared_ptr<vda5050pp::events::ErrorsAlter> data) const
      noexcept(false);

  void handleInfoAdd(std::shared_ptr<vda5050pp::events::InfoAdd> data) const noexcept(false);

  void handleInfosAlter(std::shared_ptr<vda5050pp::events::InfosAlter> data) const noexcept(false);

public:
  void initialize(vda5050pp::core::Instance &instance) override;
  void deinitialize(vda5050pp::core::Instance &instance) override;
  std::string_view describe() const override;
  std::shared_ptr<vda5050pp::config::ModuleSubConfig> generateSubConfig() const override;
};

}  // namespace vda5050pp::core::state

#endif  // VDA5050_2B_2B_CORE_STATE_STATE_EVENT_HANDLER_H_
