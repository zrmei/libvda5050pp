//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_AGV_HANDLER_ACTION_STATE_H_
#define VDA5050_2B_2B_CORE_AGV_HANDLER_ACTION_STATE_H_

#include <list>

#include "vda5050++/handler/action_state.h"

namespace vda5050pp::core::agv_handler {

class ActionState : public vda5050pp::handler::ActionState {
public:
  explicit ActionState(std::shared_ptr<const vda5050::Action> action) noexcept(true);

  ~ActionState() override = default;

  void setRunning() noexcept(false) override;
  void setPaused() noexcept(false) override;
  void setFinished() noexcept(false) override;
  void setFinished(std::string_view result_code) noexcept(false) override;
  void setFailed() noexcept(false) override;
  void setFailed(const std::list<vda5050::Error> &errors) noexcept(false) override;
};

}  // namespace vda5050pp::core::agv_handler

#endif  // VDA5050_2B_2B_CORE_AGV_HANDLER_ACTION_STATE_H_
