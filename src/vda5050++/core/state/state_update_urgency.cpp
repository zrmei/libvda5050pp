//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/core/state/state_update_urgency.h"

using namespace vda5050pp::core::state;
using namespace std::chrono_literals;

StateUpdateUrgency::StateUpdateUrgency(std::chrono::system_clock::duration max_delay)
    : max_delay_(max_delay) {}

StateUpdateUrgency StateUpdateUrgency::immediate() { return StateUpdateUrgency(0s); }

StateUpdateUrgency StateUpdateUrgency::high() { return StateUpdateUrgency(100ms); }

StateUpdateUrgency StateUpdateUrgency::medium() { return StateUpdateUrgency(1s); }

StateUpdateUrgency StateUpdateUrgency::low() { return StateUpdateUrgency(5s); }

StateUpdateUrgency StateUpdateUrgency::custom(std::chrono::system_clock::duration max_delay) {
  return StateUpdateUrgency(max_delay);
}

std::chrono::system_clock::duration StateUpdateUrgency::getMaxDelay() const {
  return this->max_delay_;
}

bool StateUpdateUrgency::isImmediate() const { return this->max_delay_ == 0s; }