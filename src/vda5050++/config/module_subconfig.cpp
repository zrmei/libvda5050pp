//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/config/module_subconfig.h"

using namespace vda5050pp::config;

void ModuleSubConfig::getFrom(const ConstConfigNode &node) {
  this->LoggingSubConfig::getFrom(node);
}

void ModuleSubConfig::putTo(ConfigNode &node) const { this->LoggingSubConfig::putTo(node); }