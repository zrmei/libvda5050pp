//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PRIVATE_VDA5050_2B_2B_CORE_MODULE_H_
#define PRIVATE_VDA5050_2B_2B_CORE_MODULE_H_

#include <string_view>

#include "vda5050++/config/types.h"
#include "vda5050++/core/instance.h"

namespace vda5050pp::core {

class Module {
public:
  virtual ~Module() = default;
  virtual void initialize(vda5050pp::core::Instance &instance) = 0;
  virtual void deinitialize(vda5050pp::core::Instance &instance) = 0;
  virtual std::string_view describe() const = 0;
  virtual std::shared_ptr<vda5050pp::config::ModuleSubConfig> generateSubConfig() const {
    return std::make_shared<vda5050pp::config::ModuleSubConfig>();
  }
};

template <typename M, auto &key> struct AutoRegisterModule {
  AutoRegisterModule() { Instance::registerModule(key, std::make_shared<M>()); }
};

}  // namespace vda5050pp::core

#endif  // PRIVATE_VDA5050_2B_2B_CORE_MODULE_H_
