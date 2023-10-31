//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_MESSAGES_MESSAGE_EVENT_HANDLER_H_
#define VDA5050_2B_2B_CORE_MESSAGES_MESSAGE_EVENT_HANDLER_H_

#include <optional>

#include "vda5050++/core/module.h"

namespace vda5050pp::core::messages {

class MessageEventHandler : public vda5050pp::core::Module {
private:
  std::optional<GenericEventManager<vda5050pp::core::events::MessageEvent>::ScopedSubscriber>
      subscriber_;

  void handleOrderMessage(
      std::shared_ptr<const vda5050pp::core::events::ReceiveOrderMessageEvent> evt) const;

  void handleInstantActionsMessage(
      std::shared_ptr<const vda5050pp::core::events::ReceiveInstantActionMessageEvent> evt) const;

public:
  void initialize(vda5050pp::core::Instance &instance) override;
  void deinitialize(vda5050pp::core::Instance &instance) override;
  std::string_view describe() const override;
};

}  // namespace vda5050pp::core::messages

#endif  // VDA5050_2B_2B_CORE_MESSAGES_MESSAGE_EVENT_HANDLER_H_
