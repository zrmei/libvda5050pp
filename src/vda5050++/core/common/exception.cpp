// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#include "vda5050++/core/common/exception.h"

#if LIBVDA5050PP_HAS_BACKTRACE
#include <execinfo.h>
#endif

#include "vda5050++/core/common/type_traits.h"

inline vda5050pp::VDA5050PPErrorContext::StackEntry fromBacktrace(
    std::string_view backtrace_entry) {
  auto b_fn = backtrace_entry.find_last_of('(') + 1;
  auto b_addr = backtrace_entry.find('+', b_fn) + 1;
  auto e_addr = backtrace_entry.find(')', b_addr);

  return {
      std::string(backtrace_entry.substr(0, b_fn - 1)),
      vda5050pp::core::common::demangle(
          std::string(backtrace_entry.substr(b_fn, b_addr - b_fn - 1))),
      std::string(backtrace_entry.substr(b_addr, e_addr - b_addr - 1)),
  };
}

std::vector<vda5050pp::VDA5050PPErrorContext::StackEntry> vda5050pp::core::common::stacktrace(
    size_t trace_size) {
#if LIBVDA5050PP_HAS_BACKTRACE
  auto trace = static_cast<void **>(malloc(trace_size * sizeof(void *)));
  trace_size = size_t(backtrace(trace, int(trace_size)));
  auto symbols = backtrace_symbols(trace, int(trace_size));

  std::vector<vda5050pp::VDA5050PPErrorContext::StackEntry> bt;
  bt.reserve(trace_size);
  for (size_t i = 0; i < trace_size; i++) {
    bt.push_back(fromBacktrace(symbols[i]));
  }

  free(symbols);
  free(trace);

  return bt;
#else
  (void)trace_size;
  return {};
#endif
}