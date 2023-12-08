//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/exception.h"

#include <spdlog/fmt/fmt.h>

#include <sstream>

#include "vda5050++/core/common/type_traits.h"

using namespace vda5050pp;

template <typename Fn, typename A>
inline std::optional<std::result_of_t<Fn(A)>> omap(Fn fn, std::optional<A> x) {
  if (x) {
    return fn(*x);
  } else {
    return std::nullopt;
  }
}

std::string VDA5050PPError::formatDefault(std::string_view exception_name) const noexcept(true) {
  if (this->context_ == nullptr) {
    return std::string(exception_name);
  }

  auto demangle = [](std::optional<std::string_view> name) {
    return omap(static_cast<std::string (*)(const char *)>(&core::common::demangle),
                omap(std::mem_fn(&std::string_view::data), name));
  };

  auto description = this->getDescription().value_or("<no description>");
  auto class_name = demangle(this->getClassContext()).value_or("<unknown class>");

  if (this->context_->function_context.has_value()) {
    return fmt::format("{} @ {}::{}(): {}", exception_name, class_name,
                       *demangle(this->getFunctionContext()), description);
  } else {
    return fmt::format("{} @ {}: {}", exception_name, class_name, description);
  }
}

std::string VDA5050PPError::format() const noexcept(true) {
  return this->formatDefault("VDA5050PPError");
}

VDA5050PPError::VDA5050PPError(VDA5050PPErrorContext &&context) noexcept(true)
    : context_(std::make_unique<VDA5050PPErrorContext>(std::move(context))) {}

std::optional<std::string_view> VDA5050PPError::getClassContext() const noexcept(true) {
  if (this->context_ == nullptr) {
    return std::nullopt;  // should never be the case
  }

  return this->context_->class_context;
}

std::optional<std::string_view> VDA5050PPError::getFunctionContext() const noexcept(true) {
  if (this->context_ == nullptr) {
    return std::nullopt;  // should never be the case
  }

  return this->context_->function_context;
}

std::optional<std::string_view> VDA5050PPError::getDescription() const noexcept(true) {
  if (this->context_ == nullptr) {
    return std::nullopt;  // should never be the case
  }

  return this->context_->description;
}

const std::vector<VDA5050PPErrorContext::StackEntry> &VDA5050PPError::getStackTrace() const
    noexcept(true) {
  if (this->context_ == nullptr) {
    static std::vector<VDA5050PPErrorContext::StackEntry> empty;
    return empty;
  }

  return this->context_->stack_trace;
}

const std::map<std::string, std::string, std::less<>> &VDA5050PPError::getAdditionalContext() const
    noexcept(true) {
  if (this->context_ == nullptr) {
    // This will never be the case
  }

  return this->context_->additional_context;
}

void VDA5050PPError::addAdditionalContext(std::string_view key,
                                          std::string_view value) noexcept(true) {
  // This should somehow be possible without string construction
  this->context_->additional_context.insert_or_assign(std::string(key), value);
  this->what_.clear();
}

void VDA5050PPError::addAdditionalContext(
    std::initializer_list<std::pair<std::string_view, std::string_view>> kv_pairs) noexcept(true) {
  for (auto &[key, value] : kv_pairs) {
    this->addAdditionalContext(key, value);
  }
}

const char *VDA5050PPError::what() const noexcept(true) {
  if (this->what_.empty()) {
    this->what_ = this->dump();
  }
  return this->what_.c_str();
}

std::string VDA5050PPError::dump() const noexcept(true) {
  std::stringstream ss;
  ss << this->format();

  if (this->context_ == nullptr || this->context_->additional_context.empty()) {
    ss << "\n  <no additional context>"
       << "\n";
  } else {
    for (const auto &[k, v] : this->context_->additional_context) {
      ss << "\n  k=\t" << v << "\n";
    }
  }
  if (this->context_ != nullptr && !this->context_->stack_trace.empty()) {
    ss << "StackTrace:";
    for (const auto &stack_entry : this->context_->stack_trace) {
      ss << "\n  " << stack_entry.function << "+" << stack_entry.offset;
    }
  }

  return ss.str();
}

VDA5050PPNotInitialized::VDA5050PPNotInitialized(VDA5050PPErrorContext &&context) noexcept(true)
    : VDA5050PPError(std::move(context)) {}

std::string VDA5050PPNotInitialized::format() const noexcept(true) {
  return this->VDA5050PPError::formatDefault("VDA5050PPNotInitialized");
}

VDA5050PPInvalidEventData::VDA5050PPInvalidEventData(VDA5050PPErrorContext &&context) noexcept(true)
    : VDA5050PPError(std::move(context)) {}

std::string VDA5050PPInvalidEventData::format() const noexcept(true) {
  return this->VDA5050PPError::formatDefault("VDA5050PPInvalidEventData");
}

VDA5050PPInvalidArgument::VDA5050PPInvalidArgument(VDA5050PPErrorContext &&context) noexcept(true)
    : VDA5050PPError(std::move(context)) {}

