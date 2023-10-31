//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_CHECKS_ACTION_H_
#define VDA5050_2B_2B_CORE_CHECKS_ACTION_H_

#include <vda5050/Action.h>
#include <vda5050/Error.h>

#include <list>

#include "vda5050++/agv_description/action_declaration.h"
#include "vda5050++/handler/base_action_handler.h"

namespace vda5050pp::core::checks {

bool matchActionType(const vda5050pp::agv_description::ActionDeclaration &action_declaration,
                     const vda5050::Action &action) noexcept(true);

std::list<vda5050::Error> validateActionWithDeclaration(
    const vda5050::Action &action, vda5050pp::misc::ActionContext context,
    const vda5050pp::agv_description::ActionDeclaration &action_declaration,
    std::optional<std::reference_wrapper<vda5050pp::handler::ParametersMap>> parameters =
        std::nullopt) noexcept(false);

std::list<vda5050::Error> contextCheck(
    const vda5050::Action &action, vda5050pp::misc::ActionContext context,
    const vda5050pp::agv_description::ActionDeclaration &action_declaration);

///
///\brief Check if an action has a unique ID. Checks against the current state and a seen set.
///
///\param action the action to check.
///\param seen the seen set. It will be extended with the id of the given action.
///\return std::list<vda5050::Error> the error list
///
std::list<vda5050::Error> uniqueActionId(
    const vda5050::Action &action, std::set<std::string_view, std::less<>> &seen) noexcept(false);

std::list<vda5050::Error> controlActionFeasible(const vda5050::Action &action) noexcept(false);

}  // namespace vda5050pp::core::checks

#endif  // VDA5050_2B_2B_CORE_CHECKS_ACTION_H_
