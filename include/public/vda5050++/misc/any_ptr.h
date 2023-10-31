//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_MISC_ANY_PTR_H_
#define PUBLIC_VDA5050_2B_2B_MISC_ANY_PTR_H_

#include <exception>
#include <memory>
#include <typeinfo>

namespace vda5050pp::misc {

///
///\brief Invalid AnyPtr cast
///
class BadAnyPtrCast : public std::bad_cast {
public:
  ///
  ///\brief Return the exception description
  ///
  ///\return const char*
  ///
  const char *what() const noexcept override { return "BadAnyPtrCast"; }
};

///
///\brief A type safe void ptr.
///
/// This class is essentially a std::shared_ptr<void> paired with a std::type_info object.
/// Used to safely vanish and restore types.
///
class AnyPtr {
private:
  std::shared_ptr<void> contained_;
  std::reference_wrapper<const std::type_info> info_;

public:
  ///
  ///\brief Create an empty AnyPtr (intentionally implicit)
  ///
  AnyPtr(std::nullptr_t = nullptr) : info_(typeid(std::nullptr_t)) {}

  ///
  ///\brief Create a new AnyPtr owning a shared_ptr to data
  ///
  /// The constructed AnyPtr will point to the same memory location as data.
  ///
  ///\tparam T the type of the stored data
  ///\param data the pointer to the stored data.
  ///
  template <typename T> AnyPtr(std::shared_ptr<T> data) : contained_(data), info_(typeid(T)) {}

  ///
  ///\brief Return the number of references to the pointed data
  ///
  ///\return long reference count
  ///
  long useCount() const { return this->contained_.use_count(); }

  ///
  ///\brief Get the std::type_info object associated with the stored type
  ///
  ///\return const std::type_info&
  ///
  const std::type_info &typeInfo() const { return this->info_.get(); }

  ///
  ///\brief Get a typed std::shared_ptr from the underlying data
  ///
  /// The returned pointer will point to the same memory location as this AnyPtr.
  ///
  ///\throws BadAnyPtrCast if the stored type cannot be casted to T
  ///\tparam T the type to cast the underlying data to
  ///\return std::shared_ptr<T> the casted pointer
  ///
  template <typename T> std::shared_ptr<T> get() noexcept(false) {
    if (this->info_.get() != typeid(T)) {
      throw BadAnyPtrCast();
    }

    return std::static_pointer_cast<T>(this->contained_);
  }

  ///
  ///\brief Convert this to a typed std::shared_ptr
  ///
  /// The returned pointer will point to the same memory location as this AnyPtr.
  ///
  ///\throws BadAnyPtrCast if the stored type cannot be casted to T
  ///\tparam T the type to cast to
  ///\return std::shared_ptr<T> the type pointer
  ///
  template <typename T> operator std::shared_ptr<T>() noexcept(false) { return this->get<T>(); }
};

}  // namespace vda5050pp::misc

#endif  // PUBLIC_VDA5050_2B_2B_MISC_ANY_PTR_H_
