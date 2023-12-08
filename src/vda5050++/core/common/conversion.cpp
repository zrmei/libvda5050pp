// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains the implementations of all common conversion functions
//
//

#include "vda5050++/core/common/conversion.h"

#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/ranges.h>

#include <algorithm>
#include <list>

#include "vda5050++/core/common/exception.h"

static vda5050::ValueDataType convertDatatype(
    vda5050pp::agv_description::ParameterValueType type) noexcept(false) {
  switch (type) {
    case vda5050pp::agv_description::ParameterValueType::k_boolean:
      return vda5050::ValueDataType::BOOL;
    case vda5050pp::agv_description::ParameterValueType::k_custom:
      return vda5050::ValueDataType::OBJECT;
    case vda5050pp::agv_description::ParameterValueType::k_float:
      return vda5050::ValueDataType::FLOAT;
    case vda5050pp::agv_description::ParameterValueType::k_integer:
      return vda5050::ValueDataType::INTEGER;
    case vda5050pp::agv_description::ParameterValueType::k_string:
      return vda5050::ValueDataType::STRING;
    default:
      throw vda5050pp::VDA5050PPInvalidArgument(
          MK_FN_EX_CONTEXT(fmt::format("Unknown ParameterValueType {}", int(type))));
  }
}

static std::string describeParameterRange(const vda5050pp::agv_description::ParameterRange &pr) {
  auto ret = fmt::format("Parameter {}:\n{}", pr.key,
                         pr.description.value_or("<no functional description>"));
  if (pr.ordinal_min && pr.ordinal_max) {
    ret = fmt::format("{}\n Must be in [{}, {}]", ret, *pr.ordinal_min, *pr.ordinal_max);
  } else if (pr.ordinal_min) {
    ret = fmt::format("{}\n Must be at least {}", ret, *pr.ordinal_min);
  } else if (pr.ordinal_max) {
    ret = fmt::format("{}\n Must be at most {}", ret, *pr.ordinal_max);
  }
  if (pr.value_set) {
    ret = fmt::format("{}\n Can only be one of {}", ret, *pr.value_set);
  }
  return ret;
}

vda5050::AgvAction vda5050pp::core::common::fromActionDeclaration(
    const vda5050pp::agv_description::ActionDeclaration &decl) noexcept(false) {
  vda5050::AgvAction agv_action;

  agv_action.actionType = decl.action_type;

  agv_action.actionDescription = decl.description;
  agv_action.resultDescription = decl.result_description;

  agv_action.actionParameters = std::vector<vda5050::ActionParameterFactsheet>();
  for (const auto &from : decl.parameter) {
    vda5050::ActionParameterFactsheet to;
    to.key = from.key;
    to.description = from.description;
    to.isOptional = false;
    to.valueDataType = convertDatatype(from.type);
    to.description = describeParameterRange(from);
    agv_action.actionParameters->push_back(to);
  }
  for (const auto &from : decl.optional_parameter) {
    vda5050::ActionParameterFactsheet to;
    to.key = from.key;
    to.description = from.description;
    to.isOptional = true;
    to.valueDataType = convertDatatype(from.type);
    to.description = describeParameterRange(from);
    agv_action.actionParameters->push_back(to);
  }

  if (decl.edge) {
    agv_action.actionScopes.push_back(vda5050::ActionScope::EDGE);
  }
  if (decl.node) {
    agv_action.actionScopes.push_back(vda5050::ActionScope::NODE);
  }
  if (decl.instant) {
    agv_action.actionScopes.push_back(vda5050::ActionScope::INSTANT);
  }

  return agv_action;
}