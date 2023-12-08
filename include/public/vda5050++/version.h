// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
//
// This file contains the version constants
//

#ifndef INCLUDE_PUBLIC_VDA5050_2B_2B_VERSION_H_
#define INCLUDE_PUBLIC_VDA5050_2B_2B_VERSION_H_

#include <set>
#include <string_view>

namespace vda5050pp::version {

///
///\brief Get the current library version
///
std::string_view getLibraryVersion();

///
///\brief The current vda5050 version number
///
std::string_view getCurrentVersion();

///
///\brief All compatible vda5050 version numbers
///
const std::set<std::string_view, std::less<>> &getCompatible();

}  // namespace vda5050pp::version

#endif  // INCLUDE_PUBLIC_VDA5050_2B_2B_VERSION_H_
