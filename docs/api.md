# AGV Description

The [`vda5050pp::agv_description` namespace](doxygen/html/namespacevda5050pp_1_1agv__description.html)
contains data structures to describe the AGV. The main purpose is to provide information
for the [Factsheet](https://github.com/VDA5050/VDA5050/blob/2.0.0/VDA5050_EN_V1.md#-616-topic-factsheet) generation and [Topic](https://github.com/VDA5050/VDA5050/blob/2.0.0/VDA5050_EN_V1.md#-63-mqtt-topic-levels)
selection. Information in the factsheet will be partially filled by
the library, the remaining information must be provided by the user (with the AGV description).
You can set the AGV description in the [`vda5050pp::Config`](doxygen/html/classvda5050pp_1_1Config.html)
like this:

```c++
vda5050pp::Config cfg;
cfg.refAgvDescription().agv_id = "my_agv_id";
cfg.refAgvDescription().manufacturer = "manufacturer";
cfg.refAgvDescription().serial_number = "sn";
```

More information can be found in [Configuration/AGV Description](configuration.md#agv-description).

## ActionDeclaration

The [`vda5050pp::agv_description::ActionDeclaration`s](doxygen/html/structvda5050pp_1_1agv__description_1_1ActionDeclaration.html)
describe an action including it's parameters. It will be used to verify incoming actions with the
[`vda5050pp::handler::SimpleActionHandler`](doxygen/html/classvda5050pp_1_1handler_1_1SimpleActionHandler.html)
and to inform the MC about them via the `factsheet` topic.

Each action may have parameters which have to be defined. This snippet contains an
example definition:

```c++
  vda5050pp::agv_description::ParameterRange duration;
  duration.key = "duration";  // The parameter .key field (IDs this parameter range uniquely)
  duration.description = "The duration of this action";
  duration.ordinal_min = "1.5";   // Optional constraints for the min duration
  duration.ordinal_max = "20.0";  // Optional constraints for the max duration
  // duration.value_set = {"1", "2", "3"}; // Value sets are also possible.
  duration.type = vda5050pp::agv_description::ParameterValueType::k_float;
```

The action declaration then might look like this:
```c++
  vda5050pp::agv_description::ActionDeclaration light_signal;
  light_signal.action_type = "light_signal";  // The action type (IDs this action)
  light_signal.description = "Turn on the AGVs signal light";
  light_signal.parameter = {duration};                          // Mandatory duration parameter
  light_signal.optional_parameter = {};                         // No optional parameters
  light_signal.blocking_types = {vda5050::BlockingType::NONE};  // Only none blocking
  light_signal.edge = false;                                    // Not on edges
  light_signal.node = false;                                    // Not on nodes
  light_signal.instant = true;                                  // As instant action
  light_signal.result_description = std::nullopt;               // No result
```
# Config

The configuration of the library is described [here](configuration.md).

# Events

All events are within the [`vda5050pp::events` namespace](doxygen/html/namespacevda5050pp_1_1events.html).
Each event can either be dispatched or received via a subscriber. Dispatchable events usually have `Status`
in their name for example `ActionEvent...` is received and `ActionStatus...` is dispatched.

You can access events by the [`vda5050pp::events::EventHandle`](doxygen/html/classvda5050pp_1_1events_1_1EventHandle.html).

A simple subscribe and dispatch looks like this:
```c++
vda5050pp::events::EventHandle handle;
auto subscriber = handle.getScopedNavigationEventSubscriber();
subscriber->subscribe([&handle](std::shared_ptr<vda5050pp::events::NavigationNextNode> evt) {
  // Navigate the AGV and wait

  // Tell the library, that the node was reached
  auto node_reached = std::make_shared<vda5050pp::events::NavigationStatusNodeReached>();
  node_reached->node_seq_id = evt->next_node->sequenceId;
  handle.dispatch(node_reached);
});

// Keep this scope, as long, as you want to handle the events.
```

# Exceptions

All Exceptions, which are caught and/or raised by the library will have
the common [`vda5050pp::VDA5050PPError`](doxygen/html/classvda5050pp_1_1VDA5050PPError.html) base class.
This class of errors is not meant to be instantiated by the user of this library, since it
needs a specific [`vda5050pp::VDA5050PPErrorContext`](doxygen/html/structvda5050pp_1_1VDA5050PPErrorContext.html), which will be generated created internally.

## Exceptions in thrown in Callbacks

If you throw any exceptions within libVDA5050++ callbacks
you will most likely see something like this in your log:
`...EventManager caught an exception, while processing events: <exception>`.
The library will be in a failed state and needs to be reinitialized to ensure consistency.

# Handle

The [`vda5050pp::Handle`](doxygen/html/classvda5050pp_1_1Handle.html) can be instantiated to control the library.
It is a thin-object containing no members, but provides access to the internal `vda5050pp` instance.
It's lifetime is not bound to the internal instances lifetime. The following member functions can
be used to control the library:

| Member function             | purpose                                                                                                                                                                                            |
| --------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `initialize`                | Initializes the internal instance with a [`vda5050pp::Config`](doxygen/html/classvda5050pp_1_1Config.html) object.                                                                                 |
| `shutdown`                  | Send offline message and tear-down the internal instance.                                                                                                                                          |
| `registerActionHandler`     | Register any [`vda5050pp::handler::BaseActionHandler`](doxygen/html/classvda5050pp_1_1handler_1_1BaseActionHandler.html) derived object as an ActionHandler.                                       |
| `registerNavigationHandler` | Register any [`vda5050pp::handler::BaseNavigationHandler`](doxygen/html/classvda5050pp_1_1handler_1_1BaseNavigationHandler.html) derived object as a NavigationHandler. Overrides the current one. |
| `registerQueryHandler`      | Register any [`vda5050pp::handler::BaseQueryHandler`](doxygen/html/classvda5050pp_1_1handler_1_1BaseQueryHandler.html) derived object as a QueryHandler. Overrides the current one.                |
| `getStatusSink`             | Return a new [`vda5050pp::sinks::StatusSink`](doxygen/html/classvda5050pp_1_1sinks_1_1StatusSink.html).                                                                                            |
| `getNavigationSink`         | Return a new [`vda5050pp::sinks::NavigationSink`](doxygen/html/classvda5050pp_1_1sinks_1_1NavigationSink.html).                                                                                    |
| `getLogger`                 | Return the internal `spdlog::logger` of a module, by it's key. This function is by default not available. To activate it, see [Install/Configuration Options](/install/#configuration-options).    |


# Handler

## ActionState

The [`vda5050pp::handler::ActionState`](doxygen/html/classvda5050pp_1_1handler_1_1ActionState.html) is passed as an argument
to the `vda5050pp::handler::BaseActionHandler::prepare`
member function and represents the state of a running action. It has the following semantics:

| Member function | effect                                                                                              |
| --------------- | --------------------------------------------------------------------------------------------------- |
| `getAction`     | Access the associated action.                                                                       |
| `setRunning`    | Set the VDA5050 action state to `RUNNING`. Should only be called after starting the action.         |
| `setPaused`     | Set the VDA5050 action state to `PAUSED`. Can be called anytime (if not already paused).            |
| `setResumed`    | Set the VDA5050 action state to `RUNNING`. Can only be called when paused.                          |
| `setFailed`     | Set the VDA5050 action state to `FAILED`. Can be called anytime (if not already finished/failed).   |
| `setFinished`   | set the VDA5050 action state to `FINISHED`. Can be called anytime (if not already finished/failed). |

## BaseActionHandler

The [`vda5050pp::handler::BaseActionHandler`](doxygen/html/classvda5050pp_1_1handler_1_1BaseActionHandler.html)
is the base class for all user-defined action handlers. It provides
the following four pure virtual member functions:

| Member function        | purpose                                                                                                                                                                                |
| ---------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `match`                | Determine if this action handler can handle the action passed as an argument.                                                                                                          |
| `prepare`              | Prepare a new valid action. This means: register all needed callbacks and optionally prepare the AGV.                                                                                  |
| `validate`             | Check if a received action is valid. The return value determines if the action will be accepted. Returns a list of errors and a parsed parameter map, which will be passed to prepare. |
| `getActionDescription` | Return all action descriptions of this handler for the `factsheet`.                                                                                                                    |

Example implementation:
```c++
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

  vda5050pp::handler::ValidationResult validate(const vda5050::Action &action,
                                                vda5050pp::misc::ActionContext context)
      noexcept(false) override {
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
```


## BaseNavigationHandler

The [`vda5050pp::handler::BaseNavigationHandler`](doxygen/html/classvda5050pp_1_1handler_1_1BaseNavigationHandler.html) is the base class for the user-defined navigation handler. It provides
the following pure virtual member functions:

| Member function      | purpose                                                                                              |
| -------------------- | ---------------------------------------------------------------------------------------------------- |
| `horizonUpdated`     | Passes the new horizon, once it was updated with an order.                                           |
| `baseIncreased`      | Passes the extension of a base, once a order/update was received.                                    |
| `navigateToNextNode` | Is called when the AGV shall navigate to a certain node in the order.                                |
| `upcomingSegment`    | Is called when the AGV can navigate a set of nodes and edges without stopping for a blocking action. |
| `cancel`             | Is called when the order was aborted and the AGV has to stop.                                        |
| `pause`              | Is called when the order was paused by the MC.                                                       |
| `resume`             | Is called when the order was resumed by the MC.                                                      |

The status of the navigation can be set with the following member functions:

| Member function  | purpose                                                                                                                                         |
| ---------------- | ----------------------------------------------------------------------------------------------------------------------------------------------- |
| `setPaused`      | Tell the library, that navigation is paused.                                                                                                    |
| `setResumed`     | Tell the library, that navigation was resumed.                                                                                                  |
| `setFailed`      | Tell the library, that navigation failed. For example when the order was canceled and the goal was not reached yet.                             |
| `setNodeReached` | Tell the library, that a node (which was part of the order) was reached. You must call this for each node of the base in order.                 |
| `setPosition`    | Tell the library the current position of the AGV. For state and visualization only.                                                             |
| `evalPosition`   | Same as `setPosition`, but checks if the AGV is inside the deviation of the goal node. If so `setNodeReached` is called and `true` is returned. |

Example implementation stubs:

```c++
class NavigationHandler : public vda5050pp::handler::BaseNavigationHandler {
public:
  void horizonUpdated(
      const std::list<std::shared_ptr<const vda5050::Node>> &horz_replace_nodes,
      const std::list<std::shared_ptr<const vda5050::Edge>> &horz_replace_edges) override {
  }

  void baseIncreased(
      const std::list<std::shared_ptr<const vda5050::Node>> &base_expand_nodes,
      const std::list<std::shared_ptr<const vda5050::Edge>> &base_expand_edges) override {
  }

  void navigateToNextNode(std::shared_ptr<const vda5050::Node> next_node,
                          std::shared_ptr<const vda5050::Edge> via_edge) override {
  }

  void upcomingSegment(decltype(vda5050::Node::sequenceId) begin_seq,
                       decltype(vda5050::Node::sequenceId) end_seq) override {
  }

  void cancel() override {
  }

  void pause() override {
  }

  void resume() override {
  }
};
```

## BaseQueryHandler

The [`vda5050pp::handler::BaseQueryHandler`](doxygen/html/classvda5050pp_1_1handler_1_1BaseQueryHandler.html)
is the base class for the user-defined query handler. It provides
the following pure virtual member functions:

| Member function      | purpose                                                                                |
| -------------------- | -------------------------------------------------------------------------------------- |
| `queryPauseable`     | Can the AGV pause at the moment? (May be hindered by something like a hardware switch) |
| `queryResumable`     | Can the AGV resume at the moment?                                                      |
| `queryAcceptZoneSet` | Can the library accept the given ZoneSet?                                              |

Example implementation stubs:
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
      // Empty iff. ok
    };
  }
};
```


## InitPositionHandler

The [`vda5050pp::handler::InitPositionHandler`](doxygen/html/classvda5050pp_1_1handler_1_1InitPositionHandler.html) is a pre-defined handler
for the `initPosition` instant action.

You can use it like this:
```c++
class MyInitPositionHandler : public vda5050pp::handler::InitPositionHandler {
public:
  void handleInitPosition(double x, double y, double theta, std::string_view map_id,
                          std::string_view last_node_id,
                          vda5050pp::handler::ActionState &action_state) override {
    // Init AGV's position

    this->setLastNodeId(last_node_id);
    action_state.setFinished();
  }
};
```

## SimpleActionHandler

The [`vda5050pp::handler::SimpleActionHandler`](doxygen/html/classvda5050pp_1_1handler_1_1SimpleActionHandler.html)
is a base class for simpler action handlers. It already takes care of `match`, `validate` and `getActionDescription`.
You only have to implement `vda5050pp::handler::BaseActionHandler::prepare` and pass 
a `vda5050pp::agv_description::ActionDeclaration` to it's constructor.

Example implementation:
```c++
class MySimpleActionHandler : public vda5050pp::handler::SimpleActionHandler {
public:
  // Setup SimpleActionHandler to handle the default start_charging action
  MySimpleActionHandler()
      : vda5050pp::handler::SimpleActionHandler(
            vda5050pp::misc::action_declarations::k_start_charging) {}

