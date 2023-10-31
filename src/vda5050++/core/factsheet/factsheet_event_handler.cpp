// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains the FactsheetEventHandler implementation.
//
//

#include "vda5050++/core/factsheet/factsheet_event_handler.h"

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/config.h"
#include "vda5050++/core/factsheet/gather.h"
#include "vda5050++/core/instance.h"

using namespace vda5050pp::core::factsheet;

inline std::shared_ptr<spdlog::logger> getFactsheetLogger() {
  return vda5050pp::core::getRemappedLogger(
      vda5050pp::core::module_keys::k_factsheet_event_handler_key);
}

void FactsheetEventHandler::handleFactsheetGatherEvent(
    std::shared_ptr<vda5050pp::core::events::FactsheetGatherEvent> evt) const {
  if (evt == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("FactsheetGatherEvent is empty"));
  }

  auto result = evt->acquireResultToken();
  if (!result.isAcquired()) {
    throw vda5050pp::VDA5050PPSynchronizedEventNotAcquired(
        MK_EX_CONTEXT("Could not acquire FactsheetGatherEvent token"));
  }

  try {
    vda5050::AgvFactsheet factsheet;
    factsheet.agvGeometry = gatherGeometry();
    factsheet.loadSpecification = gatherLoadSpecification();
    factsheet.localizationParameters = gatherLocalizationParameters();
    factsheet.physicalParameters = gatherPhysicalParameters();
    factsheet.agvProtocolFeatures = gatherProtocolFeatures();
    factsheet.protocolLimits = gatherProtocolLimits();
    factsheet.typeSpecification = gatherTypeSpecification();

    result.setValue(std::move(factsheet));
  } catch (const vda5050pp::VDA5050PPError &e) {
    getFactsheetLogger()->error("Caught an exception during factsheet gathering:\n  {}", e);
    result.setException(std::current_exception());
  }
}

void FactsheetEventHandler::initialize(vda5050pp::core::Instance &instance) {
  this->subscriber_ = instance.getFactsheetEventManager().getScopedSubscriber();
  this->subscriber_->subscribe<vda5050pp::core::events::FactsheetGatherEvent>(
      std::bind(std::mem_fn(&FactsheetEventHandler::handleFactsheetGatherEvent), this, std::placeholders::_1));
}

void FactsheetEventHandler::deinitialize(vda5050pp::core::Instance &) { this->subscriber_.reset(); }

std::string_view FactsheetEventHandler::describe() const { return "FactsheetEventHandler"; }