std::string VDA5050PPInvalidArgument::format() const noexcept(true) {
  return this->VDA5050PPError::formatDefault("VDA5050PPInvalidArgument");
}

VDA5050PPCallbackNotSet::VDA5050PPCallbackNotSet(VDA5050PPErrorContext &&context) noexcept(true)
    : VDA5050PPError(std::move(context)) {}

std::string VDA5050PPCallbackNotSet::format() const noexcept(true) {
  return this->VDA5050PPError::formatDefault("VDA5050PPCallbackNotSet");
}

VDA5050PPSynchronizedEventNotAcquired::VDA5050PPSynchronizedEventNotAcquired(
    VDA5050PPErrorContext &&context) noexcept(true)
    : VDA5050PPError(std::move(context)) {}

std::string VDA5050PPSynchronizedEventNotAcquired::format() const noexcept(true) {
  return this->VDA5050PPError::formatDefault("VDA5050PPSynchronizedEventNotAcquired");
}

VDA5050PPBadAnyPtrCast::VDA5050PPBadAnyPtrCast(VDA5050PPErrorContext &&context) noexcept(true)
    : VDA5050PPError(std::move(context)) {}

std::string VDA5050PPBadAnyPtrCast::format() const noexcept(true) {
  return this->VDA5050PPError::formatDefault("VDA5050PPBadAnyPtrCast");
}

VDA5050PPNullPointer::VDA5050PPNullPointer(VDA5050PPErrorContext &&context) noexcept(true)
    : VDA5050PPError(std::move(context)) {}

std::string VDA5050PPNullPointer::format() const noexcept(true) {
  return this->VDA5050PPError::formatDefault("VDA5050PPNullPointer");
}

VDA5050PPSynchronizedEventTimedOut::VDA5050PPSynchronizedEventTimedOut(
    VDA5050PPErrorContext &&context) noexcept(true)
    : VDA5050PPError(std::move(context)) {}

std::string VDA5050PPSynchronizedEventTimedOut::format() const noexcept(true) {
  return this->VDA5050PPError::formatDefault("VDA5050PPSynchronizedEventTimedOut");
}

VDA5050PPInvalidActionParameterType::VDA5050PPInvalidActionParameterType(
    VDA5050PPErrorContext &&context) noexcept(true)
    : VDA5050PPError(std::move(context)) {}

std::string VDA5050PPInvalidActionParameterType::format() const noexcept(true) {
  return this->VDA5050PPError::formatDefault("VDA5050PPInvalidActionParameterType");
}

VDA5050PPInvalidActionParameterKey::VDA5050PPInvalidActionParameterKey(
    VDA5050PPErrorContext &&context) noexcept(true)
    : VDA5050PPError(std::move(context)) {}

std::string VDA5050PPInvalidActionParameterKey::format() const noexcept(true) {
  return this->VDA5050PPError::formatDefault("VDA5050PPInvalidActionParameterKey");
}

VDA5050PPInvalidConfiguration::VDA5050PPInvalidConfiguration(
    VDA5050PPErrorContext &&context) noexcept(true)
    : VDA5050PPError(std::move(context)) {}

std::string VDA5050PPInvalidConfiguration::format() const noexcept(true) {
  return this->VDA5050PPError::formatDefault("VDA5050PPInvalidConfiguration");
}

VDA5050PPMqttError::VDA5050PPMqttError(VDA5050PPErrorContext &&context) noexcept(true)
    : VDA5050PPError(std::move(context)) {}

std::string VDA5050PPMqttError::format() const noexcept(true) {
  return this->VDA5050PPError::formatDefault("VDA5050PPMqttError");
}

VDA5050PPNotImplementedError::VDA5050PPNotImplementedError(
    VDA5050PPErrorContext &&context) noexcept(true)
    : VDA5050PPError(std::move(context)) {}

std::string VDA5050PPNotImplementedError::format() const noexcept(true) {
  return this->VDA5050PPError::formatDefault("VDA5050PPNotImplementedError");
}

VDA5050PPInvalidState::VDA5050PPInvalidState(VDA5050PPErrorContext &&context) noexcept(true)
    : VDA5050PPError(std::move(context)) {}

std::string VDA5050PPInvalidState::format() const noexcept(true) {
  return this->VDA5050PPError::formatDefault("VDA5050PPInvalidState");
}

VDA5050PPTOMLError::VDA5050PPTOMLError(VDA5050PPErrorContext &&context) noexcept(true)
    : VDA5050PPError(std::move(context)) {}

std::string VDA5050PPTOMLError::format() const noexcept(true) {
  return this->VDA5050PPError::formatDefault("VDA5050PPTOMLError");
}
VDA5050PPBadCast::VDA5050PPBadCast(VDA5050PPErrorContext &&context) noexcept(true)
    : VDA5050PPError(std::move(context)) {}

std::string VDA5050PPBadCast::format() const noexcept(true) {
  return this->VDA5050PPError::formatDefault("VDA5050PPBadCast");
}