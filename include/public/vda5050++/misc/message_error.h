// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_MISC_MESSAGE_ERROR_H_
#define PUBLIC_VDA5050_2B_2B_MISC_MESSAGE_ERROR_H_

namespace vda5050pp::misc {

///
///\brief The MessageErrorType enum describes the type of error that occurred during message
/// handling
///
enum class MessageErrorType {
  k_delivery,
  k_json_deserialization,
};

}  // namespace vda5050pp::misc

#endif  // PUBLIC_VDA5050_2B_2B_MISC_MESSAGE_ERROR_H_
