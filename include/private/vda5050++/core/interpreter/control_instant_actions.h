// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_INTERPRETER_CONTROL_INSTANT_ACTIONS_H_
#define VDA5050_2B_2B_CORE_INTERPRETER_CONTROL_INSTANT_ACTIONS_H_

#include <vda5050/Action.h>
#include <vda5050/AgvAction.h>
#include <vda5050/Error.h>

#include <list>

#include "vda5050++/core/events/event_control_blocks.h"
#include "vda5050++/misc/action_context.h"

namespace vda5050pp::core::interpreter {

bool isControlInstantAction(const vda5050::Action &instant_action);

std::list<vda5050::Error> validateControlInstantAction(const vda5050::Action &instant_action,
                                                       vda5050pp::misc::ActionContext ctxt);

std::shared_ptr<std::list<vda5050::AgvAction>> listControlActions() noexcept(false);

std::shared_ptr<vda5050pp::core::events::EventControlBlock> makeCancelControlBlock(
    std::shared_ptr<const vda5050::Action> action);

std::shared_ptr<vda5050pp::core::events::EventControlBlock> makePauseControlBlock(
    std::shared_ptr<const vda5050::Action> action);

std::shared_ptr<vda5050pp::core::events::EventControlBlock> makeResumeControlBlock(
    std::shared_ptr<const vda5050::Action> action);

std::shared_ptr<vda5050pp::core::events::EventControlBlock> makeRequestFactsheetControlBlock(
    std::shared_ptr<const vda5050::Action> action);

std::shared_ptr<vda5050pp::core::events::EventControlBlock> makeStateRequestControlBlock(
    std::shared_ptr<const vda5050::Action> action);

std::shared_ptr<vda5050pp::core::events::EventControlBlock> makeControlInstantActionControlBlock(
    std::shared_ptr<const vda5050::Action> action);

}  // namespace vda5050pp::core::interpreter

#endif  // VDA5050_2B_2B_CORE_INTERPRETER_CONTROL_INSTANT_ACTIONS_H_
