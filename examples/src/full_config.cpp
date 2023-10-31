// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include <vda5050++/config.h>

int main() {
  using namespace std::chrono_literals;
  vda5050pp::Config cfg;
  cfg.refAgvDescription().agv_id = "my_agv_id";
  cfg.refAgvDescription().description = "my_agv_description";
  cfg.refAgvDescription().manufacturer = "my_manufacturer";
  cfg.refAgvDescription().serial_number = "my_serial_number";
  cfg.refAgvDescription().simple_protocol_limits.id_numerical_only = false;
  cfg.refAgvDescription().simple_protocol_limits.max_id_len = 12;
  cfg.refAgvDescription().simple_protocol_limits.max_load_id_len = 12;
  cfg.refAgvDescription().simple_protocol_limits.max_loads = 2;
  cfg.refAgvDescription().battery.charge_type = "Li-Ion";
  cfg.refAgvDescription().battery.max_charge = 3.7;
  cfg.refAgvDescription().battery.max_reach = 10000;
  cfg.refAgvDescription().battery.max_voltage = 24.0;

  cfg.refAgvDescription().physical_parameters.accelerationMax = 1.0;
  cfg.refAgvDescription().physical_parameters.decelerationMax = 2.5;
  cfg.refAgvDescription().physical_parameters.heightMax = 0.6;
  cfg.refAgvDescription().physical_parameters.heightMin = 0.5;
  cfg.refAgvDescription().physical_parameters.length = 1.3;
  cfg.refAgvDescription().physical_parameters.speedMax = 2.4;
  cfg.refAgvDescription().physical_parameters.speedMin = 0.1;
  cfg.refAgvDescription().physical_parameters.width = 0.45;

  cfg.refAgvDescription().type_specification.agvClass = "CARRIER";
  cfg.refAgvDescription().type_specification.agvKinematic = "OMNI";
  cfg.refAgvDescription().type_specification.localizationTypes = {"GRID", "RFID"};
  cfg.refAgvDescription().type_specification.maxLoadMass = 120.0;
  cfg.refAgvDescription().type_specification.navigationTypes = {"AUTONOMOUS",
                                                                "VIRTUAL_LINE_GUIDED"};
  cfg.refAgvDescription().type_specification.seriesDescription = "My AGV series description";
  cfg.refAgvDescription().type_specification.seriesName = "TEST_SERIES_1";

  cfg.refAgvDescription().localization_parameters.description =
      "These are my localization parameters";
  cfg.refAgvDescription().localization_parameters.type = "The type of my localization parameters";

  cfg.refAgvDescription().load_specification.loadPositions = {"LEFT", "RIGHT"};
  cfg.refAgvDescription().load_specification.loadSets = {
      {{"load_set_1",
        "EPAL",
        {{"LEFT"}},
        vda5050::BoundingBoxReference{0.4, 0.3, 0.2, std::nullopt},
        std::nullopt,
        60.0,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        std::nullopt,
        "This is load set 1"}}};

  cfg.refAgvDescription().agv_geometry.envelopes2d = {
      {"env1",
       {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}, {0.0, 0.0}},
       "Envelope one description"}};
  cfg.refAgvDescription().agv_geometry.envelopes3d = {{"env2",
                                                       "my_format",
                                                       {"my_data_type", "my_data_desc"},
                                                       "http://example.com/my_envelope.json",
                                                       "this is a description of envelope 2"}};
  cfg.refAgvDescription().agv_geometry.wheelDefinitions = {{vda5050::WheelType::CASTER,
                                                            true,
                                                            true,
                                                            {0.0, 0.0, 0.0},
                                                            0.1,
                                                            0.1,
                                                            std::nullopt,
                                                            std::nullopt},
                                                           {vda5050::WheelType::CASTER,
                                                            true,
                                                            true,
                                                            {1.0, 1.0, 1.0},
                                                            0.1,
                                                            0.1,
                                                            std::nullopt,
                                                            std::nullopt}};

  cfg.refGlobalConfig().refEventManagerOptions().synchronous_event_dispatch = true;
  cfg.refGlobalConfig().setLogLevel(vda5050pp::config::LogLevel::k_debug);
  cfg.refGlobalConfig().useBlackList();
  cfg.refGlobalConfig().bwListModule("NodeReachedHandler");

  cfg.refMqttSubConfig().setLogLevel(vda5050pp::config::LogLevel::k_debug);
  cfg.refMqttSubConfig().refOptions().connect_timeout_ = 10s;
  cfg.refMqttSubConfig().refOptions().enable_cert_check = true;
  cfg.refMqttSubConfig().refOptions().interface = "vda5050_iface";
  cfg.refMqttSubConfig().refOptions().keep_alive_interval_ = 5s;
  cfg.refMqttSubConfig().refOptions().max_retry_interval_ = 10s;
  cfg.refMqttSubConfig().refOptions().min_retry_interval_ = 1s;
  cfg.refMqttSubConfig().refOptions().password = "p4$$w0rd";
  cfg.refMqttSubConfig().refOptions().server = "ssl://mqtt.example.com:8883";
  cfg.refMqttSubConfig().refOptions().use_ssl = true;
  cfg.refMqttSubConfig().refOptions().username = "mqtt_username";
  cfg.refMqttSubConfig().refOptions().version_overwrite = "v2";

  cfg.refNodeReachedSubConfig().setLogLevel(vda5050pp::config::LogLevel::k_debug);
  cfg.refNodeReachedSubConfig().setDefaultNodeDeviationTheta(0.1);
  cfg.refNodeReachedSubConfig().setDefaultNodeDeviationXY(0.1);
  cfg.refNodeReachedSubConfig().setOverwriteNodeDeviationTheta(0.1);
  cfg.refNodeReachedSubConfig().setOverwriteNodeDeviationXY(0.1);

  cfg.refQueryEventHandlerSubConfig().setLogLevel(vda5050pp::config::LogLevel::k_debug);
  cfg.refQueryEventHandlerSubConfig().setDefaultAcceptZoneSets({"zone1", "zone2", "zone3"});
  cfg.refQueryEventHandlerSubConfig().setDefaultAcceptZoneSetSuccess(true);
  cfg.refQueryEventHandlerSubConfig().setDefaultPauseableSuccess(true);
  cfg.refQueryEventHandlerSubConfig().setDefaultResumableSuccess(true);

  cfg.refStateUpdateTimerSubConfig().setLogLevel(vda5050pp::config::LogLevel::k_debug);
  cfg.refStateUpdateTimerSubConfig().setMaxUpdatePeriod(24s);

  cfg.refVisualizationTimerSubConfig().setLogLevel(vda5050pp::config::LogLevel::k_debug);
  cfg.refVisualizationTimerSubConfig().setVisualizationPeriod(100ms);

  cfg.save(std::filesystem::path("example_config.toml"));
}