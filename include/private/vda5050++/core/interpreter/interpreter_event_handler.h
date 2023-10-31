//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_INTERPRETER_INTERPRETER_EVENT_HANDLER_H_
#define VDA5050_2B_2B_CORE_INTERPRETER_INTERPRETER_EVENT_HANDLER_H_

#include <map>
#include <memory>
#include <optional>
#include <string>

#include "vda5050++/core/events/event_control_blocks.h"
#include "vda5050++/core/module.h"

namespace vda5050pp::core::interpreter {

class InterpreterEventHandler : public vda5050pp::core::Module {
private:
  std::optional<
      vda5050pp::core::GenericEventManager<vda5050pp::core::events::MessageEvent>::ScopedSubscriber>
      message_subscriber_;
  std::optional<vda5050pp::core::ScopedActionEventSubscriber> action_subscriber_;
  std::optional<vda5050pp::core::GenericEventManager<
      vda5050pp::core::events::FactsheetEvent>::ScopedSubscriber>
      factsheet_subscriber_;

  std::map<std::string, std::shared_ptr<vda5050pp::core::events::EventControlBlock>, std::less<>>
      active_control_blocks_;

  void handleValidInstantActions(
      std::shared_ptr<vda5050pp::core::events::ValidInstantActionMessageEvent>
          data) noexcept(false);

  void handleValidOrder(std::shared_ptr<vda5050pp::core::events::ValidOrderMessageEvent> data) const
      noexcept(false);

  void handleActionValidateEvent(std::shared_ptr<vda5050pp::events::ActionValidate> data) const
      noexcept(false);

  void handleFactsheetControlActionListEvent(
      std::shared_ptr<vda5050pp::core::events::FactsheetControlActionListEvent> data) const
      noexcept(false);

public:
  void initialize(vda5050pp::core::Instance &instance) override;
  void deinitialize(vda5050pp::core::Instance &instance) override;
  std::string_view describe() const override;
};

}  // namespace vda5050pp::core::interpreter

#endif  // VDA5050_2B_2B_CORE_INTERPRETER_INTERPRETER_EVENT_HANDLER_H_
