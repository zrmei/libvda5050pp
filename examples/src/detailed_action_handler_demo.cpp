// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
//
#include <vda5050++/agv_description/action_declaration.h>
#include <vda5050++/handle.h>
#include <vda5050++/misc/action_parameter_view.h>

class MyActionHandler : public vda5050pp::handler::BaseActionHandler {
public:
  bool match(const vda5050::Action &action) const noexcept(false) override {
    return action.actionType == "my_type";
  }

  vda5050pp::handler::ActionCallbacks prepare(
      std::shared_ptr<vda5050pp::handler::ActionState> action_state,
      std::shared_ptr<vda5050pp::handler::ParametersMap> parameters) noexcept(false) override {
    vda5050pp::handler::ActionCallbacks cbs;
    cbs.on_start = [parameters](vda5050pp::handler::ActionState &state) {
      if (auto it = parameters->find("duration"); it != parameters->end()) {
        // Do something
      }
      state.setRunning();
    };
    cbs.on_pause = [](vda5050pp::handler::ActionState &state) {
      // pause
      state.setPaused();
    };
    cbs.on_resume = [](vda5050pp::handler::ActionState &state) {
      // resume
      state.setRunning();
    };
    cbs.on_cancel = [](vda5050pp::handler::ActionState &state) {
      // cancel
      state.setFailed();
    };

    return cbs;
  }

  vda5050pp::handler::ValidationResult validate(
      const vda5050::Action &action,
      vda5050pp::misc::ActionContext context) noexcept(false) override {
    vda5050pp::misc::ActionParameterView params(
        action.actionParameters.value_or(std::vector<vda5050::ActionParameter>{}));

    vda5050pp::handler::ValidationResult result;

    if (auto maybe_duration = params.tryGetFloat("duration"); maybe_duration) {
      result.parameters.insert_or_assign("duration", (double)*maybe_duration);
    } else {
      vda5050::Error error;
      error.errorType = "Parameter duration not found";
      result.errors.push_back(error);
    }

    return result;
  }

  std::list<vda5050::AgvAction> getActionDescription() const noexcept(false) override {
    vda5050::AgvAction action;
    action.actionType = "light_signal";
    action.actionDescription = "Turn on the light signal";

    vda5050::ActionParameterFactsheet param;
    param.key = "duration";
    param.isOptional = false;
    param.valueDataType = vda5050::ValueDataType::FLOAT;

    action.actionParameters = {{param}};

    return {action};
  }
};

int main() {
  // Setup a simple config
  vda5050pp::Config cfg;
  cfg.refAgvDescription().agv_id = "agv1";
  cfg.refAgvDescription().manufacturer = "Open Logistics Foundation";
  cfg.refAgvDescription().serial_number = "1234567890";

  // Initialize the library
  vda5050pp::Handle handle;
  handle.initialize(cfg);

  // Register handlers
  handle.registerActionHandler(std::make_shared<MyActionHandler>());

  /// ....

  handle.shutdown();
}