// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_OBSERVER_MESSAGE_OBSERVER_H_
#define PUBLIC_VDA5050_2B_2B_OBSERVER_MESSAGE_OBSERVER_H_

#include <vda5050/AgvFactsheet.h>
#include <vda5050/InstantActions.h>
#include <vda5050/Order.h>
#include <vda5050/State.h>
#include <vda5050/Visualization.h>

#include <atomic>
#include <cstdint>
#include <functional>
#include <optional>
#include <shared_mutex>

#include "vda5050++/misc/any_ptr.h"
#include "vda5050++/misc/connection_status.h"
#include "vda5050++/misc/message_error.h"

namespace vda5050pp::observer {

///
///\brief The MessageObserver class is used observe the messaging status inside the library.
///
class MessageObserver {
private:
  std::atomic_uint32_t received_order_messages_ = 0;
  std::atomic_uint32_t received_instant_actions_messages_ = 0;
  std::atomic_uint32_t sent_factsheet_messages_ = 0;
  std::atomic_uint32_t sent_state_messages_ = 0;
  std::atomic_uint32_t sent_visualization_messages_ = 0;
  std::atomic_uint32_t errors_ = 0;

  mutable std::shared_mutex last_error_message_mutex_;
  std::optional<std::pair<vda5050pp::misc::MessageErrorType, std::string>> last_error_message_;
  mutable std::shared_mutex connection_status_mutex_;
  std::optional<vda5050pp::misc::ConnectionStatus> connection_status_;

  vda5050pp::misc::AnyPtr opaque_state_;

public:
  ///
  ///\brief Construct a new Message Observer object (requires a running instance)
  ///
  MessageObserver();

  ///
  ///\brief Get the number of received order messages
  ///
  ///\return uint32_t the number of received order messages
  ///
  uint32_t getReceivedOrderMessages() const;

  ///
  ///\brief Get the number of received instant actions messages
  ///
  ///\return uint32_t the number of received instant actions messages
  ///
  uint32_t getReceivedInstantActionsMessages() const;

  ///
  ///\brief Get the number of sent factsheet messages
  ///
  ///\return uint32_t the number of sent factsheet messages
  ///
  uint32_t getSentFactsheetMessages() const;

  ///
  ///\brief Get the number of sent state messages
  ///
  ///\return uint32_t the number of sent state messages
  ///
  uint32_t getSentStateMessages() const;

  ///
  ///\brief Get the number of sent visualization messages
  ///
  ///\return uint32_t the number of sent visualization messages
  ///
  uint32_t getSentVisualizationMessages() const;

  ///
  ///\brief Get the number of errors
  ///
  ///\return uint32_t the number of errors
  ///
  uint32_t getErrors() const;

  ///
  ///\brief Get the last error message
  ///
  ///\return std::optional<std::pair<vda5050pp::misc::MessageErrorType, std::string>> the last error
  /// message
  ///
  std::optional<std::pair<vda5050pp::misc::MessageErrorType, std::string>> getLastError() const;

  ///
  ///\brief Get the connection status
  ///
  ///\return std::optional<vda5050pp::misc::ConnectionStatus> the connection status
  ///
  std::optional<vda5050pp::misc::ConnectionStatus> getConnectionStatus() const;

  ///
  ///\brief Add a callback to be called when a valid order message is received
  ///
  ///\param callback the callback to be called
  ///
  void onValidOrderMessage(std::function<void(std::shared_ptr<const vda5050::Order>)> callback);

  ///
  ///\brief Add a callback to be called when a valid instant actions message is received
  ///
  ///\param callback the callback to be called
  ///
  void onValidInstantActionsMessage(
      std::function<void(std::shared_ptr<const vda5050::InstantActions>)> callback);

  ///
  ///\brief Add a callback to be called when a factsheet message is sent
  ///
  ///\param callback the callback to be called
  ///
  void onSentFactsheetMessage(
      std::function<void(std::shared_ptr<const vda5050::AgvFactsheet>)> callback);

  ///
  ///\brief Add a callback to be called when a state message is sent
  ///
  ///\param callback the callback to be called
  ///
  void onSentStateMessage(std::function<void(std::shared_ptr<const vda5050::State>)> callback);

  ///
  ///\brief Add a callback to be called when a visualization message is sent
  ///
  ///\param callback the callback to be called
  ///
  void onSentVisualizationMessage(
      std::function<void(std::shared_ptr<const vda5050::Visualization>)> callback);

  ///
  ///\brief Add a callback to be called when the connection status changes
  ///
  ///\param callback the callback to be called
  ///
  void onConnectionChanged(std::function<void(vda5050pp::misc::ConnectionStatus)> callback);

  ///
  ///\brief Add a callback to be called when a message error occurs
  ///
  ///\param callback the callback to be called
  ///
  void onMessageError(
      std::function<void(std::pair<vda5050pp::misc::MessageErrorType, std::string_view>)> callback);
};

}  // namespace vda5050pp::observer
#endif  // PUBLIC_VDA5050_2B_2B_OBSERVER_MESSAGE_OBSERVER_H_
