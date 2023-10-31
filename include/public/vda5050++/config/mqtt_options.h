//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_CONFIG_MQTT_OPTIONS_H_
#define PUBLIC_VDA5050_2B_2B_CONFIG_MQTT_OPTIONS_H_

#include <chrono>
#include <optional>
#include <string>

namespace vda5050pp::config {
///
///\brief Configuration struct for the MqttConnector.
///
struct MqttOptions {
  ///\brief username used for authentication
  std::optional<std::string> username;
  ///\brief password used for authentication
  std::optional<std::string> password;
  ///\brief overwrites <version> in all topics /<iface>/<version>/<manufacturer>/<sn>
  std::optional<std::string> version_overwrite;
  ///\brief sets the server address (use "ssl://<address>:<port>" or "tcp://<address>:<port>")
  std::string server;
  ///\brief sets <iface> in all topics /<iface>/<version>/<manufacturer>/<sn>
  std::string interface;
  ///\brief enable ssl certificate check
  bool enable_cert_check = true;
  ///\brief enable ssl
  bool use_ssl = true;
  ///\brief Min delay to reconnect after connection loss (doubled each retry)
  std::optional<std::chrono::system_clock::duration> min_retry_interval_;
  ///\brief Max delay to reconnect after connection loss
  std::optional<std::chrono::system_clock::duration> max_retry_interval_;
  ///\brief Keep-Alive interval
  std::optional<std::chrono::system_clock::duration> keep_alive_interval_;
  ///\brief Timeout duration during connect
  std::optional<std::chrono::system_clock::duration> connect_timeout_;
};

}  // namespace vda5050pp::config

#endif  // PUBLIC_VDA5050_2B_2B_CONFIG_MQTT_OPTIONS_H_
