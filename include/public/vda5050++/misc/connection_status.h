// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_MISC_CONNECTION_STATUS_H_
#define PUBLIC_VDA5050_2B_2B_MISC_CONNECTION_STATUS_H_

namespace vda5050pp::misc {

///
///\brief The Connection Status of the libraries messaging module
///
enum class ConnectionStatus {
  k_online,
  k_offline,
  k_timeout,
};

}  // namespace vda5050pp::misc

#endif  // PUBLIC_VDA5050_2B_2B_MISC_CONNECTION_STATUS_H_
