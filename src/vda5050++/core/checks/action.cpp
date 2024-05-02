//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/core/checks/action.h"

#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/ranges.h>

#include "vda5050++/core/common/container.h"
#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/common/formatters.h"
#include "vda5050++/core/instance.h"
#include "vda5050++/misc/action_parameter_view.h"

bool vda5050pp::core::checks::matchActionType(
    const vda5050pp::agv_description::ActionDeclaration &action_declaration,
    const vda5050::Action &action) noexcept(true) {
  return action.actionType == action_declaration.action_type;
}

template <typename T>
inline std::variant<T, vda5050::Error> ensureParameterConstraints(
    const vda5050pp::agv_description::ParameterRange &p, std::string_view value) {
  T t_value;
  try {
    t_value = T(vda5050pp::misc::ActionParameterValueView(value));
  } catch (const vda5050pp::VDA5050PPInvalidActionParameterType &e) {
    vda5050::Error err;
    err.errorType = "InvalidActionParameterValue";
    err.errorDescription = e.what();
    err.errorLevel = vda5050::ErrorLevel::WARNING;
    return err;
  }

  if (p.ordinal_min.has_value()) {
    T min_value(vda5050pp::misc::ActionParameterValueView(*p.ordinal_min));

    if (min_value > t_value) {
      vda5050::Error err;
      err.errorType = "InvalidActionParameterValue";
      err.errorDescription =
          fmt::format("Expected a value not smaller then {}, got {}", min_value, t_value);
      err.errorLevel = vda5050::ErrorLevel::WARNING;
      return err;
    }
  }

  if (p.ordinal_max.has_value()) {
    T max_value(vda5050pp::misc::ActionParameterValueView(*p.ordinal_max));

    if (max_value < t_value) {
      vda5050::Error err;
      err.errorType = "InvalidActionParameterValue";
      err.errorDescription =
          fmt::format("Expected a value not greater then {}, got {}", max_value, t_value);
      err.errorLevel = vda5050::ErrorLevel::WARNING;
      return err;
    }
  }

  if (p.value_set.has_value() && p.value_set->find(value) == p.value_set->end()) {
    vda5050::Error err;
    err.errorType = "InvalidActionParameterValue";
    err.errorDescription = fmt::format("Expected a value of {}, not {}", *p.value_set, t_value);
    err.errorLevel = vda5050::ErrorLevel::WARNING;
    return err;
  }

  return t_value;
}

template <>
inline std::variant<bool, vda5050::Error> ensureParameterConstraints<bool>(
    const vda5050pp::agv_description::ParameterRange &p, std::string_view value) {
  bool t_value;
  try {
    t_value = bool(vda5050pp::misc::ActionParameterValueView(value));
  } catch (const vda5050pp::VDA5050PPInvalidActionParameterType &e) {
    vda5050::Error err;
    err.errorType = "InvalidActionParameterValue";
    err.errorDescription = e.what();
    err.errorLevel = vda5050::ErrorLevel::WARNING;
    return err;
  }

  if (p.value_set.has_value() && p.value_set->find(value) == p.value_set->end()) {
    vda5050::Error err;
    err.errorType = "InvalidActionParameterValue";
    err.errorDescription = fmt::format("Expected a value of {}, not {}", *p.value_set, t_value);
    err.errorLevel = vda5050::ErrorLevel::WARNING;
    return err;
  }

  return t_value;
}

template <typename T>
std::optional<vda5050::Error> doInsert(
    const vda5050::ActionParameter &p, const vda5050pp::agv_description::ParameterRange &decl,
    std::optional<std::reference_wrapper<vda5050pp::handler::ParametersMap>> parameters) {
  if (auto v = ensureParameterConstraints<T>(decl, p.value);
      std::holds_alternative<vda5050::Error>(v)) {
    return std::get<vda5050::Error>(v);
  } else if (parameters.has_value()) {
    parameters->get().insert_or_assign(p.key, std::get<T>(v));
  }
  return std::nullopt;
}

