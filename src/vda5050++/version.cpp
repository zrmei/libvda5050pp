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

static const std::string_view k_library_version = STRING_LITERAL(LIBVDA5050PP_VERSION);

static const std::string_view k_current = "2.0.0";

static const std::set<std::string_view, std::less<>> k_compatible = {k_current};

std::string_view vda5050pp::version::getLibraryVersion() { return k_library_version; }

std::string_view vda5050pp::version::getCurrentVersion() { return k_current; }

const std::set<std::string_view, std::less<>> &vda5050pp::version::getCompatible() {
  return k_compatible;
}
