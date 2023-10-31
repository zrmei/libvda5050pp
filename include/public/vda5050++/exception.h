//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef INCLUDE_PUBLIC_VDA5050_2B_2B_EXCEPTION_H_
#define INCLUDE_PUBLIC_VDA5050_2B_2B_EXCEPTION_H_

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace vda5050pp {

///\brief This struct contains the context of a vda5050pp::Exception
struct VDA5050PPErrorContext {
  ///\brief  This struct contains the field returned by a call stack line
  struct StackEntry {
    std::string file;
    std::string function;
    std::string offset;
  };

  ///\brief The class, in which the Exception was thrown
  std::optional<std::string> class_context;
  ///\brief The function, in which the Exception was thrown
  std::optional<std::string> function_context;
  ///\brief An additional description of the Exception
  std::optional<std::string> description;
  ///\brief The current stack trace
  std::vector<StackEntry> stack_trace;
  ///\brief Some more context about the Exception (might be removed).
  std::map<std::string, std::string, std::less<>> additional_context;
};

///\brief The base class for all vda5050pp Exceptions.
class VDA5050PPError : public std::exception {
private:
  std::unique_ptr<VDA5050PPErrorContext> context_;
  mutable std::string what_;

protected:
  std::string formatDefault(std::string_view exception_name) const noexcept(true);
  virtual std::string format() const noexcept(true);

public:
  /// \brief Construct a new VDA5050PPError. This must not be called by hand. See:
  /// vda5050pp/core/common/exception
  /// \param context The error context.
  explicit VDA5050PPError(VDA5050PPErrorContext &&context) noexcept(true);

  ///\brief Get the Class Context
  ///\return std::optional<std::string_view>
  std::optional<std::string_view> getClassContext() const noexcept(true);

  ///\brief Get the Function Context
  ///\return std::optional<std::string_view>
  std::optional<std::string_view> getFunctionContext() const noexcept(true);

  ///\brief Get the Description
  ///\return std::optional<std::string_view>
  std::optional<std::string_view> getDescription() const noexcept(true);

  ///\brief Get the StackTrace of the exception
  ///\return const std::vector<VDA5050PPErrorContext::StackEntry>&
  const std::vector<VDA5050PPErrorContext::StackEntry> &getStackTrace() const noexcept(true);

  /// \brief Get the additional context
  /// \return const std::map<std::string, std::string, std::less<>> &
  const std::map<std::string, std::string, std::less<>> &getAdditionalContext() const
      noexcept(true);

  ///\brief Add additional context to the error
  ///\param key context key
  ///\param value  context value
  void addAdditionalContext(std::string_view key, std::string_view value) noexcept(true);

  ///\brief Add multiple additional context entries to the error.
  ///\param kv_pairs a list of string key value pairs.
  void addAdditionalContext(
      std::initializer_list<std::pair<std::string_view, std::string_view>> kv_pairs) noexcept(true);

  ///\brief The native what() function. Returns the contents of this->dump().
  ///\return const char*
  const char *what() const noexcept(true) override;

  ///\brief Dump the error contents.
  ///\return std::string contents
  std::string dump() const noexcept(true);
};

///\brief This exception is thrown, when the library is not yet initialized.
class VDA5050PPNotInitialized : public VDA5050PPError {
public:
  ///\brief Construct a new Error. This is not meant to be called as a user.
  /// From within the library, MK_EX_CONTEXT macro can be used.
  explicit VDA5050PPNotInitialized(VDA5050PPErrorContext &&context) noexcept(true);

  ///\brief format the exception contents.
  ///\return the formatted string
  std::string format() const noexcept(true) override;
};

///\brief This exception is thrown, an internal event contains invalid data.
class VDA5050PPInvalidEventData : public VDA5050PPError {
public:
  ///\brief format the exception contents.
  ///\return the formatted string
  explicit VDA5050PPInvalidEventData(VDA5050PPErrorContext &&context) noexcept(true);

  ///\brief format the exception contents.
  ///\return the formatted string
  std::string format() const noexcept(true) override;
};

///\brief This exception is thrown, when a function was called with invalid arguments.
class VDA5050PPInvalidArgument : public VDA5050PPError {
public:
  ///\brief format the exception contents.
  ///\return the formatted string
  explicit VDA5050PPInvalidArgument(VDA5050PPErrorContext &&context) noexcept(true);

  ///\brief format the exception contents.
  ///\return the formatted string
  std::string format() const noexcept(true) override;
};

///\brief This exception is thrown, when a necessary callback is not set.
class VDA5050PPCallbackNotSet : public VDA5050PPError {
public:
  ///\brief format the exception contents.
  ///\return the formatted string
  explicit VDA5050PPCallbackNotSet(VDA5050PPErrorContext &&context) noexcept(true);

  ///\brief format the exception contents.
  ///\return the formatted string
  std::string format() const noexcept(true) override;
};

///\brief This exception is thrown, when the library could not acquire a synchronized event.
class VDA5050PPSynchronizedEventNotAcquired : public VDA5050PPError {
public:
  ///\brief format the exception contents.
  ///\return the formatted string
  explicit VDA5050PPSynchronizedEventNotAcquired(VDA5050PPErrorContext &&context) noexcept(true);

  ///\brief format the exception contents.
  ///\return the formatted string
  std::string format() const noexcept(true) override;
};

///\brief This exception is thrown, when an AnyPtr instance was casted to the wrong type.
class VDA5050PPBadAnyPtrCast : public VDA5050PPError {
public:
  ///\brief format the exception contents.
  ///\return the formatted string
  explicit VDA5050PPBadAnyPtrCast(VDA5050PPErrorContext &&context) noexcept(true);

  ///\brief format the exception contents.
  ///\return the formatted string
  std::string format() const noexcept(true) override;
};

///\brief This exception is thrown, when the library expected a non-null pointer.
class VDA5050PPNullPointer : public VDA5050PPError {
public:
  ///\brief format the exception contents.
  ///\return the formatted string
  explicit VDA5050PPNullPointer(VDA5050PPErrorContext &&context) noexcept(true);

  ///\brief format the exception contents.
  ///\return the formatted string
  std::string format() const noexcept(true) override;
};

///\brief This exception is thrown, when the library yields a synchronized event, that is not
/// handled in time.
class VDA5050PPSynchronizedEventTimedOut : public VDA5050PPError {
public:
  ///\brief format the exception contents.
  ///\return the formatted string
  explicit VDA5050PPSynchronizedEventTimedOut(VDA5050PPErrorContext &&context) noexcept(true);

  ///\brief format the exception contents.
  ///\return the formatted string
  std::string format() const noexcept(true) override;
};

///\brief This exception is thrown, when an ActionParameterView cannot convert the underlying
/// parameter to the expected type.
class VDA5050PPInvalidActionParameterType : public VDA5050PPError {
public:
  ///\brief format the exception contents.
  ///\return the formatted string
  explicit VDA5050PPInvalidActionParameterType(VDA5050PPErrorContext &&context) noexcept(true);

  ///\brief format the exception contents.
  ///\return the formatted string
  std::string format() const noexcept(true) override;
};

///\brief This exception is thrown, when an ActionParameterView cannot find a parameter with the
/// specified key.
class VDA5050PPInvalidActionParameterKey : public VDA5050PPError {
public:
  ///\brief format the exception contents.
  ///\return the formatted string
  explicit VDA5050PPInvalidActionParameterKey(VDA5050PPErrorContext &&context) noexcept(true);

  ///\brief format the exception contents.
  ///\return the formatted string
  std::string format() const noexcept(true) override;
};

///\brief This exception is thrown, when the library was initialized with an invalid configuration
/// or is used in a way not configured. Can be thrown after initializing.
class VDA5050PPInvalidConfiguration : public VDA5050PPError {
public:
  ///\brief format the exception contents.
  ///\return the formatted string
  explicit VDA5050PPInvalidConfiguration(VDA5050PPErrorContext &&context) noexcept(true);

  ///\brief format the exception contents.
  ///\return the formatted string
  std::string format() const noexcept(true) override;
};

///\brief This exception is thrown, when the library encountered an error the the MQTT backend.
class VDA5050PPMqttError : public VDA5050PPError {
public:
  ///\brief format the exception contents.
  ///\return the formatted string
  explicit VDA5050PPMqttError(VDA5050PPErrorContext &&context) noexcept(true);

  ///\brief format the exception contents.
  ///\return the formatted string
  std::string format() const noexcept(true) override;
};

///\brief This exception is thrown, when some functionality is not implemented by the library.
class VDA5050PPNotImplementedError : public VDA5050PPError {
public:
  ///\brief format the exception contents.
  ///\return the formatted string
  explicit VDA5050PPNotImplementedError(VDA5050PPErrorContext &&context) noexcept(true);

  ///\brief format the exception contents.
  ///\return the formatted string
  std::string format() const noexcept(true) override;
};

///\brief This exception is thrown, when the called stateful function is in an invalid state.
class VDA5050PPInvalidState : public VDA5050PPError {
public:
  ///\brief format the exception contents.
  ///\return the formatted string
  explicit VDA5050PPInvalidState(VDA5050PPErrorContext &&context) noexcept(true);

  ///\brief format the exception contents.
  ///\return the formatted string
  std::string format() const noexcept(true) override;
};

///\brief This exception is thrown, when the library encountered an error with the TOML backend.
class VDA5050PPTOMLError : public VDA5050PPError {
public:
  ///\brief format the exception contents.
  ///\return the formatted string
  explicit VDA5050PPTOMLError(VDA5050PPErrorContext &&context) noexcept(true);

  ///\brief format the exception contents.
  ///\return the formatted string
  std::string format() const noexcept(true) override;
};

///\brief This exception is thrown, when the library cannot cast a given object.
class VDA5050PPBadCast : public VDA5050PPError {
public:
  ///\brief format the exception contents.
  ///\return the formatted string
  explicit VDA5050PPBadCast(VDA5050PPErrorContext &&context) noexcept(true);

  ///\brief format the exception contents.
  ///\return the formatted string
  std::string format() const noexcept(true) override;
};

}  // namespace vda5050pp

#endif  // INCLUDE_PUBLIC_VDA5050_2B_2B_EXCEPTION_H_
