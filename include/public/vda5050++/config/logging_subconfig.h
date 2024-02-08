//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_CONFIG_LOGGING_SUBCONFIG_H_
#define PUBLIC_VDA5050_2B_2B_CONFIG_LOGGING_SUBCONFIG_H_

#include <optional>
#include <string>

#include "vda5050++/config/types.h"

namespace vda5050pp::config {

///\brief The log level used in configs
enum class LogLevel {
  k_debug,
  k_info,
  k_warn,
  k_error,
  k_off,
};

///\brief A specialized SubConfig, which includes logging settings
class LoggingSubConfig : public SubConfig {
private:
  std::optional<LogLevel> log_level_;
  std::optional<std::string> log_file_name_;

protected:
  ///\brief Get "log_level" and "log_file_name" from the config node.
  /// NOTE: If you overwrite this function, make sure to call it from within that function.
  ///\param node the config node
  void getFrom(const ConstConfigNode &node) override;

  ///\brief Write "log_level" and "log_file_name" to the config node.
  /// NOTE: If you overwrite this function, make sure to call it from within that function.
  ///\param node the config node
  void putTo(ConfigNode &node) const override;

public:
  ///\brief Set the log level.
  ///\param level the log level
  void setLogLevel(std::optional<LogLevel> level);

  ///\brief Get the current log level
  ///\return std::optional<LogLevel>
  std::optional<LogLevel> getLogLevel() const;

  ///\brief Set an optional log file
  ///\param file_name the logfile
  void setLogFileName(std::optional<std::string_view> file_name);

  ///\brief Get the current log file
  ///\return std::optional<std::string_view> the log file
  std::optional<std::string_view> getLogFileName() const;
};
}  // namespace vda5050pp::config

#endif  // PUBLIC_VDA5050_2B_2B_CONFIG_LOGGING_SUBCONFIG_H_
