//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_CONFIG_TYPES_H_
#define PUBLIC_VDA5050_2B_2B_CONFIG_TYPES_H_

#include <typeinfo>

#include "vda5050++/exception.h"

namespace vda5050pp {

class Config;

namespace config {

///\brief The parent class for TOML nodes. Used to not expose the TOML Backend dependency.
class ConfigNode {};

///\brief The SubConfig base class. To register a subconfig, one must inherit from this class.
class SubConfig {
public:
  friend class vda5050pp::Config;

private:
  ///\brief Create a new VDA5050PPBadCast exception
  ///\param is the expected type
  ///\param got the actual type
  ///\return the exception object
  static vda5050pp::VDA5050PPBadCast badCast(const std::type_info &is, const std::type_info &got);

protected:
  ///\brief Restore this object from a ConfigNode
  ///\param node the config node
  virtual void getFrom(const ConfigNode &node) = 0;

  ///\brief Dump this object to a ConfigNode
  ///\param node the config node
  virtual void putTo(ConfigNode &node) const = 0;

public:
  virtual ~SubConfig() = default;

  ///\brief Return a reference to *this, casted to a specialized SubConfig.
  ///\tparam Specialized the specialized SubConfig type
  ///\throws vda5050pp::VDA5050PPBadCast if the type mismatches
  ///\return Specialized& the casted reference to *this
  template <typename Specialized> inline Specialized &as() noexcept(false) {
    if (auto casted = dynamic_cast<Specialized *>(this); casted != nullptr) {
      return *casted;
    } else {
      throw SubConfig::badCast(typeid(this), typeid(Specialized));
    }
  }

  ///\brief Return a const reference to *this, casted to a specialized SubConfig.
  ///\tparam Specialized the specialized SubConfig type
  ///\throws vda5050pp::VDA5050PPBadCast if the type mismatches
  ///\return const Specialized& the casted reference to *this
  template <typename Specialized> inline const Specialized &as() const noexcept(false) {
    if (auto casted = dynamic_cast<Specialized *>(this); casted != nullptr) {
      return *casted;
    } else {
      throw SubConfig::badCast(typeid(this), typeid(Specialized));
    }
  }

  ///\brief Cast a SubConfig pointer to a specialized SubConfig pointer.
  ///\tparam Specialized the specialized SubConfig type
  ///\param cfg the pointer to the SubConfig
  ///\throws vda5050pp::VDA5050PPBadCast if the type mismatches
  ///\return std::shared_ptr<Specialized> a casted pointer
  template <typename Specialized>
  static inline std::shared_ptr<Specialized> ptr_as(std::shared_ptr<SubConfig> cfg) noexcept(
      false) {
    if (cfg == nullptr) {
      return nullptr;
    }
    if (auto casted = std::dynamic_pointer_cast<Specialized>(cfg); casted != nullptr) {
      return casted;
    } else {
      const auto &o = *cfg;
      throw SubConfig::badCast(typeid(o), typeid(Specialized));
    }
  }
};

}  // namespace config

}  // namespace vda5050pp

#endif  // PUBLIC_VDA5050_2B_2B_CONFIG_TYPES_H_
