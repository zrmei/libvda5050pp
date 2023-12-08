// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains the QueryEventHandler implementation.
//
//
#include "vda5050++/core/agv_handler/query_event_handler.h"

#include <spdlog/fmt/ranges.h>

#include "vda5050++/config/query_event_handler_subconfig.h"
#include "vda5050++/core/common/container.h"
#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/logger.h"

using namespace vda5050pp::core::agv_handler;

void QueryEventHandler::handleQueryPauseableEvent(
    std::shared_ptr<vda5050pp::events::QueryPauseable> data) const noexcept(false) {
  if (data == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("QueryPauseableEvent is empty."));
  }

  auto result = data->acquireResultToken();
  if (!result.isAcquired()) {
    getAGVHandlerLogger()->debug("Could not acquire QueryPauseableEvent result token");
    return;
  }

  std::optional<vda5050pp::events::QueryPauseResumeResult> result_data;

  // Try to get an answer with the handler
  if (auto ptr = Instance::ref().getQueryHandler().lock(); ptr) {
    auto ret = ptr->callQueryPauseable();
    if (ret.has_value()) {
      result_data = vda5050pp::events::QueryPauseResumeResult{ret->errors, ret->notify};
    }
  }

  // If there is no result_data, use default
  if (!result_data) {
    auto default_success = Instance::ref()
                               .getConfig()
                               .lookupModuleConfigAs<vda5050pp::config::QueryEventHandlerSubConfig>(
                                   module_keys::k_query_event_handler_key)
                               ->getDefaultPauseableSuccess();
    result_data = vda5050pp::events::QueryPauseResumeResult{};
    if (default_success.value_or(true)) {
      result_data->errors = {};
      result_data->notify = true;
    } else {
      vda5050::Error err;
      err.errorType = "controlError";
      err.errorDescription = "Cannot pause order by configured default.";
      err.errorLevel = vda5050::ErrorLevel::WARNING;
      result_data->errors = {std::move(err)};
    }
  }

  result.setValue(std::move(*result_data));
}

void QueryEventHandler::handleQueryResumableEvent(
    std::shared_ptr<vda5050pp::events::QueryResumable> data) const noexcept(false) {
  if (data == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("QueryResumableEvent is empty."));
  }

  auto result = data->acquireResultToken();
  if (!result.isAcquired()) {
    getAGVHandlerLogger()->debug("Could not acquire QueryResumableEvent result token");
    return;
  }

  std::optional<vda5050pp::events::QueryPauseResumeResult> result_data;

  // Try to get an answer with the handler
  if (auto ptr = Instance::ref().getQueryHandler().lock(); ptr) {
    auto ret = ptr->callQueryResumable();
    if (ret.has_value()) {
      result_data = vda5050pp::events::QueryPauseResumeResult{ret->errors, ret->notify};
    }
  }

  // If there is no result_data, use default
  if (!result_data) {
    auto default_success = Instance::ref()
                               .getConfig()
                               .lookupModuleConfigAs<vda5050pp::config::QueryEventHandlerSubConfig>(
                                   module_keys::k_query_event_handler_key)
                               ->getDefaultResumableSuccess();
    result_data = vda5050pp::events::QueryPauseResumeResult{};
    if (default_success.value_or(true)) {
      result_data->errors = {};
      result_data->notify = true;
    } else {
      vda5050::Error err;
      err.errorType = "controlError";
      err.errorDescription = "Cannot resume order by configured default.";
      err.errorLevel = vda5050::ErrorLevel::WARNING;
      result_data->errors = {std::move(err)};
    }
  }

  result.setValue(std::move(*result_data));
}

void QueryEventHandler::handleQueryAcceptZoneSet(
    std::shared_ptr<vda5050pp::events::QueryAcceptZoneSet> data) const noexcept(false) {
  if (data == nullptr) {
    throw vda5050pp::VDA5050PPInvalidEventData(MK_EX_CONTEXT("QueryAcceptZoneSetEvent is empty."));
  }

  getAGVHandlerLogger()->debug("handleQueryAcceptZoneSet(zone_set_id={})", data->zone_set_id);

  auto result = data->acquireResultToken();
  if (!result.isAcquired()) {
    getAGVHandlerLogger()->debug("Could not acquire QueryAcceptZoneSetEvent result token");
    return;
  }

  std::optional<std::list<vda5050::Error>> result_data;

  // Try to get an answer with the handler
  if (auto ptr = Instance::ref().getQueryHandler().lock(); ptr) {
    getAGVHandlerLogger()->debug("handleQueryAcceptZoneSet(): using query handler");
    result_data = ptr->callQueryAcceptZoneSet(data->zone_set_id);
  }

  // If there is still no result_data, use default accept set
  if (!result_data) {
    auto default_sets = Instance::ref()
                            .getConfig()
                            .lookupModuleConfigAs<vda5050pp::config::QueryEventHandlerSubConfig>(
                                module_keys::k_query_event_handler_key)
                            ->getDefaultAcceptZoneSets();
    if (default_sets.has_value()) {
      getAGVHandlerLogger()->debug("handleQueryAcceptZoneSet(): validating against default set");
      if (common::contains(*default_sets, data->zone_set_id)) {
        result_data = std::list<vda5050::Error>{};
      } else {
        vda5050::Error err;
        err.errorType = "orderError";
        err.errorDescription = fmt::format("Cannot accept zone set \"{}\". Allowed sets: {}",
                                           data->zone_set_id, *default_sets);
        err.errorLevel = vda5050::ErrorLevel::WARNING;
        err.errorReferences = {{"order.zoneSetId", data->zone_set_id}};
        result_data = {{err}};
      }
    }
  }

  // If there is still no result_data, use default success
  if (!result_data) {
    getAGVHandlerLogger()->debug("handleQueryAcceptZoneSet(): falling back to default success");
    auto default_success = Instance::ref()
                               .getConfig()
                               .lookupModuleConfigAs<vda5050pp::config::QueryEventHandlerSubConfig>(
                                   module_keys::k_query_event_handler_key)
                               ->getDefaultAcceptZoneSetSuccess();
    if (default_success.value_or(true)) {
      result_data = std::list<vda5050::Error>{};
    } else {
      vda5050::Error err;
      err.errorType = "controlError";
      err.errorDescription = "Cannot accept zone set order by configured default.";
      err.errorLevel = vda5050::ErrorLevel::WARNING;
      result_data = std::list<vda5050::Error>{std::move(err)};
    }
  }

  result.setValue(std::move(*result_data));
}

void QueryEventHandler::initialize(vda5050pp::core::Instance &instance) {
  this->subscriber_ = instance.getQueryEventManager().getScopedQueryEventSubscriber();
  this->subscriber_->subscribe(std::bind(std::mem_fn(&QueryEventHandler::handleQueryPauseableEvent),
                                         this, std::placeholders::_1));
  this->subscriber_->subscribe(std::bind(std::mem_fn(&QueryEventHandler::handleQueryResumableEvent),
                                         this, std::placeholders::_1));
  this->subscriber_->subscribe(std::bind(std::mem_fn(&QueryEventHandler::handleQueryAcceptZoneSet),
                                         this, std::placeholders::_1));
}

void QueryEventHandler::deinitialize(vda5050pp::core::Instance &) { this->subscriber_.reset(); }

std::string_view QueryEventHandler::describe() const { return "QueryEventHandler"; }

std::shared_ptr<vda5050pp::config::ModuleSubConfig> QueryEventHandler::generateSubConfig() const {
  return std::make_shared<vda5050pp::config::QueryEventHandlerSubConfig>();
}