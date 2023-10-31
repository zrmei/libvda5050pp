//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_STATE_STATE_UPDATE_URGENCY_H_
#define VDA5050_2B_2B_CORE_STATE_STATE_UPDATE_URGENCY_H_

#include <chrono>

namespace vda5050pp::core::state {

class StateUpdateUrgency {
private:
  std::chrono::system_clock::duration max_delay_ = std::chrono::seconds(0);

public:
  StateUpdateUrgency() = default;
  explicit StateUpdateUrgency(std::chrono::system_clock::duration max_delay);
  static StateUpdateUrgency immediate();
  static StateUpdateUrgency high();
  static StateUpdateUrgency medium();
  static StateUpdateUrgency low();
  static StateUpdateUrgency custom(std::chrono::system_clock::duration max_delay);

  std::chrono::system_clock::duration getMaxDelay() const;
  bool isImmediate() const;
};

}  // namespace vda5050pp::core::state

#endif  // VDA5050_2B_2B_CORE_STATE_STATE_UPDATE_URGENCY_H_
