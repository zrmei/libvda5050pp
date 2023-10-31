// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains the declaration of the SimpleMultiActionHandler
//
//

#ifndef PUBLIC_VDA5050_2B_2B_HANDLER_SIMPLE_MULTI_ACTION_HANDLER_H_
#define PUBLIC_VDA5050_2B_2B_HANDLER_SIMPLE_MULTI_ACTION_HANDLER_H_

#include "vda5050++/agv_description/action_declaration.h"
#include "vda5050++/handler/base_action_handler.h"

namespace vda5050pp::handler {

///
///\brief A SimpleMultiActionHandler which will automatically take care of:
/// - Matching
/// - Validation/Parameter Parsing
/// - Factsheet description
///
/// If you intend to use this class, you must overwrite only BaseActionHandler::prepare().
/// Since there may be multiple types of actions that will be passed to prepare(),
/// you have to discriminate by action_state->getAction()->actionType yourself.
///
class SimpleMultiActionHandler : public BaseActionHandler {
private:
  std::set<vda5050pp::agv_description::ActionDeclaration> declarations_;

public:
  SimpleMultiActionHandler() = default;

  ///
  ///\brief Construct a new SimpleMultiActionHandler.
  ///
  ///\param declarations the declarations of all actions handled by this handler.
  ///
  explicit SimpleMultiActionHandler(
      std::initializer_list<vda5050pp::agv_description::ActionDeclaration> declarations);

  ///
  ///\brief Construct a new SimpleMultiActionHandler.
  ///
  ///\param declarations the declarations of all actions handled by this handler.
  ///
  explicit SimpleMultiActionHandler(
      const std::set<vda5050pp::agv_description::ActionDeclaration> &declarations);

  ///
  ///\brief Construct a new SimpleMultiActionHandler.
  ///
  ///\param declarations the declarations of all actions handled by this handler.
  ///
  explicit SimpleMultiActionHandler(
      std::set<vda5050pp::agv_description::ActionDeclaration> &&declarations);

  ///
  ///\brief Add a declaration for actions that will be handled by this handler.
  ///
  ///\param decl the action declaration
  ///
  void addActionDeclaration(const vda5050pp::agv_description::ActionDeclaration &decl);

  ///
  ///\brief This function will be called by the library, when a new action arrives. It determines
  /// if a specific action will be handled by this BaseActionHandler.
  ///
  ///\param action the action to check.
  ///\return does this handle the action.
  ///
  bool match(const vda5050::Action &action) const noexcept(true) override;

  ///
  ///\brief Validate an incoming action. If the result contains errors the action will be rejected.
  ///
  ///\param action the action to validate.
  ///\param context the context of the action (node/edge/instant).
  ///\return ValidationResult the validation result.
  ///
  ValidationResult validate(const vda5050::Action &action,
                            vda5050pp::misc::ActionContext context) noexcept(true) override;

  ///
  ///\brief Get a description of all actions handled by this handler (used for the factsheet).
  ///
  ///\return std::list<vda5050::AgvAction>
  ///
  std::list<vda5050::AgvAction> getActionDescription() const noexcept(false) override;
};

}  // namespace vda5050pp::handler

#endif  // PUBLIC_VDA5050_2B_2B_HANDLER_SIMPLE_MULTI_ACTION_HANDLER_H_