std::list<vda5050::Error> vda5050pp::core::checks::contextCheck(
    const vda5050::Action &action, vda5050pp::misc::ActionContext context,
    const vda5050pp::agv_description::ActionDeclaration &action_declaration) {
  std::string where;

  // Guard
  switch (context) {
    case vda5050pp::misc::ActionContext::k_edge:
      if (action_declaration.edge) {
        return {};  // OK
      }
      where = "Edge";
      break;
    case vda5050pp::misc::ActionContext::k_instant:
      if (action_declaration.instant) {
        return {};  // OK
      }
      where = "Instant";
      break;
    case vda5050pp::misc::ActionContext::k_node:
      if (action_declaration.node) {
        return {};  // OK
      }
      where = "Node";
      break;
    case vda5050pp::misc::ActionContext::k_unspecified:
      [[fallthrough]];
    default:
      return {};  // OK
  }

  std::vector<std::string_view> allowed;
  if (action_declaration.instant) {
    allowed.push_back("Instant");
  }
  if (action_declaration.node) {
    allowed.push_back("Node");
  }
  if (action_declaration.edge) {
    allowed.push_back("Edge");
  }

  vda5050::Error err;
  err.errorType = "ActionContext";
  err.errorDescription = fmt::format("The action is not allowed to run at \"{}\"", where);
  err.errorReferences = std::vector<vda5050::ErrorReference>();
  err.errorReferences->push_back({"action.actionId", action.actionId});
  err.errorReferences->push_back({"action.actionType", action.actionType});
  err.errorReferences->push_back({"action.context", where});
  err.errorReferences->push_back({"action.allowedContext", fmt::format("{}", allowed)});

  return {err};
}

std::list<vda5050::Error> vda5050pp::core::checks::validateActionWithDeclaration(
    const vda5050::Action &action, vda5050pp::misc::ActionContext context,
    const vda5050pp::agv_description::ActionDeclaration &action_declaration,
    std::optional<std::reference_wrapper<vda5050pp::handler::ParametersMap>>
        parameters) noexcept(false) {
  std::list<vda5050::Error> errors;

  // Check action type
  if (action_declaration.action_type != action.actionType) {
    vda5050::Error err;
    err.errorType = "ActionTypeMismatch";
    err.errorDescription =
        fmt::format("The action type \"{}\" does not match required action type \"{}\"",
                    action.actionType, action_declaration.action_type);
    err.errorLevel = vda5050::ErrorLevel::WARNING;
    err.errorReferences = {{"action.actionType", action.actionType},
                           {"requiredActionType", action_declaration.action_type}};
    errors.push_back(err);
  }

  errors.splice(errors.end(), contextCheck(action, context, action_declaration));

  // check blocking type
  if (!vda5050pp::core::common::contains(action_declaration.blocking_types, action.blockingType)) {
    auto expected_types = fmt::format("{}", action_declaration.blocking_types);

    vda5050::Error err;
    err.errorType = "ActionBlockingTypeMismatch";
    err.errorDescription = fmt::format("BlockingType \"{}\" is not allowed, expected one of {}",
                                       action.blockingType, expected_types);
    err.errorLevel = vda5050::ErrorLevel::WARNING;
    err.errorReferences = {{"action.blockingType", fmt::format("{}", action.blockingType)},
                           {"allowedBlockingTypes", expected_types}};

    errors.emplace_back(std::move(err));
  }

  // check parameters

  auto to_be_found = action_declaration.parameter;
  auto may_be_found = action_declaration.optional_parameter;

  for (const auto &p : action.actionParameters.value_or(std::vector<vda5050::ActionParameter>())) {
    vda5050pp::agv_description::ParameterRange reference;
    reference.key = p.key;

    // Find a declaration for the parameter
    auto node = to_be_found.extract(reference);
    if (node.empty()) {
      node = may_be_found.extract(reference);
    }
    if (node.empty()) {
      vda5050::Error err;
      err.errorType = "UnknownActionParameter";
      err.errorDescription =
          fmt::format("ActionParameter \"{}\" with value \"{}\" is unknown.", p.key, p.value);
      err.errorLevel = vda5050::ErrorLevel::WARNING;
      err.errorReferences = {{"action.actionType", action.actionType},
                             {"action.actionId", action.actionId},
                             {"action.parameter.key", p.key}};
      errors.emplace_back(std::move(err));
      continue;
    }

    // Check value constraints
    const auto &decl = node.value();
    std::optional<vda5050::Error> maybe_error;
    switch (decl.type) {
      case vda5050pp::agv_description::ParameterValueType::k_integer:
        maybe_error = doInsert<int64_t>(p, decl, parameters);
        break;
      case vda5050pp::agv_description::ParameterValueType::k_float:
        maybe_error = doInsert<double>(p, decl, parameters);
        break;
      case vda5050pp::agv_description::ParameterValueType::k_boolean:
        maybe_error = doInsert<bool>(p, decl, parameters);
        break;
      case vda5050pp::agv_description::ParameterValueType::k_string:
        maybe_error = doInsert<std::string>(p, decl, parameters);
        break;
      case vda5050pp::agv_description::ParameterValueType::k_custom:
        throw vda5050pp::VDA5050PPInvalidConfiguration(
            MK_FN_EX_CONTEXT("Custom Parameter Values are not supported."));
      default:
        throw vda5050pp::VDA5050PPInvalidConfiguration(
            MK_FN_EX_CONTEXT("Unknown ParameterValueType"));
    }
    if (maybe_error.has_value()) {
      errors.emplace_back(std::move(*maybe_error));
    }
  }

  // Check if every required parameter was set
  if (!to_be_found.empty()) {
    std::vector<std::string_view> missing;
    missing.reserve(to_be_found.size());
    for (const auto &p : to_be_found) {
      missing.push_back(p.key);
    }
    vda5050::Error err;
    err.errorType = "MissingActionParameter";
    err.errorDescription = fmt::format("Missing action parameters: {}", missing);
    err.errorReferences = {{"action.actionId", action.actionId},
                           {"action.actionType", action.actionType}};
    errors.emplace_back(std::move(err));
  }

  return errors;
}

