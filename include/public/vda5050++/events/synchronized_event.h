//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_EVENTS_SYNCHRONIZED_EVENT_H_
#define PUBLIC_VDA5050_2B_2B_EVENTS_SYNCHRONIZED_EVENT_H_

#include <future>
#include <memory>
#include <mutex>

#include "vda5050++/exception.h"

namespace vda5050pp::events {

// Forward declaration of SynchronizedEvent.
template <typename ResultT> class SynchronizedEvent;

///
///\brief The result state shared amongst all event tokens.
///
///\tparam ResultT the result type.
///
template <typename ResultT> struct _SharedResultState {
  ///
  ///\brief The access mutex
  ///
  std::mutex mutex;

  ///
  ///\brief The promise, where the result will be written into.
  ///
  std::promise<ResultT> promise;

  ///
  ///\brief Is this result state still live, i.e. is the promise still unused?
  ///
  bool live = true;
};

///
///\brief This class contains common static member functions, that are inherited by
/// SynchronizedEventToken.
///
class _SynchronizedEventTokenStatic {
protected:
  ///
  ///\brief Get an instance of VDA5050PPSynchronizedEventNotAcquired
  ///
  ///\return vda5050pp::VDA5050PPSynchronizedEventNotAcquired the exception instance.
  ///
  static vda5050pp::VDA5050PPSynchronizedEventNotAcquired notAcquiredException();
};

///
///\brief This token will be retrieved from a SynchronizedEvent. Every owner of such a token
/// can try to acquire it in order to set a result.
///
///\tparam ResultT the result type.
///
template <typename ResultT> class SynchronizedEventToken : private _SynchronizedEventTokenStatic {
public:
  using result_type = ResultT;

private:
  friend class SynchronizedEvent<ResultT>;
  std::unique_lock<std::mutex> lock_;
  std::shared_ptr<_SharedResultState<ResultT>> acquired_shared_state_;

  ///
  ///\brief Construct a new (acquired) SynchronizedEventToken.
  ///
  ///\param lock a lock for the mutex of the _SharedResultState.
  ///\param acquired_shared_state a pointer to the acquired _SharedResultState.
  ///
  SynchronizedEventToken(
      std::unique_lock<std::mutex> &&lock,
      std::shared_ptr<_SharedResultState<ResultT>> acquired_shared_state) noexcept(true)
      : lock_(std::move(lock)), acquired_shared_state_(acquired_shared_state) {}

  ///
  ///\brief Construct a new (not acquired) SynchronizedEventToken.
  ///
  SynchronizedEventToken() = default;

public:
  ///
  ///\brief Is this Token acquired, i.e. can the result be set?
  ///
  ///\return is acquired?
  ///
  bool isAcquired() const noexcept(true) { return this->acquired_shared_state_ != nullptr; }

  ///
  ///\brief Set the result value. This token must be acquired.
  ///
  ///\tparam T the value type.
  ///\param r the value to set.
  ///\throws VDA5050PPSynchronizedEventNotAcquired when the token is not acquired.
  ///
  template <typename T> void setValue(T &&r) const noexcept(false) {
    static_assert(std::is_convertible_v<T, ResultT>,
                  "setValue(T &&r) can only accept T as convertible to ResultT");
    if (this->acquired_shared_state_ == nullptr || !this->acquired_shared_state_->live) {
      throw notAcquiredException();
    }
    this->acquired_shared_state_->promise.set_value(std::forward<T>(r));
    this->acquired_shared_state_->live = false;
  }

  ///
  ///\brief Set an exception, in place of a value. The token must be acquired.
  ///
  ///\param e the exception pointer.
  ///\throws VDA5050PPSynchronizedEventNotAcquired when the token is not acquired.
  ///
  void setException(std::exception_ptr e) const noexcept(false) {
    if (this->acquired_shared_state_ == nullptr || !this->acquired_shared_state_->live) {
      throw notAcquiredException();
    }
    this->acquired_shared_state_->promise.set_exception(e);
    this->acquired_shared_state_->live = false;
  }

  ///
  ///\brief Release an acquired token. This is only useful, if the token was acquired.
  ///
  ///\throws VDA5050PPSynchronizedEventNotAcquired when the token is not acquired.
  ///
  void release() noexcept(false) {
    if (this->acquired_shared_state_ == nullptr || !this->acquired_shared_state_->live) {
      throw notAcquiredException();
    }
    this->lock_.unlock();
    this->acquired_shared_state_ = nullptr;
  }

  ///
  ///\brief Returns this->isAcquired()
  ///
  ///\return is this token acquired?
  ///
  operator bool() const noexcept(true) { return this->isAcquired(); }
};

///
///\brief SynchronizedEventToken specialization for void value types.
///
template <> class SynchronizedEventToken<void> : private _SynchronizedEventTokenStatic {
public:
  using result_type = void;

private:
  friend class SynchronizedEvent<void>;
  std::unique_lock<std::mutex> lock_;
  std::shared_ptr<_SharedResultState<void>> acquired_shared_state_;

  ///
  ///\brief Construct a new (acquired) SynchronizedEventToken.
  ///
  ///\param lock a lock for the mutex of the _SharedResultState.
  ///\param acquired_shared_state a pointer to the acquired _SharedResultState.
  ///
  SynchronizedEventToken(
      std::unique_lock<std::mutex> &&lock,
      std::shared_ptr<_SharedResultState<void>> acquired_shared_state) noexcept(true)
      : lock_(std::move(lock)), acquired_shared_state_(acquired_shared_state) {}

  ///
  ///\brief Construct a new (not acquired) SynchronizedEventToken.
  ///
  SynchronizedEventToken() = default;

public:
  ///
  ///\brief Is this Token acquired, i.e. can the result be set?
  ///
  ///\return is acquired?
  ///
  bool isAcquired() const noexcept(true) { return this->acquired_shared_state_ != nullptr; }

  ///
  ///\brief Set the result value. This token must be acquired.
  ///
  ///\throws VDA5050PPSynchronizedEventNotAcquired when the token is not acquired.
  ///
  void setValue() const noexcept(false) {
    if (this->acquired_shared_state_ == nullptr || !this->acquired_shared_state_->live) {
      throw notAcquiredException();
    }
    this->acquired_shared_state_->promise.set_value();
    this->acquired_shared_state_->live = false;
  }

  ///
  ///\brief Set an exception, in place of a value. The token must be acquired.
  ///
  ///\param e the exception pointer.
  ///\throws VDA5050PPSynchronizedEventNotAcquired when the token is not acquired.
  ///
  void setException(std::exception_ptr e) const noexcept(false) {
    if (this->acquired_shared_state_ == nullptr || !this->acquired_shared_state_->live) {
      throw notAcquiredException();
    }
    this->acquired_shared_state_->promise.set_exception(e);
    this->acquired_shared_state_->live = false;
  }

  ///
  ///\brief Release an acquired token. This is only useful, if the token was acquired.
  ///
  ///\throws VDA5050PPSynchronizedEventNotAcquired when the token is not acquired.
  ///
  void release() noexcept(false) {
    if (this->acquired_shared_state_ == nullptr || !this->acquired_shared_state_->live) {
      throw notAcquiredException();
    }
    this->lock_.unlock();
    this->acquired_shared_state_ = nullptr;
  }

  ///
  ///\brief Returns this->isAcquired()
  ///
  ///\return is this token acquired?
  ///
  operator bool() const noexcept(true) { return this->isAcquired(); }
};

///
///\brief A base type for SynchronizedEvents. It only provides the result members, not the actual
/// event members, so a base event type must be inherited, too.
///
/// A SynchronizedEvent is meant to make the sender get a result from the recipient.
/// The sender can wait for the SynchronizedEvent (getFuture()), while the recipient can
/// set a result via acquireResultToken().
///
///\tparam ResultT the result of the synchronized event.
///
template <typename ResultT> class SynchronizedEvent {
public:
  using result_type = ResultT;

private:
  std::shared_ptr<_SharedResultState<ResultT>> shared_state_ =
      std::make_shared<_SharedResultState<ResultT>>();

public:
  ///
  ///\brief Get a future object of the result. (Called by sender)
  ///
  ///\return std::future<ResultT>
  ///
  std::future<ResultT> getFuture() noexcept(true) {
    return this->shared_state_->promise.get_future();
  }

  ///
  ///\brief Try to acquire a SynchronizedEventToken. (Called by recipient)
  ///
  ///\return SynchronizedEventToken<ResultT>
  ///
  SynchronizedEventToken<ResultT> acquireResultToken() noexcept(true) {
    std::unique_lock lock(this->shared_state_->mutex, std::defer_lock);
    if (lock.try_lock() && this->shared_state_->live) {
      return SynchronizedEventToken(std::move(lock), this->shared_state_);
    } else {
      return {};
    }
  }
};

}  // namespace vda5050pp::events

#endif  // PUBLIC_VDA5050_2B_2B_EVENTS_SYNCHRONIZED_EVENT_H_
