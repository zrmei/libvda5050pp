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

extern const std::string_view k_library_version;

///
///\brief The current vda5050 version number
///
extern const std::string_view k_current;

///
///\brief All compatible vda5050 version numbers
///
extern const std::set<std::string_view, std::less<>> k_compatible;

}  // namespace vda5050pp::version

#endif  // INCLUDE_PUBLIC_VDA5050_2B_2B_VERSION_H_
