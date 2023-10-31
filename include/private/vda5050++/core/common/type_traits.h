//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef VDA5050_2B_2B_CORE_COMMON_TYPE_TRAITS_H_
#define VDA5050_2B_2B_CORE_COMMON_TYPE_TRAITS_H_

#include <functional>
#include <string>
#include <type_traits>

namespace vda5050pp::core::common {

template <typename F, typename Prototype> struct is_signature;

template <typename F, typename Ret, typename... Args>
struct is_signature<F, Ret(Args...)> : std::is_same<Ret, std::result_of_t<F(Args...)>> {};

template <typename F, typename G> constexpr bool is_signature_v = is_signature<F, G>::value;

template <typename, typename = void> struct has_std_hash : std::false_type {};
template <typename T>
struct has_std_hash<T, std::void_t<decltype(std::hash<T>(std::declval<T>))>> : std::true_type {};

template <typename T> constexpr bool has_std_hash_v = has_std_hash<T>::value;

std::string demangle(const char *mangled_cxx_name);
std::string demangle(const std::string &mangled_cxx_name);

}  // namespace vda5050pp::core::common

#endif  // VDA5050_2B_2B_CORE_COMMON_TYPE_TRAITS_H_
