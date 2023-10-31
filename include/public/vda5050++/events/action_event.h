//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_EVENTS_ACTION_EVENT_H_
#define PUBLIC_VDA5050_2B_2B_EVENTS_ACTION_EVENT_H_

#include <vda5050/Action.h>
#include <vda5050/AgvAction.h>
#include <vda5050/Error.h>

#include <future>
#include <list>
#include <string>

#include "vda5050++/events/synchronized_event.h"
#include "vda5050++/misc/action_context.h"

namespace vda5050pp::events {

///
///\brief The ID Type for ActionEvents.
///
enum class ActionEventType {
  k_action_list,
  k_action_validate,
  k_action_prepare,
  k_action_start,
  k_action_pause,
  k_action_resume,
  k_action_cancel,
  k_action_forget,
};

///
///\brief The common action event base type.
///
struct ActionEvent {};

///
///\brief This event is sent by the library to enquire all currently supported actions. (Factsheet)
///
struct ActionList : public ActionEvent,
                    SynchronizedEvent<std::shared_ptr<std::list<vda5050::AgvAction>>> {};

///
///\brief ActionValidate. The library sends this for each received action, such that the user
/// can acquire the SynchronizedEventToken and validate the action.
///
struct ActionValidate : public ActionEvent, SynchronizedEvent<std::list<vda5050::Error>> {
  ///
  ///\brief The action to validate.
  ///
  std::shared_ptr<const vda5050::Action> action;
  ///
  ///\brief The context of the Action.
  ///
  vda5050pp::misc::ActionContext context = vda5050pp::misc::ActionContext::k_unspecified;

  ///
  ///\brief Keep track of this action. This will be true, when it was received in an Order's Base or
  /// in as an instant action.
  ///
  bool keep = true;
};

///
///\brief ActionPrepare. The library send this for each action, that will be executed eventually.
/// This means it was validated before.
///
struct ActionPrepare : public ActionEvent {
  ///
  ///\brief The action to prepare.
  ///
  std::shared_ptr<const vda5050::Action> action;
};

///
///\brief ActionStart. The library sends this exactly when an action is meant to start.
/// If you handle Actions directly via Events, make sure to keep track of id<->action_ptr mappings.
///
struct ActionStart : public ActionEvent {
  ///
  ///\brief The ID of the action, which has to start.
  ///
  std::string action_id;
};

///
///\brief ActionPause. The library sends this exactly when an action is meant to pause.
/// If you handle Actions directly via Events, make sure to keep track of id<->action_ptr mappings.
///
struct ActionPause : public ActionEvent {
  ///
  ///\brief The ID of the action, which has to be paused.
  ///
  std::string action_id;
};

///
///\brief ActionResume. The library sends this exactly when an action is meant to resume.
/// If you handle Actions directly via Events, make sure to keep track of id<->action_ptr mappings.
///
struct ActionResume : public ActionEvent {
  ///
  ///\brief The ID of the action, which has to be resumed.
  ///
  std::string action_id;
};

///
///\brief ActionCancel. The library sends this exactly when an action is meant to cancel.
/// If you handle Actions directly via Events, make sure to keep track of id<->action_ptr mappings.
///
struct ActionCancel : public ActionEvent {
  ///
  ///\brief The ID of the action, which has to be canceled.
  ///
  std::string action_id;
};

///
///\brief ActionForget. The library sends this exactly when an action is not handled anymore.
/// If you handle Actions directly via Events, make sure to keep track of id<->action_ptr mappings.
///
struct ActionForget : public ActionEvent {
  ///
  ///\brief The ID of the action, which can be forget.
  ///
  std::string action_id;
};

///
///\brief The ID Type for ActionStatus Events.
///
enum class ActionStatusType {
  k_action_status_waiting,
  k_action_status_initializing,
  k_action_status_running,
  k_action_status_paused,
  k_action_status_finished,
  k_action_status_failed,
};

///
///\brief The ActionStatus base event type.
///
struct ActionStatus {
  ///
  ///\brief The action id of the concerning action.
  ///
  std::string action_id;
};

///
///\brief If the user chooses to handle events directly, this Event is sent,
/// once the action is waiting. This might be removed, since the library already knows this.
///
struct ActionStatusWaiting : public ActionStatus {};

///
///\brief If the user chooses to handle events directly, this Event must be sent,
/// once the action is initializing, i.e. after ActionStart was called, but before it is actually
/// running.
///
struct ActionStatusInitializing : public ActionStatus {};

///
///\brief If the user chooses to handle events directly, this Event must be sent,
/// once the action is running, i.e. after ActionStart was called.
///
struct ActionStatusRunning : public ActionStatus {};

///
///\brief If the user chooses to handle events directly, this Event must be sent,
/// once the action is paused.
///
struct ActionStatusPaused : public ActionStatus {};

///
///\brief If the user chooses to handle events directly, this Event must be sent,
/// once the action is finished.
///
struct ActionStatusFinished : public ActionStatus {
  ///
  ///\brief Set an optional result string for the action.
  ///
  std::optional<std::string> action_result;
};

///
///\brief If the user chooses to handle events directly, this Event must be sent,
/// once the action is failed.
///
struct ActionStatusFailed : public ActionStatus {
  ///
  ///\brief Set an list of errors associated with this action.
  ///
  std::list<vda5050::Error> action_errors;
};

}  // namespace vda5050pp::events

#endif  // PUBLIC_VDA5050_2B_2B_EVENTS_ACTION_EVENT_H_
