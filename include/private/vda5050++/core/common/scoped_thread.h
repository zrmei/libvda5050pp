//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_COMMON_SCOPED_THREAD_H_
#define VDA5050_2B_2B_CORE_COMMON_SCOPED_THREAD_H_

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <type_traits>

namespace vda5050pp::core::common {

///
///\brief ScopedThread exception base class
///
class ScopedThreadException : public std::logic_error {
public:
  explicit ScopedThreadException(const char *what = "Generic ScopedThread exception")
      : std::logic_error(what) {}
};

///
///\brief ScopedThreadAlreadyStarted exception will be thrown on double start() call
///
class ScopedThreadAlreadyStarted : public ScopedThreadException {
public:
  ScopedThreadAlreadyStarted() : ScopedThreadException("ScopedThread already started") {}
};

///
///\brief Copyable StopToken used to communicate a stop request to the thread task
///
class StopToken {
private:
  template <typename T> friend class ScopedThread;

  std::shared_ptr<std::atomic_bool> stop_request_ptr_;
  std::shared_ptr<std::function<void()>> on_stop_requested_;

  StopToken() noexcept(true) {
    this->stop_request_ptr_ = std::make_shared<std::atomic_bool>(false);
    this->on_stop_requested_ = std::make_shared<std::function<void()>>();
  }

public:
  ///
  ///\brief Does the thread have to stop it's execution?
  ///
  ///\return stop?
  ///
  bool stopRequested() const noexcept(true) { return *this->stop_request_ptr_; }

  ///\brief Set a function to call when a stop is requested
  ///\tparam CallableT the function type (void())
  ///\param void_fn the function object
  template <typename CallableT> void onStopRequested(CallableT &&void_fn) const {
    *this->on_stop_requested_ = std::forward<CallableT>(void_fn);
  }
};

template <typename T> class ScopedThread {};

///
///\brief A move-only RAII based std::thread wrapper
///
/// This is basically a remix of c++20 std::jthread using a std::packaged_task
///
///\tparam FnT The function type to run
///\tparam ArgsT the arguments of the function (without the first StopToken argument)
///
template <typename FnT, typename... ArgsT> class ScopedThread<FnT(ArgsT...)> {
private:
  /// Type of the fully bound function (with StopToken as first argument)
  using BoundFnT = FnT(StopToken, ArgsT...);
  /// Return type of the function (the value contained inside of the future)
  using ResultT = typename std::invoke_result_t<BoundFnT, StopToken, ArgsT...>;

  std::unique_ptr<std::thread> thread_ptr_;
  std::unique_ptr<std::packaged_task<ResultT()>> packaged_task_ptr_;
  StopToken stop_token_;

public:
  ///
  ///\brief Construct a new ScopedThread object
  ///
  ///\param fn the function of type ReturnT(StopToken)
  ///\param autostart automatically call start() on construction
  ///
  explicit ScopedThread(std::function<BoundFnT> fn, ArgsT... args, bool autostart = true) {
    auto bound_fn = std::bind(fn, StopToken(this->stop_token_), args...);
    packaged_task_ptr_ = std::make_unique<std::packaged_task<ResultT()>>(bound_fn);

    if (autostart) {
      this->start();
    }
  }

  void reset(std::function<BoundFnT> fn, ArgsT... args) {
    this->thread_ptr_.reset();
    this->stop_token_ = StopToken();
    auto bound_fn = std::bind(fn, StopToken(this->stop_token_), args...);
    this->packaged_task_ptr_ = std::make_unique<std::packaged_task<ResultT()>>(bound_fn);
  }

  ///
  ///\brief Stop and join the underlying thread. Does request stop via StopToken
  ///
  ~ScopedThread() noexcept(true) {
    this->stop();
    if (this->joinable()) {
      this->join();
    }
  }

  ScopedThread(ScopedThread &&) noexcept(true) = default;
  ScopedThread(const ScopedThread &) noexcept(true) = delete;
  ScopedThread &operator=(ScopedThread &&) noexcept(true) = default;
  ScopedThread &operator=(const ScopedThread &) noexcept(true) = delete;

  ///
  ///\brief Start the thread. May only be called once
  ///
  ///\throw ScopedThreadAlreadyStarted when the thread was already started
  ///
  void start() noexcept(false) {
    if (this->thread_ptr_ != nullptr) {
      throw ScopedThreadAlreadyStarted();
    }
    this->thread_ptr_ =
        std::make_unique<std::thread>([this] { this->packaged_task_ptr_->operator()(); });
  }

  ///
  ///\brief Get the future associated with the function of this thread
  ///
  ///\return std::future<ResultT>
  ///
  std::future<ResultT> getFuture() noexcept(true) { return this->packaged_task_ptr_->get_future(); }

  ///
  ///\brief Set the StopToken request to true and call the stop request callback
  ///
  void stop() const noexcept(true) {
    stop_token_.stop_request_ptr_->operator=(true);
    if (stop_token_.on_stop_requested_ != nullptr && *stop_token_.on_stop_requested_ != nullptr) {
      stop_token_.on_stop_requested_->operator()();
    }
  }

  ///
  ///\brief Join the current thread
  ///
  void join() noexcept(false) {
    if (this->thread_ptr_ != nullptr) {
      this->thread_ptr_->join();
    }
  }

  ///
  ///\brief Is the current thread joinable?
  ///
  ///\return joinable
  ///
  bool joinable() noexcept(true) {
    return this->thread_ptr_ != nullptr && this->thread_ptr_->joinable();
  }
};

}  // namespace vda5050pp::core::common

#endif  // VDA5050_2B_2B_CORE_COMMON_SCOPED_THREAD_H_
