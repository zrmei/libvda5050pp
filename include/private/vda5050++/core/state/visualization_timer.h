// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains the declaration of the VisualizationTimer module
//
//

#ifndef VDA5050_2B_2B_CORE_STATE_VISUALIZATION_TIMER_H_
#define VDA5050_2B_2B_CORE_STATE_VISUALIZATION_TIMER_H_

#include "vda5050++/core/common/interruptable_timer.h"
#include "vda5050++/core/common/scoped_thread.h"
#include "vda5050++/core/module.h"

namespace vda5050pp::core::state {

class VisualizationTimer : public vda5050pp::core::Module {
private:
  vda5050pp::core::common::InterruptableTimer timer_;
  vda5050pp::core::common::ScopedThread<void()> thread_;
  std::chrono::system_clock::duration update_period_;

protected:
  void sendVisualization() const;
  void timerRoutine(vda5050pp::core::common::StopToken stop_token) const;

public:
  VisualizationTimer();
  void initialize(vda5050pp::core::Instance &instance) override;
  void deinitialize(vda5050pp::core::Instance &instance) override;
  std::string_view describe() const override;
  std::shared_ptr<vda5050pp::config::ModuleSubConfig> generateSubConfig() const override;
};

}  // namespace vda5050pp::core::state

#endif  // VDA5050_2B_2B_CORE_STATE_VISUALIZATION_TIMER_H_