  vda5050pp::handler::ActionCallbacks prepare(
      std::shared_ptr<vda5050pp::handler::ActionState> action_state,
      std::shared_ptr<vda5050pp::handler::ParametersMap> parameters) noexcept(false) override {
    vda5050pp::handler::ActionCallbacks cbs;
    cbs.on_start = [](vda5050pp::handler::ActionState &state) {
      // Instruct AGV to start charging

      // Set action to running
      state.setRunning();

      // Await AGV status
      state.setFinished();
    };
    cbs.on_pause = [](vda5050pp::handler::ActionState &state) {
      // Instruct AGV to pause charging
      state.setPaused();
    };
    cbs.on_resume = [](vda5050pp::handler::ActionState &state) {
      // Instruct AGV to resume charging
      state.setRunning();
    };
    cbs.on_cancel = [](vda5050pp::handler::ActionState &state) {
      // Instruct AGV to cancel charging
      state.setFailed();
    };

    return cbs;
  }
};
```

## SimpleMultiActionHandler

The [`vda5050pp::handler::SimpleMultiActionHandler`](doxygen/html/classvda5050pp_1_1handler_1_1SimpleMultiActionHandler.html)
is a base class for multiple simple action handlers. It already takes care of `match`, `validate` and `getActionDescription`.
You only have to implement `vda5050pp::handler::BaseActionHandler::prepare` and pass 
a set of `vda5050pp::agv_description::ActionDeclaration`s to it's constructor.

Example implementation:
```c++
class MySimpleMultiActionHandler : public vda5050pp::handler::SimpleMultiActionHandler {
public:
  MySimpleMultiActionHandler()
      : vda5050pp::handler::SimpleMultiActionHandler(
            {vda5050pp::misc::action_declarations::k_start_charging,
             vda5050pp::misc::action_declarations::k_stop_charging}) {}

