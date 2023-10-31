//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_HANDLER_ACTION_STATE_H_
#define PUBLIC_VDA5050_2B_2B_HANDLER_ACTION_STATE_H_

#include <vda5050/Action.h>
#include <vda5050/Error.h>

#include <functional>
#include <future>
#include <list>

namespace vda5050pp::handler {

///
///\brief This class is used by any BaseActionHandler to keep track of individual actions.
/// It can be treated as some kind of StateMachine, which represents the state of the concerting
/// action.
///
class ActionState {
private:
  std::shared_ptr<const vda5050::Action> action_;

protected:
  ///
  ///\brief Construct a new ActionState (can only be called by the library).
  ///
  ///\param action the concerning action.
  ///
  explicit ActionState(std::shared_ptr<const vda5050::Action> action);

public:
  virtual ~ActionState() = default;

  ///
  ///\brief The user has to call this function to set the actionState to running.
  ///
  virtual void setRunning() noexcept(false) = 0;

  ///
  ///\brief The user has to call this function to set the actionState to paused.
  ///
  virtual void setPaused() noexcept(false) = 0;

  ///
  ///\brief The user has to call this function to set the actionState to finished (without result).
  ///
  virtual void setFinished() noexcept(false) = 0;

  ///
  ///\brief The user has to call this function to set the actionState to finished (with result).
  ///
  virtual void setFinished(std::string_view result_code) noexcept(false) = 0;

  ///
  ///\brief The user has to call this function to set the actionState to failed (without errors).
  ///
  virtual void setFailed() noexcept(false) = 0;

  ///
  ///\brief The user has to call this function to set the actionState to failed (with errors).
  ///
  virtual void setFailed(const std::list<vda5050::Error> &errors) noexcept(false) = 0;

  ///
  ///\brief Get a reference to the concerning action.
  ///
  ///\return const vda5050::Action&
  ///
  const vda5050::Action &getAction() const noexcept(false);
};

///
///\brief This struct bundles a set of callbacks, which will be called by the library
/// once the action shall start/pause/resume/cancel.
///
struct ActionCallbacks {
  using CallbackT = std::function<void(ActionState &)>;
  CallbackT on_start;
  CallbackT on_pause;
  CallbackT on_resume;
  CallbackT on_cancel;

  ActionCallbacks() = default;

  ///
  ///\brief Convenience constructor for ActionCallbacks.
  ///
  ///\tparam StartT the type of the Start callback
  ///\tparam PauseT the type of the Pause callback
  ///\tparam ResumeT the type of the Resume callback
  ///\tparam CancelT the type of the Cancel callback
  ///\param on_start_fn the start callback
  ///\param on_pause_fn the pause callback
  ///\param on_resume_fn the resume callback
  ///\param on_cancel_fn the cancel callback
  ///
  template <typename StartT, typename PauseT, typename ResumeT, typename CancelT>
  ActionCallbacks(StartT &&on_start_fn, PauseT &&on_pause_fn, ResumeT &&on_resume_fn,
                  CancelT &&on_cancel_fn)
      : on_start(std::forward<StartT>(on_start_fn)),
        on_pause(std::forward<PauseT>(on_pause_fn)),
        on_resume(std::forward<ResumeT>(on_resume_fn)),
        on_cancel(std::forward<CancelT>(on_cancel_fn)) {}
};

}  // namespace vda5050pp::handler

#endif  // PUBLIC_VDA5050_2B_2B_HANDLER_ACTION_STATE_H_
