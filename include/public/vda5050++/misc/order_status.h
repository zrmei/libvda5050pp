// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_MISC_ORDER_STATUS_H_
#define PUBLIC_VDA5050_2B_2B_MISC_ORDER_STATUS_H_

namespace vda5050pp::misc {

///
///\brief The OrderStatus Type represents the process state of the current order.
///
enum class OrderStatus {
  ///
  ///\brief the order is active and running.
  ///
  k_order_active,

  ///
  ///\brief There is no order / the last order was finished.
  ///
  k_order_idle,

  ///
  ///\brief There is (no order / the last order was finished) and the library is paused
  ///
  k_order_idle_paused,

  ///
  ///\brief The order is currently beeing canceled.
  ///
  k_order_canceling,

  ///
  ///\brief The order is currently beeing paused.
  ///
  k_order_pausing,

  ///
  ///\brief The order is currently beeing resumed.
  ///
  k_order_resuming,

  ///
  ///\brief The order is currently paused.
  ///
  k_order_paused,

  ///
  ///\brief The order is currently failed.
  ///
  k_order_failed,

  ///
  ///\brief The order is currently beeing interrupted.
  ///
  k_order_interrupting,
};

}  // namespace vda5050pp::misc

#endif  // PUBLIC_VDA5050_2B_2B_MISC_ORDER_STATUS_H_
