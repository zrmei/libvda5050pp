// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/core/common/type_traits.h"

#if LIBVDA5050PP_HAS_DEMANGLE
#include <cxxabi.h>
#endif

std::string vda5050pp::core::common::demangle(const char *mangled_cxx_name) {
#if LIBVDA5050PP_HAS_DEMANGLE
  if (auto raw_demangled = abi::__cxa_demangle(mangled_cxx_name, nullptr, nullptr, nullptr);
      raw_demangled) {
    std::string str_demangled(raw_demangled);
    free(raw_demangled);
    return str_demangled;
  } else {
    return "<not demanglable>";
  }
#else
  return std::string(mangled_cxx_name);
#endif
}

std::string vda5050pp::core::common::demangle(const std::string &mangled_cxx_name) {
#if LIBVDA5050PP_HAS_DEMANGLE
  return vda5050pp::core::common::demangle(mangled_cxx_name.c_str());
#else
  return mangled_cxx_name;
#endif
}