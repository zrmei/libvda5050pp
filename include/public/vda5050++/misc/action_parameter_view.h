//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_MISC_ACTION_PARAMETER_VIEW_H_
#define PUBLIC_VDA5050_2B_2B_MISC_ACTION_PARAMETER_VIEW_H_

#include <optional>
#include <string_view>
#include <vector>

#include "vda5050/ActionParameter.h"

namespace vda5050pp::misc {

///
///\brief A convenience view for vda5050::ActionParameter vectors.
/// It can be used if you want to parse parameters by yourself.
///
class ActionParameterView {
private:
  const std::vector<vda5050::ActionParameter> &parameters_;

public:
  ///
  ///\brief Create a new view from a vector of ActionParameters.
  ///
  ///\param parameters the action parameters to view.
  ///
  explicit ActionParameterView(const std::vector<vda5050::ActionParameter> &parameters) noexcept(
      false);

  ///
  ///\brief Get the key's value as string
  ///
  ///\param key the parameter key
  ///\return std::string_view the key's value
  ///\throws VDA5050PPInvalidActionParameterKey if the key is unknown.
  ///
  std::string_view get(std::string_view key) const noexcept(false);

  ///
  ///\brief Try to get the key's value as string
  ///
  ///\param key the parameter key
  ///\return std::optional<std::string_view> the key's value (if present)
  ///
  std::optional<std::string_view> tryGet(std::string_view key) const;

  ///
  ///\brief Get the key's value as integer.
  ///
  ///\param key the parameter key
  ///\return int64_t the key's value
  ///\throws VDA5050PPInvalidActionParameterKey if the key is unknown.
  ///\throws VDA5050PPInvalidActionParameterType if the key cannot be parsed as int.
  ///
  int64_t getInt(std::string_view key) const noexcept(false);

  ///
  ///\brief Try to get the key's value as integer.
  ///
  ///\param key the parameter key
  ///\return std::optional<int64_t> the key's value (if present and parsable)
  ///
  std::optional<int64_t> tryGetInt(std::string_view key) const;

  ///
  ///\brief Get the key's value as double.
  ///
  ///\param key the parameter key
  ///\return double the key's value
  ///\throws VDA5050PPInvalidActionParameterKey if the key is unknown.
  ///\throws VDA5050PPInvalidActionParameterType if the key cannot be parsed as double.
  ///
  double getFloat(std::string_view key) const noexcept(false);

  ///
  ///\brief Try to get the key's value as double.
  ///
  ///\param key the parameter key
  ///\return std::optional<double> the key's value (if present and parsable)
  ///
  std::optional<double> tryGetFloat(std::string_view key) const;

  ///
  ///\brief Get the key's value as bool.
  ///
  /// True values:   "true", "True", "TRUE", "1", "yes", "Yes", "YES", "on", "On", "ON"
  /// False values: "false", "False", "FALSE", "0", "no", "No", "NO", "off", "Off", "OFF"
  ///
  ///\param key the parameter key
  ///\return bool the key's value
  ///\throws VDA5050PPInvalidActionParameterKey if the key is unknown.
  ///\throws VDA5050PPInvalidActionParameterType if the key cannot be parsed as bool.
  ///
  bool getBool(std::string_view key) const noexcept(false);

  ///
  ///\brief Try to get the key's value as bool.
  ///
  /// True values:   "true", "True", "TRUE", "1", "yes", "Yes", "YES", "on", "On", "ON"
  /// False values: "false", "False", "FALSE", "0", "no", "No", "NO", "off", "Off", "OFF"
  ///
  ///\param key the parameter key
  ///\return std::optional<bool> the key's value (if present and parsable)
  ///
  std::optional<bool> tryGetBool(std::string_view key) const;
};

///
///\brief A view for a specific parameter value.
///
///
class ActionParameterValueView {
private:
  std::string_view value_;

public:
  ///
  ///\brief Construct new ActionParameterValueView for value.
  ///
  ///\param value the value to view.
  ///
  explicit ActionParameterValueView(std::string_view value);

  ///
  ///\brief Get the value as string.
  ///
  ///\return std::string_view
  ///
  std::string_view get() const;

  ///
  ///\brief Get the value as integer.
  ///
  ///\return int64_t the value
  ///\throws VDA5050PPInvalidActionParameterType if the key cannot be parsed as int.
  ///
  int64_t getInt() const noexcept(false);

  ///
  ///\brief Try to get the value as integer.
  ///
  ///\return std::optional<int64_t> the value (if parsable)
  ///
  std::optional<int64_t> tryGetInt() const;

  ///
  ///\brief Get the value as double.
  ///
  ///\return double the value
  ///\throws VDA5050PPInvalidActionParameterType if the key cannot be parsed as double.
  ///
  double getFloat() const noexcept(false);

  ///
  ///\brief Try to get the value as double.
  ///
  ///\return std::optional<double> the value (if parsable)
  ///
  std::optional<double> tryGetFloat() const;

  ///
  ///\brief Get the value as bool.
  ///
  /// True values:   "true", "True", "TRUE", "1", "yes", "Yes", "YES", "on", "On", "ON"
  /// False values: "false", "False", "FALSE", "0", "no", "No", "NO", "off", "Off", "OFF"
  ///
  ///\return bool the value
  ///\throws VDA5050PPInvalidActionParameterType if the key cannot be parsed as bool.
  ///
  bool getBool() const noexcept(false);

  ///
  ///\brief Try to get the value as bool.
  ///
  /// True values:   "true", "True", "TRUE", "1", "yes", "Yes", "YES", "on", "On", "ON"
  /// False values: "false", "False", "FALSE", "0", "no", "No", "NO", "off", "Off", "OFF"
  ///
  ///\return std::optional<bool> the value (if parsable)
  ///
  std::optional<bool> tryGetBool() const;

  explicit operator std::string_view() const;
  explicit operator std::string() const;
  explicit operator int64_t() const noexcept(false);
  explicit operator std::optional<int64_t>() const;
  explicit operator double() const noexcept(false);
  explicit operator std::optional<double>() const;
  explicit operator bool() const noexcept(false);
  explicit operator std::optional<bool>() const;
};

}  // namespace vda5050pp::misc

#endif  // PUBLIC_VDA5050_2B_2B_MISC_ACTION_PARAMETER_VIEW_H_
