//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_HANDLER_BASE_ACTION_HANDLER_H_
#define PUBLIC_VDA5050_2B_2B_HANDLER_BASE_ACTION_HANDLER_H_

#include <vda5050/Action.h>
#include <vda5050/AgvAction.h>
#include <vda5050/Error.h>

#include <any>
#include <list>
#include <memory>
#include <string_view>
#include <variant>

#include "vda5050++/handler/action_state.h"
#include "vda5050++/misc/action_context.h"
#include "vda5050++/misc/any_ptr.h"
namespace vda5050pp::handler {

///
///\brief A variant of all possible parameter value types.
/// Can be std::string, int64_t, double, bool or AnyPtr for custom datatypes.
///
using ParameterValue = std::variant<std::string, int64_t, double, bool, vda5050pp::misc::AnyPtr>;

///
///\brief A mapping from parameter keys to parsed values.
///
using ParametersMap = std::map<std::string, ParameterValue, std::less<>>;

///
///\brief The bundled validation result. Contains errors and parsed parameters.
///
struct ValidationResult {
  ///
  ///\brief Validation errors (empty iff. valid action)
  ///
  std::list<vda5050::Error> errors;

  ///
  ///\brief All parsed parameters (will be stored for later use in order to parse the parameters
  /// only once).
  ///
  ParametersMap parameters;
};

///
///\brief The BaseActionHandler class can be overwritten to let the library automatically handle
/// ActionEvents and dispatch ActionStatusEvents.
///
class BaseActionHandler {
public:
  virtual ~BaseActionHandler() = default;

  ///
  ///\brief This function will be called by the library, when a new action arrives. It determines
  /// if a specific action will be handled by this BaseActionHandler.
  ///
  ///\param action the action to check.
  ///\return does this handle the action.
  ///
  virtual bool match(const vda5050::Action &action) const noexcept(false) = 0;

  ///
  ///\brief This function is called to prepare a valid action.
  /// This will be mainly used to organize possible things in the user-code and set callbacks
  /// for the concerning action.
  ///
  ///\param action_state the ActionState (may not be used here)
  ///\param parameters the parsed parameters of the validate() call
  ///\return ActionCallbacks the callbacks used for the Action associated with action_state.
  ///
  virtual vda5050pp::handler::ActionCallbacks prepare(
      std::shared_ptr<vda5050pp::handler::ActionState> action_state,
      std::shared_ptr<ParametersMap> parameters) noexcept(false) = 0;

  ///
  ///\brief Validate an incoming action. If the result contains errors the action will be rejected.
  ///
  ///\param action the action to validate.
  ///\param context the context of the action (node/edge/instant).
  ///\return ValidationResult the validation result.
  ///
  virtual vda5050pp::handler::ValidationResult validate(
      const vda5050::Action &action, vda5050pp::misc::ActionContext context) noexcept(false) = 0;

  ///
  ///\brief Get a description of all actions handled by this handler (used for the factsheet).
  ///
  ///\return std::list<vda5050::AgvAction>
  ///
  virtual std::list<vda5050::AgvAction> getActionDescription() const noexcept(false) = 0;
};

}  // namespace vda5050pp::handler

#endif  // PUBLIC_VDA5050_2B_2B_HANDLER_BASE_ACTION_HANDLER_H_
