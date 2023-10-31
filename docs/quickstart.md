# libVDA5050++ quick-start guide

# 0. Requirements

This Guide assumes, that you have installed the library.

If not, follow one of the guides at [install](install.md).

# 1. Overview

libVDA5050++ is a mostly passive framework for VDA5050 compliant behavior.
It is meant to be used by an _Adapter_, which implements AGV specific logic (for example a ROS-Node).
Data will be fed from two ends i.e. the MC and the AGV.
The MC won't concern the user that much, since the MQTT Connection and message handling
will be done by the library. Each time a message is received, the library evaluates what
has to be done by the AGV and notifies it. This will be done via `Handlers` which have
to be implemented by the user. Every time the AGV yields some VDA5050 relevant data, the
user can dump that information into the library, such that it can evaluate the next steps and inform the MC.

# 2. CMake Integration

As described in [Install/Linking the libVDA5050++](install.md#linking-the-libvda5050), 
you need to include the following in your `CMakeLists.txt` to link the library with your executable
(called `my_app` here):

```cmake
find_package(libvda5050++ REQUIRED)
target_link_libraries(my_app PUBLIC libvda5050++::vda5050++)
```

# 3. Calling the library.

Within your code, you can interact with the library via the [vda5050pp::Handle](doxygen/html/classvda5050pp_1_1Handle.html).
The `vda5050pp::Handle::initialize` member function requires a
[`vda5050pp::Config`](doxygen/html/classvda5050pp_1_1Config.html) object.
For customization see [configuration](configuration.md).

```c++
#include <vda5050++/handle.h>

void initVDA5050() {
  vda5050pp::Handle handle;

  // Initialize with default config and go online
  handle.initialize(vda5050pp::Config());
}

void shutdownVDA5050() {
  vda5050pp::Handle handle;

  // Shutdown (send offline msg, ...)
  handle.shutdown();
}
```

Note, that the library will run independent of a `vda5050pp::Handle`'s
lifetime.
After calling `initVDA5050` the library is initialized and running.
This means it will be receiving messages and send status updates, 
nothing more at this point, since no callbacks were set.
The following steps will show you how install your callbacks and dump data into the library.

# 4. NavigationHandler

The NavigationHandler can be used to implement navigation behaviour.
You'll need to include the `vda5050++/handler/base_navigation_handler.h` header and
create your own handler deriving from [`vda5050pp::handler::BaseNavigationHandler`](doxygen/html/classvda5050pp_1_1handler_1_1BaseNavigationHandler.html).
The following code-snipped overwrites all pure virtual member functions with some stubs:

```c++
#include <vda5050++/handler/base_navigation_handler.h>

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

    // Notify the library, that the node was reached
    this->setNodeReached(next_node->sequenceId);
  }

  void upcomingSegment(decltype(vda5050::Node::sequenceId) begin_seq,
                       decltype(vda5050::Node::sequenceId) end_seq) override {
    // Instruct the AGV to travel along the nodes and edges in [begin_seq, end_seq].
    // The AGV can travel without stopping to execute actions (except for instant actions)

    auto agv_position_callback = [this](double x, double y, double theta, std::string_view map) {
      // Use eval position to make the libVDA5050++ decide if the node was reached (based on Node deviation)
      if (this->evalPosition(x, y, theta, map)) {
        // Optionally notify the AGV about the node, that was reached.
      }
    };
  }

  void cancel() override {
    // Instruct AGV to cancel navigation

    // Tell the library, that the navigation failed (this->setNodeReached() is still possible, 
    // when the AGV cannot stop in between nodes)
    this->setFailed();
  }

  void pause() override {
    // Instruct AGV to pause navigation

    // Tell the library, that the navigation is paused
    this->setPaused();
  }

  void resume() override {
    // Instruct AGV to resume navigation

    // Tell the library, that the navigation is paused
    this->setResumed();
  }
};
```

#### `navigateToNextNode` vs `upcomingSegment`

These two member functions are meant to be used mutually exclusive. You can choose which behavior
suites your use-case best. `navigateToNextNode` is called exactly when the AGV shall move to
the next node of the order (after processing actions ect.). `upcomingSegment` provides the
AGV a sequence of nodes and edges, which will be traversable without stopping for a blocking action (like
`pick` or `drop`). This can be used to prevent the AGV from unnecessary stops at each node.

#### Register the NavigationHandler

_NOTE: This can only be called after `vda5050pp::Handle::initialize`._

```c++
void registerNavigationHandler() {
  vda5050pp::Handle handle;

  handle.registerNavigationHandler(std::make_shared<NavigationHandler>());
}
```

# 5. ActionHandler

[ActionHandler](doxygen/html/classvda5050pp_1_1handler_1_1BaseActionHandler.html) can be used to implement Action behavior.
It is intended, that you register multiple ActionHandlers. Each [ActionHandler](doxygen/html/classvda5050pp_1_1handler_1_1BaseActionHandler.html)
can for example handle similar actions, like `pick` and `drop` to avoid possible redundant code.

The following snipped describes, how you can build an [ActionHandler](doxygen/html/classvda5050pp_1_1handler_1_1BaseActionHandler.html)
for `pick` and `drop` with the help of the [SimpleMultiActionHandler](doxygen/html/classvda5050pp_1_1handler_1_1SimpleMultiActionHandler.html):

```c++
#include <vda5050++/handler/simple_multi_action_handler.h>
#include <vda5050++/misc/action_declarations.h>

class PickDropHandler : public vda5050pp::handler::SimpleMultiActionHandler {
public:
  // Initialize the SimpleMultiActionHandler to handle the default pick and drop format.
  PickDropHandler()
    : vda5050pp::handler::SimpleMultiActionHandler(
          {vda5050pp::misc::action_declarations::k_pick,
           vda5050pp::misc::action_declarations::k_drop}) {}

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

      // Get the parameters of interest like this:
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
```

#### Callbacks and `prepare`

The `vda5050pp::BaseActionHandler::prepare` member function will be called as soon, as the Action is known and validated
i.e. received via order or instant actions. At this point the user should prepare all callbacks to setup the
handling of the individual action.
The [`vda5050pp::handler::ActionState`](doxygen/html/classvda5050pp_1_1handler_1_1ActionState.html) can be seen as some kind of
state machine associated to exactly one active action. The user shall call it's member functions to update the state of
the action. As a result of the `prepare` function
a [`vda5050pp::handler::ActionCallbacks`](doxygen/html/structvda5050pp_1_1handler_1_1ActionCallbacks.html) object is returned.
It contains four callbacks that will be called by the library, once the associated active action has to
`start`, `pause`, `resume` or `cancel`.

#### ActionParameters

A parameter map is also passed to the `vda5050pp::BaseActionHandler::prepare` member function.
It is essentially a `std::map<std::string, std::variant<std::string, int64_t, double, bool, vda5050pp::misc::AnyPtr>>` type.
When using a [`vda5050pp::handler::SimpleActionHandler`](doxygen/html/classvda5050pp_1_1handler_1_1SimpleActionHandler.html)
or [`vda5050pp::handler::SimpleMultiActionHandler`](doxygen/html/classvda5050pp_1_1handler_1_1SimpleMultiActionHandler.html),
the parameters are parsed automatically to match the 
[`vda5050pp::agv_description::ActionDeclaration`](doxygen/html/structvda5050pp_1_1agv__description_1_1ActionDeclaration.html)
passed to the constructor.

#### Register the ActionHandler

_NOTE: This can only be called after `vda5050pp::Handle::initialize`._

```c++
void registerActionHandler() {
  vda5050pp::Handle handle;

  handle.registerActionHandler(std::make_shared<PickDropHandler>());
}
```

# 6. QueryHandler

Another important part is the [`vda5050pp::handler::BaseQueryHandler`](doxygen/html/classvda5050pp_1_1handler_1_1BaseQueryHandler.html).
It is meant to be a place, where the library can ask the user certain things.
Currently there are three questions, that needs to be answered anytime.

1. Is the AGV pauseable?
2. Is the AGV resumable?
3. Can the AGV accept this ZoneSet?

A implementation might look like this:

```c++
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

  std::list<vda5050::Error> queryAcceptZoneSet(std::string_view zone_set_id) override {
    return {
      // Ask AGV if it can accept this zone set (no errors iff. accept this zone set)
    };
  }
};
```

The pauseable/resumable will be queried after receiving `startPause` or
`stopPause` instant actions. Before notifying each handler the query is done.
Only if it succeeds the instant action is executed.

This detailed handling is optional. You can also configure the libVDA5050++ to automatically
handle the query requests:
```c++
vda5050pp::Config cfg;
cfg.refQueryEventHandlerSubConfig().setDefaultAcceptZoneSets({"zone1", "zone2"});
cfg.refQueryEventHandlerSubConfig().setDefaultPauseableSuccess(true);
cfg.refQueryEventHandlerSubConfig().setDefaultResumableSuccess(true);
```
_NOTE: The config has to be passed via `vda5050pp::Handle::initialize`._

#### Register the QueryHandler

_NOTE: This can only be called after `vda5050pp::Handle::initialize`._

```c++
void registerQueryHandler() {
  vda5050pp::Handle handle;

  handle.registerQueryHandler(std::make_shared<QueryHandler>());
}
```

# 7. Sinks

Sinks can be used to dump all sorts of information into the libVDA5050++.

This snippet adds a load and an error to the state. It also sets the
current battery state:

```c++
void useSink() {
  vda5050pp::Handle handle;
  auto sink = handle.getStatusSink();
  sink.addLoad(vda5050::Load{});
  sink.addError(vda5050::Error{});
  sink.setBatteryState(vda5050::BatteryState{});
}
```

There are many more member functions available, see [`vda5050pp::sinks::StatusSink`](doxygen/html/classvda5050pp_1_1sinks_1_1StatusSink.html).
You can also set navigation info with the [`vda5050pp::sinks::NavigationSink`](doxygen/html/classvda5050pp_1_1sinks_1_1NavigationSink.html).

# 8. Raw Events

If any of the interfaces shown so far do not suite your use case, you can also access
the raw events, on which the interfaces were built.

The following code snippet shows a simple way to navigate using events:
```c++
#include <vda5050++/events/event_handle.h>

void navigateWithEvents() {
  vda5050pp::events::EventHandle handle;
  auto subscriber = handle.getScopedNavigationEventSubscriber();
  subscriber->subscribe([&handle](std::shared_ptr<vda5050pp::events::NavigationNextNode> evt){
    // Navigate the AGV and wait

    // Tell the library, that the node was reached
    auto node_reached = std::make_shared<vda5050pp::events::NavigationStatusNodeReached>();
    node_reached->node_seq_id = evt->next_node->sequenceId;
    handle.dispatch(node_reached);
  });

  // Keep this scope, as long, as you want to handle the events.
}
```
All available events are in the [`vda5050pp::events` namespace](doxygen/html/namespacevda5050pp_1_1events.html).
You can subscribe/dispatch them via the [`vda5050pp::events::EventHandle`](doxygen/html/classvda5050pp_1_1events_1_1EventHandle.html).