std::list<vda5050::Error> vda5050pp::core::checks::uniqueActionId(
    const vda5050::Action &action, std::set<std::string_view, std::less<>> &seen) noexcept(false) {
  if (common::contains(seen, action.actionId)) {
    vda5050::Error error;
    error.errorType = "orderError";
    error.errorDescription = "The order contains duplicate action ids.";
    error.errorReferences = {
        {"order.action.actionId", action.actionId},
        {"order.action.actionType", action.actionType},
    };
    error.errorLevel = vda5050::ErrorLevel::WARNING;

    return {error};
  }

  seen.insert(action.actionId);

  if (auto known_action =
          vda5050pp::core::Instance::ref().getOrderManager().tryGetAction(action.actionId);
      known_action != nullptr) {
    vda5050::Error error;
    error.errorType = "orderError";
    error.errorDescription = "The order contains an action with a non-unique id";
    error.errorReferences = {
        {"state.action.actionId", known_action->actionId},
        {"state.action.actionType", known_action->actionType},
        {"order.action.actionId", action.actionId},
        {"order.action.actionType", action.actionType},
    };
    error.errorLevel = vda5050::ErrorLevel::WARNING;

    return {error};
  }

  return {};
}

inline bool isPauseable(vda5050pp::misc::OrderStatus status) {
  return status == vda5050pp::misc::OrderStatus::k_order_active ||
         status == vda5050pp::misc::OrderStatus::k_order_idle;
}

inline bool isResumable(vda5050pp::misc::OrderStatus status) {
  return status == vda5050pp::misc::OrderStatus::k_order_idle_paused ||
         status == vda5050pp::misc::OrderStatus::k_order_paused;
}

inline bool isCancelable(vda5050pp::misc::OrderStatus status) {
  return status == vda5050pp::misc::OrderStatus::k_order_idle_paused ||
         status == vda5050pp::misc::OrderStatus::k_order_active ||
         status == vda5050pp::misc::OrderStatus::k_order_idle ||
         status == vda5050pp::misc::OrderStatus::k_order_paused;
}

std::list<vda5050::Error> vda5050pp::core::checks::controlActionFeasible(
    const vda5050::Action &action) noexcept(false) {
  auto order_status = Instance::ref().getOrderManager().getOrderStatus();

  if (action.actionType == "startPause" && !isPauseable(order_status)) {
    vda5050::Error error;
    error.errorType = "controlError";
    error.errorDescription = fmt::format("Cannot pause order with status \"{}\"", order_status);
    error.errorLevel = vda5050::ErrorLevel::WARNING;
    error.errorReferences = {{"action.actionId", action.actionId},
                             {"action.actionType", action.actionType},
                             {"internal.orderStatus", fmt::format("{}", order_status)}};
    return {error};
  } else if (action.actionType == "stopPause" && !isResumable(order_status)) {
    vda5050::Error error;
    error.errorType = "controlError";
    error.errorDescription = fmt::format("Cannot resume order with status \"{}\"", order_status);
    error.errorLevel = vda5050::ErrorLevel::WARNING;
    error.errorReferences = {{"action.actionId", action.actionId},
                             {"action.actionType", action.actionType},
                             {"internal.orderStatus", fmt::format("{}", order_status)}};
    return {error};
  } else if (action.actionType == "cancelOrder" && !isCancelable(order_status)) {
    vda5050::Error error;
    error.errorType = "controlError";
    error.errorDescription = fmt::format("Cannot cancel order with status \"{}\"", order_status);
    error.errorLevel = vda5050::ErrorLevel::WARNING;
    error.errorReferences = {{"action.actionId", action.actionId},
                             {"action.actionType", action.actionType},
                             {"internal.orderStatus", fmt::format("{}", order_status)}};
    return {error};
  } else {
    return {};
  }
}