// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
//
#include "vda5050++/version.h"

#define STRING(x) #x
#define STRING_LITERAL(x) STRING(x)

const std::string_view vda5050pp::version::k_library_version = STRING_LITERAL(LIBVDA5050PP_VERSION);

const std::string_view vda5050pp::version::k_current = "2.0.0";

const std::set<std::string_view, std::less<>> vda5050pp::version::k_compatible = {
    vda5050pp::version::k_current};