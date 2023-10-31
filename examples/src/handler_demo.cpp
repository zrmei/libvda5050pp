// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include <vda5050++/handle.h>
#include <vda5050++/handler/base_navigation_handler.h>
#include <vda5050++/handler/base_query_handler.h>
#include <vda5050++/handler/simple_multi_action_handler.h>
#include <vda5050++/misc/action_declarations.h>

static vda5050pp::agv_description::ActionDeclaration getPickDeclaration() {
  using namespace vda5050pp::misc;
  auto pick = action_declarations::k_pick;

  auto height = action_parameter::k_height;
  height.ordinal_min = "0.5";
  height.ordinal_max = "1.5";

  pick.optional_parameter.extract(action_parameter::k_height);
  pick.optional_parameter.insert(height);

  return pick;
}

static vda5050pp::agv_description::ActionDeclaration getDropDeclaration() {
  using namespace vda5050pp::misc;
  auto drop = action_declarations::k_drop;

  auto height = action_parameter::k_height;
  height.ordinal_min = "0.5";
  height.ordinal_max = "1.5";

  drop.optional_parameter.extract(action_parameter::k_height);
  drop.optional_parameter.insert(height);

  return drop;
}

class PickDropHandler : public vda5050pp::handler::SimpleMultiActionHandler {
public:
  // Initialize the SimpleMultiActionHandler to handle the default pick and drop format.
  PickDropHandler()
      : vda5050pp::handler::SimpleMultiActionHandler({getPickDeclaration(), getDropDeclaration()}) {
  }

  vda5050pp::handler::ActionCallbacks prepare(
      std::shared_ptr<vda5050pp::handler::ActionState> action_state,
      std::shared_ptr<vda5050pp::handler::ParametersMap> parameters) noexcept(false) override {
    // Prepare is called as soon, as the Action is known to be executed in the future.
    // So we can setup all callbacks ect from here
    if (action_state->getAction().actionId == "pick") {
      return handlePick(parameters);
    } else {
      return handleDrop(parameters);
    }
  }

  vda5050pp::handler::ActionCallbacks handlePick(
      std::shared_ptr<vda5050pp::handler::ParametersMap> parameters) {
    vda5050pp::handler::ActionCallbacks cbs;
    cbs.on_start = [parameters](vda5050pp::handler::ActionState &state) {
      // Instruct AGV

      // Set action to running
      state.setRunning();

      // Get the parameters of interest likes this:
      if (auto it = parameters->find("height"); it != parameters->end()) {
        auto height = std::get<double>(it->second);
      }

      // Await AGV status

      // Set result status
      state.setFinished("Optional VDA5050 ResultCode");
    };
    cbs.on_pause = [](vda5050pp::handler::ActionState &state) {
      // Instruct AGV
      state.setPaused();
    };
    cbs.on_resume = [](vda5050pp::handler::ActionState &state) {
      // Instruct AGV
      state.setRunning();
    };
    cbs.on_cancel = [](vda5050pp::handler::ActionState &state) {
      // Instruct AGV
      state.setFailed();
    };

    return cbs;
  }

  vda5050pp::handler::ActionCallbacks handleDrop(
      std::shared_ptr<vda5050pp::handler::ParametersMap> parameters) {
    vda5050pp::handler::ActionCallbacks cbs;
    cbs.on_start = [](vda5050pp::handler::ActionState &state) {
      // Instruct AGV

      // Set action to running
      state.setRunning();

      // Await AGV status

      // Set result status
      state.setFinished("Optional VDA5050 ResultCode");
    };
    cbs.on_pause = [](vda5050pp::handler::ActionState &state) {
      // Instruct AGV
      state.setPaused();
    };
    cbs.on_resume = [](vda5050pp::handler::ActionState &state) {
      // Instruct AGV
      state.setRunning();
    };
    cbs.on_cancel = [](vda5050pp::handler::ActionState &state) {
      // Instruct AGV
      state.setFailed();
    };

    return cbs;
  }
};

class NavigationHandler : public vda5050pp::handler::BaseNavigationHandler {
public:
  void horizonUpdated(
      const std::list<std::shared_ptr<const vda5050::Node>> &horz_replace_nodes,
      const std::list<std::shared_ptr<const vda5050::Edge>> &horz_replace_edges) override {
    // Optionally do something with the new horizon
  }

  void baseIncreased(
      const std::list<std::shared_ptr<const vda5050::Node>> &base_expand_nodes,
      const std::list<std::shared_ptr<const vda5050::Edge>> &base_expand_edges) override {
    // Optionally do something with the base's extension
  }

  void navigateToNextNode(std::shared_ptr<const vda5050::Node> next_node,
                          std::shared_ptr<const vda5050::Edge> via_edge) override {
    // Instruct the AGV to move to "next_node" via "via_edge"

    auto agv_position_callback = [this](double x, double y, double theta, std::string_view map) {
      // Dump the position into the state (for visualization and state message purposes only)
      this->setPosition(x, y, theta, map);
    };

    this->setNodeReached(next_node->sequenceId);
  }

  void upcomingSegment(decltype(vda5050::Node::sequenceId) begin_seq,
                       decltype(vda5050::Node::sequenceId) end_seq) override {
    // Instruct the AGV to travel along the nodes and edges in [begin_seq, end_seq].
    // The AGV can travel without stopping to execute actions (except for instant actions)

    auto agv_position_callback = [this](double x, double y, double theta, std::string_view map) {
      if (this->evalPosition(x, y, theta, map)) {
        // Optionally notify the AGV about the node, that was reached.
      }
    };
  }

  void cancel() override {
    // Instruct AGV to cancel navigation
    this->setFailed();
  }

  void pause() override {
    // Instruct AGV to pause navigation
    this->setPaused();
  }

  void resume() override {
    // Instruct AGV to resume navigation
    this->setResumed();
  }
};

class QueryHandler : public vda5050pp::handler::BaseQueryHandler {
public:
  QueryPauseResumeResult queryPauseable() override {
    QueryPauseResumeResult result;
    result.errors = {
        // Ask AGV if can pause (no errors iff. ok)
    };
    return result;
  }

  QueryPauseResumeResult queryResumable() override {
    QueryPauseResumeResult result;
    result.errors = {
        // Ask AGV if can resume (no errors iff. ok)
    };
    return result;
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
  handle.registerActionHandler(std::make_shared<PickDropHandler>());
  handle.registerNavigationHandler(std::make_shared<NavigationHandler>());
  handle.registerQueryHandler(std::make_shared<QueryHandler>());

  /// ....

  handle.shutdown();
}