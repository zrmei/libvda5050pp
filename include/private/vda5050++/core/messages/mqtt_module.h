// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
//

#ifndef VDA5050_2B_2B_CORE_MESSAGES_MQTT_MODULE_H_
#define VDA5050_2B_2B_CORE_MESSAGES_MQTT_MODULE_H_

#include <mqtt/async_client.h>
#include <vda5050/AgvFactsheet.h>
#include <vda5050/Connection.h>
#include <vda5050/State.h>
#include <vda5050/Visualization.h>

#include <map>
#include <memory>
#include <optional>

#include "vda5050++/agv_description/agv_description.h"
#include "vda5050++/config/mqtt_options.h"
#include "vda5050++/core/module.h"

namespace vda5050pp::core::messages {

///
///\brief An Mqtt implementation for message retrieval
///
class MqttModule final : public vda5050pp::core::Module,
                         public mqtt::callback,
                         public mqtt::iaction_listener {
private:
  enum class State {
    k_constructed,
    k_initialized,
    k_offline,
    k_online,
    k_error,
  };
  State state_ = State::k_constructed;

  std::optional<
      vda5050pp::core::GenericEventManager<vda5050pp::core::events::MessageEvent>::ScopedSubscriber>
      m_subscriber_;
  std::optional<
      vda5050pp::core::GenericEventManager<vda5050pp::core::events::ControlEvent>::ScopedSubscriber>
      c_subscriber_;

  uint32_t connection_seq_id_ = 0;
  uint32_t factsheet_seq_id_ = 0;
  uint32_t state_seq_id_ = 0;
  uint32_t visualization_seq_id_ = 0;
  std::unique_ptr<mqtt::async_client> mqtt_client_;
  std::string server_;
  mqtt::connect_options connect_opts_;
  std::string connection_topic_;
  std::string factsheet_topic_;
  std::string instant_actions_topic_;
  std::string order_topic_;
  std::string state_topic_;
  std::string visualization_topic_;
  std::string manufacturer_;
  std::string serial_number_;

  const int k_qos = 0;

  void fillHeaderConnection(vda5050::HeaderVDA5050 &header);
  void fillHeaderFactsheet(vda5050::HeaderVDA5050 &header);
  void fillHeaderState(vda5050::HeaderVDA5050 &header);
  void fillHeaderVisualization(vda5050::HeaderVDA5050 &header);
  mqtt::will_options getWill();

public:
  void useMqttOptions(const vda5050pp::config::MqttOptions &opts);

  /**
   * This method is invoked when an action fails.
   * @param asyncActionToken
   */
  void on_failure(const mqtt::token &asyncActionToken) override;
  /**
   * This method is invoked when an action has completed successfully.
   * @param asyncActionToken
   */
  void on_success(const mqtt::token &asyncActionToken) override;

  /**
   * This method is called when the client is connected.
   * Note that, in response to an initial connect(), the token from the
   * connect call is also signaled with an on_success(). That occurs just
   * before this is called.
   */
  void connected(const std::string & /*cause*/) override;
  /**
   * This method is called when the connection to the server is lost.
   */
  void connection_lost(const std::string & /*cause*/) override;
  /**
   * This method is called when a message arrives from the server.
   */
  void message_arrived(mqtt::const_message_ptr /*msg*/) override;
  /**
   * Called when delivery for a message has been completed, and all
   * acknowledgments have been received.
   */
  void delivery_complete(mqtt::delivery_token_ptr /*tok*/) override;

  void connect();

  void disconnect();

  void sendState(const vda5050::State &state) const noexcept(false);
  void sendFactsheet(const vda5050::AgvFactsheet &state) const noexcept(false);
  void sendVisualization(const vda5050::Visualization &visualization) const noexcept(false);
  void sendConnection(const vda5050::Connection &connection) const noexcept(false);

  void initialize(vda5050pp::core::Instance &instance) override;
  void deinitialize(vda5050pp::core::Instance &instance) override;
  std::string_view describe() const override;
  std::shared_ptr<vda5050pp::config::ModuleSubConfig> generateSubConfig() const override;
};

}  // namespace vda5050pp::core::messages

#endif  // VDA5050_2B_2B_CORE_MESSAGES_MQTT_MODULE_H_
