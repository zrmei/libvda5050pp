//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_MISC_ACTION_CONTEXT_H_
#define PUBLIC_VDA5050_2B_2B_MISC_ACTION_CONTEXT_H_

namespace vda5050pp::misc {

/// The ActionContext type.
enum class ActionContext {
  ///\brief invalid
  k_unspecified,
  ///\brief instant action
  k_instant,
  ///\brief edge action
  k_edge,
  ///\brief node action
  k_node,
};

}  // namespace vda5050pp::misc

#endif  // PUBLIC_VDA5050_2B_2B_MISC_ACTION_CONTEXT_H_
