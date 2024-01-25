// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_OBSERVER_ORDER_OBSERVER_H_
#define PUBLIC_VDA5050_2B_2B_OBSERVER_ORDER_OBSERVER_H_

#include <vda5050/ActionStatus.h>
#include <vda5050/Node.h>

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <shared_mutex>
#include <string>
#include <utility>

#include "vda5050++/misc/any_ptr.h"
#include "vda5050++/misc/order_status.h"

namespace vda5050pp::observer {

///
///\brief The OrderObserver class is used observe the order status inside the library.
///
class OrderObserver {
private:
  mutable std::shared_mutex last_node_mutex_;
  std::optional<std::string> last_node_id_;
  std::optional<decltype(vda5050::Node::sequenceId)> last_seq_id_;

  mutable std::shared_mutex action_status_mutex_;
  std::map<std::string, vda5050::ActionStatus, std::less<>> action_status_;

  mutable std::shared_mutex order_status_mutex_;
  std::optional<vda5050pp::misc::OrderStatus> order_status_;

  vda5050pp::misc::AnyPtr opaque_state_;

public:
  ///
  ///\brief Construct a new Order Observer object (requires a running instance)
  ///
  OrderObserver();

  ///
  ///\brief Get the last observed node id and sequence id
  ///
  ///\return std::pair<std::string, decltype(vda5050::Node::sequenceId)> the last observed node
  /// id and seq id
  ///
  std::optional<std::pair<std::string, decltype(vda5050::Node::sequenceId)>> getLastNode() const;

  ///
  ///\brief Return the last action status observed for the given action id
  ///
  ///\param action_id the action id
  ///\return const vda5050::ActionStatus& the last observed status for action_id
  ///
  std::optional<vda5050::ActionStatus> getActionStatus(std::string_view action_id) const;

  ///
  ///\brief Get the last observed OrderStatus
  ///
  ///\return vda5050pp::misc::OrderStatus the last observed OrderStatus
  ///
  std::optional<vda5050pp::misc::OrderStatus> getOrderStatus() const;

  ///
  ///\brief Add a callback to be called when the order status changes
  ///
  ///\param callback the callback to be called
  ///
  void onOrderStatusChanged(std::function<void(vda5050pp::misc::OrderStatus)> callback);

  ///
  ///\brief Add a callback to be called when the action status changes
  ///
  ///\param action_id the action id to observe
  ///\param callback the callback to be called
  ///
  void onActionStatusChanged(std::string_view action_id,
                             std::function<void(vda5050::ActionStatus)> callback);

  ///
  ///\brief Add a callback to be called when the last node id changes
  ///
  ///\param callback the callback to be called
  ///
  void onLastNodeIdChanged(
      std::function<void(std::pair<std::string_view, decltype(vda5050::Node::sequenceId)>)>
          callback);
};

}  // namespace vda5050pp::observer

#endif  // PUBLIC_VDA5050_2B_2B_OBSERVER_ORDER_OBSERVER_H_