  vda5050pp::handler::ActionCallbacks handleStartCharging(
      std::shared_ptr<vda5050pp::handler::ActionState> action_state,
      std::shared_ptr<vda5050pp::handler::ParametersMap> parameters);

  vda5050pp::handler::ActionCallbacks handleStopCharging(
      std::shared_ptr<vda5050pp::handler::ActionState> action_state,
      std::shared_ptr<vda5050pp::handler::ParametersMap> parameters);

  vda5050pp::handler::ActionCallbacks prepare(
      std::shared_ptr<vda5050pp::handler::ActionState> action_state,
      std::shared_ptr<vda5050pp::handler::ParametersMap> parameters) noexcept(false) override {
    if (action_state->getAction().actionId == "startCharging") {
      return handleStartCharging(action_state, parameters);
    } else {
      return handleStopCharging(action_state, parameters);
    };
  }
};
```

# Observer

The Observers in the [`vda5050pp::observer` namespace](doxygen/html/namespacevda5050pp_1_1observer.html) can be used
to observe the internal library state, such as messages and the currently processed Order.

## MessageObserver

During it's lifetime, the [`vda5050pp::observer::MessageObserver`](doxygen/html/classvda5050pp_1_1observer_1_1MessageObserver.html)
subscribes to all necessary events and tracks statistics about:

- the number of valid received Orders
- the number of valid received InstantActions
- the number of sent State messages
- the number of sent Visualization messages
- the number of message errors (deserialization or delivery errors)

Furthermore the user can add custom callbacks to observe these events directly.

### Example

```c++
// Start observing message events
vda5050pp::observers::MessageObserver observer;

