// Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/config/logging_subconfig.h"

#include "vda5050++/core/common/exception.h"
#include "vda5050++/core/config.h"

using namespace vda5050pp::config;

inline LogLevel logLevelFromString(std::string_view str) {
  if (str == "debug") {
    return LogLevel::k_debug;
  } else if (str == "info") {
    return LogLevel::k_info;
  } else if (str == "warn") {
    return LogLevel::k_warn;
  } else if (str == "error") {
    return LogLevel::k_error;
  } else if (str == "off") {
    return LogLevel::k_off;
  } else {
    throw vda5050pp::VDA5050PPTOMLError(
        MK_FN_EX_CONTEXT(fmt::format("Unknown LogLevel \"{}\"", str)));
  }
}

inline std::string_view logLevelToString(LogLevel level) {
  switch (level) {
    case LogLevel::k_debug:
      return "debug";
    case LogLevel::k_info:
      return "info";
    case LogLevel::k_warn:
      return "warn";
    case LogLevel::k_error:
      return "error";
    case LogLevel::k_off:
      return "off";
    default:
      throw vda5050pp::VDA5050PPTOMLError(
          MK_FN_EX_CONTEXT(fmt::format("Unknown LogLevel \"{}\"", int(level))));
  }
}

void LoggingSubConfig::setLogLevel(std::optional<LogLevel> level) { this->log_level_ = level; }

std::optional<LogLevel> LoggingSubConfig::getLogLevel() const { return this->log_level_; }

void LoggingSubConfig::setLogFileName(std::optional<std::string_view> file_name) {
  this->log_file_name_ = file_name;
}

std::optional<std::string_view> LoggingSubConfig::getLogFileName() const {
  return this->log_file_name_;
}

void LoggingSubConfig::getFrom(const ConstConfigNode &node) {
  auto node_view = core::config::ConstConfigNode::upcast(node).get();

  if (auto ll = node_view["log_level"].value<std::string_view>(); ll.has_value()) {
    this->log_level_ = logLevelFromString(ll.value());
  }
  this->log_file_name_ = node_view["log_file_name"].value<std::string_view>();
}

void LoggingSubConfig::putTo(ConfigNode &node) const {
  auto node_view = core::config::ConfigNode::upcast(node).get();
  auto &table = *node_view.as_table();

  if (this->log_level_.has_value()) {
    table.insert("log_level", logLevelToString(this->log_level_.value()));
  }
  if (this->log_file_name_.has_value()) {
    table.insert("log_file_name", this->log_file_name_.value());
  }
}