//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/handler/simple_action_handler.h"

#include "vda5050++/core/checks/action.h"
#include "vda5050++/core/common/conversion.h"

using namespace vda5050pp::handler;

bool SimpleActionHandler::match(const vda5050::Action &action) const noexcept(true) {
  return vda5050pp::core::checks::matchActionType(this->decl_, action);
}

vda5050pp::handler::ValidationResult SimpleActionHandler::validate(
    const vda5050::Action &action, vda5050pp::misc::ActionContext context) noexcept(true) {
  ValidationResult result;
  result.errors = vda5050pp::core::checks::validateActionWithDeclaration(
      action, context, this->decl_, result.parameters);
  return result;
}

std::list<vda5050::AgvAction> SimpleActionHandler::getActionDescription() const noexcept(false) {
  return {vda5050pp::core::common::fromActionDeclaration(this->decl_)};
}