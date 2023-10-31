//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_VALIDATION_VALIDATION_EVENT_HANDLER_H_
#define VDA5050_2B_2B_CORE_VALIDATION_VALIDATION_EVENT_HANDLER_H_

#include <optional>

#include "vda5050++/core/events/validation_event.h"
#include "vda5050++/core/module.h"

namespace vda5050pp::core::validation {

class ValidationEventHandler : public vda5050pp::core::Module {
private:
  std::optional<vda5050pp::core::GenericEventManager<
      vda5050pp::core::events::ValidationEvent>::ScopedSubscriber>
      subscriber_;

  void handleValidateOrder(std::shared_ptr<vda5050pp::core::events::ValidateOrderEvent> evt) const;
  void handleValidateInstantActions(
      std::shared_ptr<vda5050pp::core::events::ValidateInstantActionsEvent> evt) const;

public:
  void initialize(vda5050pp::core::Instance &instance) override;
  void deinitialize(vda5050pp::core::Instance &instance) override;
  std::string_view describe() const override;
};

}  // namespace vda5050pp::core::validation

#endif  // VDA5050_2B_2B_CORE_VALIDATION_VALIDATION_EVENT_HANDLER_H_
