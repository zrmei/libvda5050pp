//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_CONFIG_EVENT_MANAGER_OPTIONS_H_
#define PUBLIC_VDA5050_2B_2B_CONFIG_EVENT_MANAGER_OPTIONS_H_

namespace vda5050pp::config {

///
///\brief The EventManagerOptions struct contains all common settings for the internal
/// EventManagers.
///
struct EventManagerOptions {
  bool synchronous_event_dispatch = false;
};

}  // namespace vda5050pp::config

#endif  // PUBLIC_VDA5050_2B_2B_CONFIG_EVENT_MANAGER_OPTIONS_H_
