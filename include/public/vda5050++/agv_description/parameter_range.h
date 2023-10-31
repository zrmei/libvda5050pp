// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
//
// This file contains the parameter range data structure
//

#ifndef PUBLIC_VDA5050_2B_2B_AGV_DESCRIPTION_PARAMETER_RANGE_H_
#define PUBLIC_VDA5050_2B_2B_AGV_DESCRIPTION_PARAMETER_RANGE_H_

#include <optional>
#include <set>
#include <string>

namespace vda5050pp::agv_description {

enum class ParameterValueType {
  k_string = 0,
  k_integer = 1,
  k_float = 2,
  k_boolean = 3,
  k_custom = 4,
};

struct ParameterRange {
  ///
  ///\brief The Parameter's Key
  ///
  /// This also identifies this ParameterRange among others
  ///
  std::string key;

  ///
  ///\brief The Parameter's Value type
  ///
  ParameterValueType type = ParameterValueType::k_string;

  ///
  ///\brief A description of this Parameter (factsheet)
  ///
  std::optional<std::string> description;

  ///
  ///\brief JSON-Serialized minimum value type.
  ///
  /// Optional for ordinal value types.
  ///
  std::optional<std::string> ordinal_min;

  ///
  ///\brief JSON-Serialized maximum value type.
  ///
  /// Optional for ordinal value types.
  ///
  std::optional<std::string> ordinal_max;

  ///
  ///\brief A list of allowed values
  ///
  /// Optional for enumerating values
  ///
  std::optional<std::set<std::string, std::less<>>> value_set;

  ///
  ///\brief comparison for set ordering
  ///
  ///\param other the ParameterRange to compare to
  ///\return this < other
  ///
  bool operator<(const ParameterRange &other) const { return this->key < other.key; }
};
}  // namespace vda5050pp::agv_description

#endif  // PUBLIC_VDA5050_2B_2B_AGV_DESCRIPTION_PARAMETER_RANGE_H_
