//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_HANDLER_INIT_POSITION_HANDLER_H_
#define PUBLIC_VDA5050_2B_2B_HANDLER_INIT_POSITION_HANDLER_H_

#include "vda5050++/handler/init_position_handler.h"
#include "vda5050++/handler/simple_action_handler.h"

namespace vda5050pp::handler {

///
///\brief A pre implemented BaseActionHandler for the initPosition instant action.
///
/// As a user you only have to overwrite the handleInitPosition() function.
///
class InitPositionHandler : public vda5050pp::handler::SimpleActionHandler {
private:
  ///
  ///\brief This function is called to prepare a valid action.
  /// This will be mainly used to organize possible things in the user-code and set callbacks
  /// for the concerning action.
  ///
  ///\param action_state the ActionState (may not be used here)
  ///\param parameters the parsed parameters of the validate() call
  ///\return ActionCallbacks the callbacks used for the Action associated with action_state.
  ///
  vda5050pp::handler::ActionCallbacks prepare(
      std::shared_ptr<vda5050pp::handler::ActionState> action_state,
      std::shared_ptr<ParametersMap> parameters) noexcept(false) override;

public:
  InitPositionHandler();
  ~InitPositionHandler() override = default;

  ///
  ///\brief A convenience function to set the state.lastNodeId
  ///
  ///\param last_node_id the new last node id.
  ///
  void setLastNodeId(std::string_view last_node_id) const;

  ///
  ///\brief This function is called when the initPosition instant action is executed.
  /// The success of this instant action must be set via the action_state.
  ///
  ///\param x new position.x
  ///\param y new position.y
  ///\param theta new position.theta
  ///\param map_id new mapId
  ///\param last_node_id new last_node_id
  ///\param action_state the action_state for finished/failed
  ///
  virtual void handleInitPosition(double x, double y, double theta, std::string_view map_id,
                                  std::string_view last_node_id,
                                  vda5050pp::handler::ActionState &action_state) = 0;
};

}  // namespace vda5050pp::handler

#endif  // PUBLIC_VDA5050_2B_2B_HANDLER_INIT_POSITION_HANDLER_H_
