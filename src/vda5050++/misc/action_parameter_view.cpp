//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/misc/action_parameter_view.h"

#include <fmt/format.h>

#include <algorithm>
#include <set>

#include "vda5050++/core/common/container.h"
#include "vda5050++/core/common/exception.h"

using namespace vda5050pp::misc;

inline std::optional<std::string_view> lookupActionParameter(
    const std::vector<vda5050::ActionParameter> &parameters, std::string_view key) noexcept(false) {
  auto match = [key](const vda5050::ActionParameter &parameter) { return parameter.key == key; };

  if (auto it = std::find_if(begin(parameters), end(parameters), match); it != end(parameters)) {
    return it->value;
  } else {
    return std::nullopt;
  }
}

inline std::optional<bool> parseBool(std::string_view value) {
  static const std::set<std::string_view, std::less<>> true_values{
      "true", "True", "TRUE", "1", "yes", "Yes", "YES", "on", "On", "ON"};

  static const std::set<std::string_view, std::less<>> false_values{
      "false", "False", "FALSE", "0", "no", "No", "NO", "off", "Off", "OFF"};

  if (vda5050pp::core::common::contains(true_values, value)) {
    return true;
  }

  if (vda5050pp::core::common::contains(false_values, value)) {
    return false;
  }

  return std::nullopt;
}

ActionParameterView::ActionParameterView(
    const std::vector<vda5050::ActionParameter> &parameters) noexcept(false)
    : parameters_(parameters) {}

std::string_view ActionParameterView::get(std::string_view key) const noexcept(false) {
  if (auto maybe_value = lookupActionParameter(this->parameters_, key); maybe_value.has_value()) {
    return *maybe_value;
  } else {
    throw vda5050pp::VDA5050PPInvalidActionParameterKey(
        MK_EX_CONTEXT(fmt::format("Key \"{}\" not found", key)));
  }
}
std::optional<std::string_view> ActionParameterView::tryGet(std::string_view key) const {
  return lookupActionParameter(this->parameters_, key);
}

int64_t ActionParameterView::getInt(std::string_view key) const noexcept(false) {
  auto value = this->get(key);
  return ActionParameterValueView(value).getInt();
}
std::optional<int64_t> ActionParameterView::tryGetInt(std::string_view key) const {
  if (auto maybe_value = this->tryGet(key); maybe_value.has_value()) {
    return ActionParameterValueView(*maybe_value).tryGetInt();
  } else {
    return std::nullopt;
  }
}

double ActionParameterView::getFloat(std::string_view key) const noexcept(false) {
  auto value = this->get(key);
  return ActionParameterValueView(value).getFloat();
}
std::optional<double> ActionParameterView::tryGetFloat(std::string_view key) const {
  if (auto maybe_value = this->tryGet(key); maybe_value.has_value()) {
    return ActionParameterValueView(*maybe_value).tryGetFloat();
  } else {
    return std::nullopt;
  }
}

bool ActionParameterView::getBool(std::string_view key) const noexcept(false) {
  auto value = this->get(key);
  return ActionParameterValueView(value).getBool();
}

std::optional<bool> ActionParameterView::tryGetBool(std::string_view key) const {
  if (auto maybe_value = this->tryGet(key); maybe_value.has_value()) {
    return ActionParameterValueView(*maybe_value).tryGetBool();
  } else {
    return std::nullopt;
  }
}

ActionParameterValueView::ActionParameterValueView(std::string_view value) : value_(value) {}

std::string_view ActionParameterValueView::get() const { return this->value_; }

int64_t ActionParameterValueView::getInt() const noexcept(false) {
  try {
    std::size_t pos = 0;
    if (auto parsed = std::stol(this->value_.data(), &pos); pos != this->value_.size()) {
      throw vda5050pp::VDA5050PPInvalidActionParameterType(
          MK_EX_CONTEXT(fmt::format("Could not parse \"{}\" as integer", this->value_)));
    } else {
      return parsed;
    }
  } catch (const std::invalid_argument &) {
    throw vda5050pp::VDA5050PPInvalidActionParameterType(
        MK_EX_CONTEXT(fmt::format("Could not parse \"{}\" as integer", this->value_)));
  } catch (const std::out_of_range &) {
    throw vda5050pp::VDA5050PPInvalidActionParameterType(
        MK_EX_CONTEXT(fmt::format("Value \"{}\" exceeds int64_t range", this->value_)));
  }
}

std::optional<int64_t> ActionParameterValueView::tryGetInt() const {
  try {
    std::size_t pos = 0;
    if (auto parsed = std::stol(this->value_.data(), &pos); pos != this->value_.size()) {
      return std::nullopt;
    } else {
      return parsed;
    }
  } catch (const std::invalid_argument &) {
    return std::nullopt;
  } catch (const std::out_of_range &) {
    return std::nullopt;
  }
}

double ActionParameterValueView::getFloat() const noexcept(false) {
  try {
    return std::stod(this->value_.data());
  } catch (const std::invalid_argument &) {
    throw vda5050pp::VDA5050PPInvalidActionParameterType(
        MK_EX_CONTEXT(fmt::format("Could not parse \"{}\" as double", this->value_)));
  } catch (const std::out_of_range &) {
    throw vda5050pp::VDA5050PPInvalidActionParameterType(
        MK_EX_CONTEXT(fmt::format("Value \"{}\" exceeds double range", this->value_)));
  }
}

std::optional<double> ActionParameterValueView::tryGetFloat() const {
  try {
    return std::stod(this->value_.data());
  } catch (const std::invalid_argument &) {
    return std::nullopt;
  } catch (const std::out_of_range &) {
    return std::nullopt;
  }
}

bool ActionParameterValueView::getBool() const noexcept(false) {
  if (auto maybe_bool = parseBool(this->value_); maybe_bool.has_value()) {
    return *maybe_bool;
  } else {
    throw vda5050pp::VDA5050PPInvalidActionParameterType(
        MK_EX_CONTEXT(fmt::format("Could not parse \"{}\" as bool", this->value_)));
  }
}

std::optional<bool> ActionParameterValueView::tryGetBool() const { return parseBool(this->value_); }

ActionParameterValueView::operator std::string_view() const { return this->value_; }

ActionParameterValueView::operator std::string() const { return std::string(this->value_); }

ActionParameterValueView::operator int64_t() const noexcept(false) { return this->getInt(); }

ActionParameterValueView::operator std::optional<int64_t>() const { return this->tryGetInt(); }

ActionParameterValueView::operator double() const noexcept(false) { return this->getFloat(); }

ActionParameterValueView::operator std::optional<double>() const { return this->tryGetFloat(); }

ActionParameterValueView::operator bool() const noexcept(false) { return this->getBool(); }

ActionParameterValueView::operator std::optional<bool>() const { return this->tryGetBool(); }