// Observe changes in the connection state
observer.onConnectionChanged([](vda5050pp::misc::ConnectionStatus status) {
  // ...
});

// ...

observer.getErrors(); // Get the number of encountered message errors
auto [type, description] = observer.getLastError().value(); // Inspect the latest error.
```

## OrderObserver

During it's lifetime, the [`vda5050pp::observer::OrderObserver`](doxygen/html/classvda5050pp_1_1observer_1_1MessageObserver.html)
subscribes to all necessary events and provides the current state of:

- the high level order state (idle, active, paused, ...)
- the state of each action
- the logical position of the AGV (node_id, sequence_id)

The user can provide custom callbacks to get notified on each change.

### Example

```c++
// Start observing the order events
vda5050pp::observer::OrderObserver observer;

observer.onActionStatusChanged("action_id", [](vda5050::ActionStatus status){
  // ...
});

// ...

auto [last_node_id, seq_id] = observer.getLastNode().value(); // Get the logical AGV position
```

# Misc

Overall the [`vda5050pp::misc` namespace](doxygen/html/namespacevda5050pp_1_1misc.html)
contains auxiliary data structures.

## ActionContext

The `vda5050pp::misc::ActionContext` describes the context, in which an action was received.
It will be used if you implement a custom action validation.

## ActionDeclarations

The [`vda5050pp::misc::action_declarations` namespace](doxygen/html/namespacevda5050pp_1_1misc_1_1action__declarations.html) contains all default
action declarations, which can be used (and modified) in combination
with the [`vda5050pp::handler::SimpleActionHandler`](doxygen/html/classvda5050pp_1_1handler_1_1SimpleActionHandler.html)
and the [`vda5050pp::handler::SimpleMultiActionHandler`](doxygen/html/classvda5050pp_1_1handler_1_1SimpleMultiActionHandler.html).

## ActionParameterView

The [`vda5050pp::misc::ActionParameterView`](doxygen/html/classvda5050pp_1_1misc_1_1ActionParameterView.html)
can be used to conveniently view a `std::vector<vda5050::ActionParameter>`. This might be handy if
you implement your own action validation.

Example usage:
```c++
std::vector<vda5050::ActionParameter> params;
vda5050pp::misc::ActionParameterView view(params);

if (auto maybe_height = view.tryGetFloat("height"); maybe_height) {
  double &height = *maybe_height;
}
```

## AnyPtr

The [`vda5050pp::misc::AnyPtr`](doxygen/html/classvda5050pp_1_1misc_1_1AnyPtr.html) is 
a type-safe `std::shared_ptr<void>`. It is used to safely vanish and restore types.
You can use it to store objects of any type in a [`vda5050pp::handler::ParameterMap`](doxygen/html/namespacevda5050pp_1_1handler.html).

Example usage:
```c++
auto my_vector_ptr = std::make_shared<std::vector<int>>();

// Vanish type
vda5050pp::misc::AnyPtr my_any_ptr = my_vector_ptr;

// Still holds reference after the original ptr was destroyed
my_vector_ptr.reset();

// Restore type
std::shared_ptr<int> my_failed_ptr = my_any_ptr;                 // Throws exception
std::shared_ptr<std::vector<int>> my_restored_ptr = my_any_ptr;  // OK
```

## OrderStatus

The [`vda5050pp::misc::OrderStatus`] represents the internal state of the order interpreter.
There will be a public event interface for internal events in further versions.