// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains the implementation of the BaseQueryHandler.
//
//

#include "vda5050++/handler/base_query_handler.h"

using namespace vda5050pp::handler;

std::optional<vda5050pp::handler::BaseQueryHandler::QueryPauseResumeResult>
BaseQueryHandler::callQueryPauseable() {
  std::unique_lock lock(this->call_query_pauseable_mutex_);
  this->overwrite_query_pauseable_ = true;

  if (auto result = this->queryPauseable(); this->overwrite_query_pauseable_) {
    return result;
  } else {
    return std::nullopt;
  }
}

std::optional<vda5050pp::handler::BaseQueryHandler::QueryPauseResumeResult>
BaseQueryHandler::callQueryResumable() {
  std::unique_lock lock(this->call_query_resumable_mutex_);
  this->overwrite_query_resumable_ = true;

  if (auto result = this->queryResumable(); this->overwrite_query_resumable_) {
    return result;
  } else {
    return std::nullopt;
  }
}

std::optional<std::list<vda5050::Error>> BaseQueryHandler::callQueryAcceptZoneSet(
    std::string_view zone_set_id) {
  std::unique_lock lock(this->call_query_accept_zone_set_mutex);
  this->overwrite_query_accept_zone_set_ = true;

  if (auto result = this->queryAcceptZoneSet(zone_set_id); this->overwrite_query_accept_zone_set_) {
    return result;
  } else {
    return std::nullopt;
  }
}

vda5050pp::handler::BaseQueryHandler::QueryPauseResumeResult BaseQueryHandler::queryPauseable() {
  this->overwrite_query_pauseable_ = false;
  return {};
}

vda5050pp::handler::BaseQueryHandler::QueryPauseResumeResult BaseQueryHandler::queryResumable() {
  this->overwrite_query_resumable_ = false;
  return {};
}

std::list<vda5050::Error> BaseQueryHandler::queryAcceptZoneSet(std::string_view) {
  this->overwrite_query_accept_zone_set_ = false;
  return {};
}