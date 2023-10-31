//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/handler/action_state.h"

#include "vda5050++/core/instance.h"

using namespace vda5050pp::handler;

ActionState::ActionState(std::shared_ptr<const vda5050::Action> action) : action_(action) {}

const vda5050::Action &ActionState::getAction() const noexcept(false) {
  if (this->action_ == nullptr) {
    VDA5050PPNullPointer(MK_EX_CONTEXT("action_ is unset"));
  }

  return *this->action_;
}