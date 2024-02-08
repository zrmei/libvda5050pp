# Configuration of the libVDA5050++

The libVDA5050++ can be fully configured by the [`vda5050pp::Config`](doxygen/html/classvda5050pp_1_1Config.html).
It contains a [global](#global-config) configuration, an [AGV description](#agv-description) and
several [module configurations](#module-configuration). There is also
support for used defined config tables. An example configuration file can
be found in `libvda5050++/examples/example_config.toml`.

# Load/Save

The default configuration is created with
```c++
#include <vda5050++/config.h>

vda5050pp::Config cfg;
```
you can access all configuration options through this object.

The libVDA5050++ supports [TOML](https://toml.io/en/) serialization of the whole configuration:

```c++
using namespace std::string_view_literals;

// Load
vda5050pp::Config cfg = vda5050pp::Config::loadFrom("<this_is_a_toml_string>"sv);  // or
cfg = vda5050pp::Config::loadFrom(std::filesystem::path("/path/to/config.toml"));  // or
cfg.load("<this_is_a_toml_string>"sv);                                             // or
cfg.load(std::filesystem::path("/path/to/config.toml"));

// Save
std::string toml_string;
cfg.save(toml_string);  // or
cfg.save(std::filesystem::path("/path/to/config.toml"));
```


# Global Config

The global config can be accessed via `vda5050pp::Config::refGlobalConfig`
and will be serialized in the `[global]` table.
The current options won't concern most users.

| key                                              | description                                                                   |
| ------------------------------------------------ | ----------------------------------------------------------------------------- |
| module_black_list                                | A list of module names, which will not be loaded (for modding purposes only). |
| event_manager_options.synchronous_event_dispatch | Disable all internal event threads, use direct dispatch only.                 |
| event_manager_options.synchronous_event_dispatch | Disable all internal event threads, use direct dispatch only.                 |
| log_level                                        | Default log level: `debug`, `info`, `warn`, `error` or `off`                  |
| log_file_name                                    | a file to write the log to. (currently unsupported)                           |

# AGV Description

The AGV Description can be accessed via `vda5050pp::Config::refAGVDescription` and
will be serialized in the `[agv_description]` table.

| key           | description                                                   | optional |
| ------------- | ------------------------------------------------------------- | -------- |
| agv_id        | The AGV ID to use. (Will be part of the MQTT ID)              | no       |
| manufacturer  | The manufacturer to use, mandatory for MQTT Topic selection.  | no       |
| serial_number | The serial number to use, mandatory for MQTT Topic selection. | no       |
| description   | A description of the AGV. Will be part of the factsheet.      | no       |

### `[agv_description.battery]` subtable

**Currently unused**

| key         | description                              |
| ----------- | ---------------------------------------- |
| charge_type | A string description of the charge type. |
| max_charge  | Maximum charge [Ah]                      |
| max_reach   | Maximum reach [m]                        |
| max_voltage | Maximum voltage [V]                      |

### `[agv_description.type_specification]` subtable

Contains JSON serialized [`factsheet.typeSpecification`](https://github.com/VDA5050/VDA5050/blob/2.0.0/VDA5050_EN_V1.md#typespecification).

| key                 | description                                                                                 |
| ------------------- | ------------------------------------------------------------------------------------------- |
| json                | A JSON serialized string of the type specification. Alternative of `json_file`              |
| json_file           | The path of a file containing the JSON serialized type specification. Alternative of `json` |
| json_file_overwrite | Allow the `vda5050pp::Config::save` to overwrite the file.                                  |

### `[agv_description.physical_parameters]` subtable

Contains JSON serialized [`factsheet.physicalParameters`](https://github.com/VDA5050/VDA5050/blob/2.0.0/VDA5050_EN_V1.md#physicalparameters).

| key                 | description                                                                                  |
| ------------------- | -------------------------------------------------------------------------------------------- |
| json                | A JSON serialized string of the physical parameters. Alternative of `json_file`              |
| json_file           | The path of a file containing the JSON serialized physical parameters. Alternative of `json` |
| json_file_overwrite | Allow the `vda5050pp::Config::save` to overwrite the file.                                   |

### `[agv_description.agv_geometry]` subtable

Contains JSON serialized [`factsheet.agvGeometry`](https://github.com/VDA5050/VDA5050/blob/2.0.0/VDA5050_EN_V1.md#agvgeometry).

| key                 | description                                                                           |
| ------------------- | ------------------------------------------------------------------------------------- |
| json                | A JSON serialized string of the AGV geometry. Alternative of `json_file`              |
| json_file           | The path of a file containing the JSON serialized AGV geometry. Alternative of `json` |
| json_file_overwrite | Allow the `vda5050pp::Config::save` to overwrite the file.                            |

### `[agv_description.load_specification]` subtable

Contains JSON serialized [`factsheet.loadSpecification`](https://github.com/VDA5050/VDA5050/blob/2.0.0/VDA5050_EN_V1.md#loadspecification).

| key                 | description                                                                                 |
| ------------------- | ------------------------------------------------------------------------------------------- |
| json                | A JSON serialized string of the load specification. Alternative of `json_file`              |
| json_file           | The path of a file containing the JSON serialized load specification. Alternative of `json` |
| json_file_overwrite | Allow the `vda5050pp::Config::save` to overwrite the file.                                  |

### `[agv_description.localization_parameters]` subtable

Contains JSON serialized `factsheet.localizationParameters`, since it was not defined in the original document,
we expect a `{ "type": "", "description": "" }` like object.

| key                 | description                                                                                      |
| ------------------- | ------------------------------------------------------------------------------------------------ |
| json                | A JSON serialized string of the localization parameters. Alternative of `json_file`              |
| json_file           | The path of a file containing the JSON serialized localization parameters. Alternative of `json` |
| json_file_overwrite | Allow the `vda5050pp::Config::save` to overwrite the file.                                       |


### `[agv_description.simple_protocol_limits]` subtable

It contains a simple form of the [factsheet.protocolLimits](https://github.com/VDA5050/VDA5050/blob/2.0.0/VDA5050_EN_V1.md#protocollimits)
defined in the VDA5050, that is only the parameters, which won't be set by the library itself.


| key               | description                                | optional |
| ----------------- | ------------------------------------------ | -------- |
| max_id_len        | Maximum length of IDs used in the protocol | yes      |
| id_numerical_only | Are IDs numerical only?                    | yes      |
| max_load_id_len   | Maximum length of load IDs                 | yes      |
| max_loads         | Maximum number of loads in the array       | yes      |

# Module Configuration

Several of the libVDA5050++'s internal modules can be configured.

### Logging

The logging behavior of each module can be controlled with the following keys inside of it's table:

| key           | description                                         | optional |
| ------------- | --------------------------------------------------- | -------- |
| log_level     | `debug`, `info`, `warn`, `error` or `off`           | yes      |
| log_file_name | a file to write the log to. (currently unsupported) | yes      |

There are modules, which cannot be configured yet. The logging can still be controlled.
If you see a log entry beginning with `[<module_name>]` that you want to control, simply set
the log variables in the table `[module.<module_name>]`.

In c++ you would do:
```c++
cfg.lookupModuleConfig("<module_name>")->setLogLevel(vda5050pp::config::LogLevel::k_debug);
```

### `[module.Mqtt]` subtable

The MQTT sub config is probably the most important one, since it describes
how to connect to the broker.

| key               | description                                                               | optional | default                |
| ----------------- | ------------------------------------------------------------------------- | -------- | ---------------------- |
| username          | MQTT username.                                                            | yes      | none                   |
| password          | MQTT password.                                                            | yes      | none                   |
| version_overwrite | Overwrite the version substring in the MQTT topic.                        | yes      | `v2`                   |
| server            | Set the broker address in this format `[tcp/ws/ssl/wss]://<host>:<port>`. | yes      | `tcp://localhost:1883` |
| interface         | Set the interface in the MQTT topic.                                      | yes      | `uagv`                 |
| enable_cert_check | Enable certificate checking in combination with ssl.                      | yes      | `false`                |
| use_ssl           | Enable ssl.                                                               | yes      | `false`                |

### `[module.NodeReachedHandler]` subtable

The NodeReachedHandler sub config contains configuration for the automatic detection of
reached nodes, e.g. `vda5050pp::handler::BaseNavigationHandler::evalPosition`.

| key                            | description                                                         | optional |
| ------------------------------ | ------------------------------------------------------------------- | -------- |
| default_node_deviation_xy      | If no `node.nodeDeviationXY` was received, use this one instead.    | yes      |
| default_node_deviation_theta   | If no `node.nodeDeviationTheta` was received, use this one instead. | yes      |
| overwrite_node_deviation_xy    | Always use this as `node.nodeDeviationXY`.                          | yes      |
| overwrite_node_deviation_theta | Always use this as `node.nodeDeviationTheta`.                       | yes      |

### `[module.QueryEventHandler]` subtable

The QueryEventHandler sub config contains the settings for default
[`vda5050pp::handler::BaseQueryHandler`](doxygen/html/classvda5050pp_1_1handler_1_1BaseQueryHandler.html) answers.

| key                             | description                                           | optional |
| ------------------------------- | ----------------------------------------------------- | -------- |
| default_pauseable_success       | The default success of pause query.                   | yes      |
| default_resumable_success       | The default success of resume query.                  | yes      |
| default_accept_zone_set_success | The default success the accept zone set query.        | yes      |
| default_zone_sets               | The default set of zone sets, which will be accepted. | yes      |

### `[module.StateUpdateTimer]` subtable

The StateUpdateTimer sub config contains the maximum interval of state messages.

| key                  | description                                | optional | default |
| -------------------- | ------------------------------------------ | -------- | ------- |
| max_update_period_ms | The maximum update period in milliseconds. | yes      | `30000` |

### `[module.VisualizationTimer]` subtable

The VisualizationTimer sub config contains the interval of visualization messages.

| key                     | description                               | optional | default |
| ----------------------- | ----------------------------------------- | -------- | ------- |
| visualization_period_ms | The visualization period in milliseconds. | yes      | `1000`  |

# Custom Configuration

You can add your own configuration to the [`vda5050pp::Config`](doxygen/html/classvda5050pp_1_1Config.html)
to load them from the same file. All user configurations are stored in the `[custom.*]` sub-table.
There is currently a support for simple key-value configurations with `boolean`, `float`, `integer`
and `string` values via the [`vda5050pp::config::KeyValueConfig`](doxygen/html/classvda5050pp_1_1config_1_1KeyValueConfig.html) class.

If your configuration looks like this for example:
```toml
# ...
[custom.my_service]
my_service_address = "192.168.0.100"
my_service_port = 8000
# ...
```

You can read and write it like this:
```c++
auto my_configuration = std::make_shared<vda5050pp::config::KeyValueConfig>();
my_configuration->addStringEntry("my_service_address",                              // key
                                 true,                                              // required
                                 "127.0.0.1",                                       // default
                                 [](auto v) { return isMyServiceAddressValid(v); }  // validator
);
my_configuration->addIntegerEntry("my_service_port",                              // key
                                  true,                                           // required
                                  8080,                                           // default
                                  [](auto v) { return isMyServicePortValid(v); }  // validator
);

vda5050pp::Config cfg;
cfg.registerCustomConfig("my_service", my_configuration);
cfg.load(std::filesystem::path("/path/to/config.toml")); // Will also load [custom.my_service]

// Access loaded values
cfg.lookupCustomConfigAs<vda5050pp::config::KeyValueConfig>("my_service")->getString("my_service_address");
cfg.lookupCustomConfigAs<vda5050pp::config::KeyValueConfig>("my_service")->getInteger("my_service_port");
```

You can also nest your [`vda5050pp::config::KeyValueConfig`](doxygen/html/classvda5050pp_1_1config_1_1KeyValueConfig.html) objects:
```c++
auto my_sub_configuration = std::make_shared<vda5050pp::KeyValueConfig>();
my_sub_configuration->addStringEntry("user", true, "admin");
my_sub_configuration->addStringEntry("pass", true, "admin");

my_configuration->addSubConfigEntry("auth", my_sub_configuration);

cfg.load(std::filesystem::path("/path/to/config.toml")); // Will load [custom.my_service] and [custom.my_service.auth]

// Access auth
cfg.lookupCustomConfigAs<vda5050pp::config::KeyValueConfig>("my_service")
  ->getSubConfigAs<vda5050pp::config::KeyValueConfig>("auth")
  ->getStringEntry("user");

```