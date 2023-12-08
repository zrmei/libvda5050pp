# Copyright Open Logistics Foundation
# 
# Licensed under the Open Logistics Foundation License 1.3.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: OLFL-1.3
#
# Populates the LIBVDA5050PP_PLATFORM_DEFINITIONS list
# 

include(CheckCXXSymbolExists)

set(LIBVDA5050PP_PLATFORM_DEFINITIONS "")

# Stacktrace support 
check_cxx_symbol_exists(backtrace execinfo.h LIBVDA5050PP_HAS_BACKTRACE)
check_cxx_symbol_exists(backtrace_symbols execinfo.h LIBVDA5050PP_HAS_BACKTRACE_SYMBOLS)

if (LIBVDA5050PP_HAS_BACKTRACE AND LIBVDA5050PP_HAS_BACKTRACE_SYMBOLS)
  list(APPEND LIBVDA5050PP_PLATFORM_DEFINITIONS "LIBVDA5050PP_HAS_BACKTRACE")
else()
  message(WARNING "No backtrace support. Exception Stacktraces will not be gathered.")
endif ()


# Demangle support
check_cxx_symbol_exists(abi::__cxa_demangle cxxabi.h LIBVDA5050PP_HAS_DEMANGLE)

if (LIBVDA5050PP_HAS_DEMANGLE)
  list(APPEND LIBVDA5050PP_PLATFORM_DEFINITIONS "LIBVDA5050PP_HAS_DEMANGLE")
else()
  message(WARNING "No demangle support. Symbols will not be demangled.")
endif ()