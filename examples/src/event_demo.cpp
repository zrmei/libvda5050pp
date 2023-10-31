// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
//

#include <vda5050++/events/event_handle.h>
#include <vda5050++/handle.h>

#include <future>

void navigateWithEvents(std::future<void> barrier) {
  vda5050pp::events::EventHandle handle;
  auto subscriber = handle.getScopedNavigationEventSubscriber();
  subscriber->subscribe([&handle](std::shared_ptr<vda5050pp::events::NavigationNextNode> evt) {
    // Navigate the AGV and wait

    // Tell the library, that the node was reached
    auto node_reached = std::make_shared<vda5050pp::events::NavigationStatusNodeReached>();
    node_reached->node_seq_id = evt->next_node->sequenceId;
    handle.dispatch(node_reached);
  });

  barrier.wait();
}

void actionWithEvents(std::future<void> barrier) {
  vda5050pp::events::EventHandle handle;
  auto subscriber = handle.getScopedActionEventSubscriber();

  std::map<std::string, std::shared_ptr<const vda5050::Action>, std::less<>> actions;

  subscriber->subscribe([](std::shared_ptr<vda5050pp::events::ActionValidate> evt) {
    if (auto result = evt->acquireResultToken(); result.isAcquired()) {
      // Validate the action
      std::list<vda5050::Error> errors{
          // Validation result (empty iff no errors)
      };
      result.setValue(errors);
    }
  });

  subscriber->subscribe([&actions](std::shared_ptr<vda5050pp::events::ActionPrepare> evt) {
    // Prepare the action passed by the events and save it
    actions.emplace(evt->action->actionId, evt->action);
  });

  subscriber->subscribe([&handle](std::shared_ptr<vda5050pp::events::ActionStart> evt) {
    // Start the action and send result
    auto status = std::make_shared<vda5050pp::events::ActionStatusRunning>();
    status->action_id = evt->action_id;
    handle.dispatch(status);
  });

  subscriber->subscribe([&handle](std::shared_ptr<vda5050pp::events::ActionPause> evt) {
    // Pause the action and send result
    auto status = std::make_shared<vda5050pp::events::ActionStatusPaused>();
    status->action_id = evt->action_id;
    handle.dispatch(status);
  });

  subscriber->subscribe([&handle](std::shared_ptr<vda5050pp::events::ActionResume> evt) {
    // Resume the action and send result
    auto status = std::make_shared<vda5050pp::events::ActionStatusRunning>();
    status->action_id = evt->action_id;
    handle.dispatch(status);
  });

  subscriber->subscribe([&handle](std::shared_ptr<vda5050pp::events::ActionCancel> evt) {
    // Cancel the action and send result
    auto status = std::make_shared<vda5050pp::events::ActionStatusFailed>();
    status->action_id = evt->action_id;
    status->action_errors = {
        // Error describing, why the action failed during cancel.
        // Normal cancel allows finishing/failing of each acton.
    };
    handle.dispatch(status);
  });

  subscriber->subscribe([&actions](std::shared_ptr<vda5050pp::events::ActionForget> evt) {
    // Forget about the action
    actions.erase(evt->action_id);
  });

  subscriber->subscribe([](std::shared_ptr<vda5050pp::events::ActionList> evt) {
    // List all compatible actions
    if (auto result = evt->acquireResultToken(); result.isAcquired()) {
      auto action_list =
          std::make_shared<std::list<vda5050::AgvAction>>(std::list<vda5050::AgvAction>({
              // List of actions
          }));
      result.setValue(action_list);
    }
  });

  barrier.wait();
}

void queryWithEvents(std::future<void> barrier) {
  vda5050pp::events::EventHandle handle;
  auto subscriber = handle.getScopedQueryEventSubscriber();

  subscriber->subscribe([](std::shared_ptr<vda5050pp::events::QueryAcceptZoneSet> evt) {
    if (auto result = evt->acquireResultToken(); result.isAcquired()) {
      // Check if the zone set can be accepted
      std::list<vda5050::Error> errors{
          // result (empty iff ok)
      };
      result.setValue(errors);
    }
  });

  subscriber->subscribe([](std::shared_ptr<vda5050pp::events::QueryPauseable> evt) {
    if (auto result = evt->acquireResultToken(); result.isAcquired()) {
      vda5050pp::events::QueryPauseResumeResult pauseable;
      // Check if the AGV can be paused
      pauseable.errors = {
          // result (empty iff ok)
      };
      result.setValue(pauseable);
    }
  });

  subscriber->subscribe([](std::shared_ptr<vda5050pp::events::QueryResumable> evt) {
    if (auto result = evt->acquireResultToken(); result.isAcquired()) {
      vda5050pp::events::QueryPauseResumeResult resumable;
      // Check if the AGV can be paused
      resumable.errors = {
          // result (empty iff ok)
      };
      result.setValue(resumable);
    }
  });

  barrier.wait();
}

int main() {
  // Setup a simple config
  vda5050pp::Config cfg;
  cfg.refAgvDescription().agv_id = "agv1";
  cfg.refAgvDescription().manufacturer = "Open Logistics Foundation";
  cfg.refAgvDescription().serial_number = "1234567890";

  // Initialize the library
  vda5050pp::Handle handle;
  handle.initialize(cfg);

  auto shutdown_promise = std::promise<void>();

  auto nav_done = std::async(std::launch::async, navigateWithEvents, shutdown_promise.get_future());
  auto act_done = std::async(std::launch::async, actionWithEvents, shutdown_promise.get_future());
  auto que_done = std::async(std::launch::async, queryWithEvents, shutdown_promise.get_future());

  // Wait for shutdown
  shutdown_promise.set_value();

  // Wait for threads
  nav_done.wait();
  act_done.wait();
  que_done.wait();

  // Shutdown the library
  handle.shutdown();
}