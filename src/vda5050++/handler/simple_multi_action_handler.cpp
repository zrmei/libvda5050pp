// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains the implementation of the SimpleMultiActionHandler
//
//
#include "vda5050++/handler/simple_multi_action_handler.h"

#include "vda5050++/core/checks/action.h"
#include "vda5050++/core/common/conversion.h"

using namespace vda5050pp::handler;

SimpleMultiActionHandler::SimpleMultiActionHandler(
    std::initializer_list<vda5050pp::agv_description::ActionDeclaration> declarations)
    : declarations_(std::move(declarations)) {}

SimpleMultiActionHandler::SimpleMultiActionHandler(
    const std::set<vda5050pp::agv_description::ActionDeclaration> &declarations)
    : declarations_(declarations) {}

SimpleMultiActionHandler::SimpleMultiActionHandler(
    std::set<vda5050pp::agv_description::ActionDeclaration> &&declarations)
    : declarations_(std::move(declarations)) {}

void SimpleMultiActionHandler::addActionDeclaration(
    const vda5050pp::agv_description::ActionDeclaration &decl) {
  this->declarations_.insert(decl);
}

bool SimpleMultiActionHandler::match(const vda5050::Action &action) const noexcept(true) {
  return std::any_of(this->declarations_.begin(), this->declarations_.end(), [&action](auto &d) {
    return vda5050pp::core::checks::matchActionType(d, action);
  });
}

vda5050pp::handler::ValidationResult SimpleMultiActionHandler::validate(
    const vda5050::Action &action, vda5050pp::misc::ActionContext context) noexcept(true) {
  ValidationResult result;

  for (const auto &decl : this->declarations_) {
    if (vda5050pp::core::checks::matchActionType(decl, action)) {
      result.errors = vda5050pp::core::checks::validateActionWithDeclaration(action, context, decl,
                                                                             result.parameters);
      break;
    }
  }

  return result;
}

std::list<vda5050::AgvAction> SimpleMultiActionHandler::getActionDescription() const
    noexcept(false) {
  std::list<vda5050::AgvAction> actions;

  std::transform(this->declarations_.begin(), this->declarations_.end(),
                 std::back_inserter(actions), &vda5050pp::core::common::fromActionDeclaration);

  return actions;
}