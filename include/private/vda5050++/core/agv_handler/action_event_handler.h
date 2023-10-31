//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_AGV_HANDLER_ACTION_EVENT_HANDLER_H_
#define VDA5050_2B_2B_CORE_AGV_HANDLER_ACTION_EVENT_HANDLER_H_

#include <map>
#include <memory>
#include <optional>

#include "vda5050++/core/action_event_manager.h"
#include "vda5050++/core/agv_handler/action_state.h"
#include "vda5050++/core/module.h"
#include "vda5050++/handler/action_state.h"
#include "vda5050++/handler/base_action_handler.h"

namespace vda5050pp::core::agv_handler {

class ActionEventHandler : public Module {
private:
  std::optional<vda5050pp::core::ScopedActionEventSubscriber> subscriber_;

  struct ActionStore {
    std::shared_ptr<const vda5050::Action> action;
    std::shared_ptr<vda5050pp::core::agv_handler::ActionState> action_state;
    std::shared_ptr<vda5050pp::handler::BaseActionHandler> action_handler;
    std::shared_ptr<vda5050pp::handler::ActionCallbacks> action_callbacks;
    std::shared_ptr<std::map<std::string, vda5050pp::handler::ParameterValue, std::less<>>>
        action_parameters;
  };

  std::map<std::string, ActionStore, std::less<>> handled_actions_;

  void handleActionListEvent(std::shared_ptr<vda5050pp::events::ActionList> data) const
      noexcept(false);
  void handleValidateEvent(std::shared_ptr<vda5050pp::events::ActionValidate> data) noexcept(false);
  void handlePrepareEvent(std::shared_ptr<vda5050pp::events::ActionPrepare> data) noexcept(false);
  void handleCancelEvent(std::shared_ptr<vda5050pp::events::ActionCancel> data) noexcept(false);
  void handlePauseEvent(std::shared_ptr<vda5050pp::events::ActionPause> data) noexcept(false);
  void handleResumeEvent(std::shared_ptr<vda5050pp::events::ActionResume> data) noexcept(false);
  void handleStartEvent(std::shared_ptr<vda5050pp::events::ActionStart> data) noexcept(false);
  void handleForgetEvent(std::shared_ptr<vda5050pp::events::ActionForget> data) noexcept(false);

  std::optional<std::reference_wrapper<ActionStore>> tryFindHandledAction(
      std::string_view action_id);
  bool tryRemoveActionStore(std::string_view action_id);

public:
  void initialize(vda5050pp::core::Instance &instance) noexcept(false) override;
  void deinitialize(vda5050pp::core::Instance &instance) noexcept(false) override;
  std::string_view describe() const override;
};

}  // namespace vda5050pp::core::agv_handler

#endif  // VDA5050_2B_2B_CORE_AGV_HANDLER_ACTION_EVENT_HANDLER_H_
