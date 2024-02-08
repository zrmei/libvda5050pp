// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains the QueryEventHandlerSubConfig class.
//
//

#ifndef PUBLIC_VDA5050_2B_2B_CONFIG_QUERY_EVENT_HANDLER_SUBCONFIG_H_
#define PUBLIC_VDA5050_2B_2B_CONFIG_QUERY_EVENT_HANDLER_SUBCONFIG_H_

#include <optional>
#include <set>
#include <string>

#include "vda5050++/config/module_subconfig.h"

namespace vda5050pp::config {

///
///\brief The QueryEventHandler sub config.
///
/// Contains:
/// - default pauseable/resumable
/// - default accept zone set behaviour
///
///
class QueryEventHandlerSubConfig : public vda5050pp::config::ModuleSubConfig {
private:
  std::optional<bool> default_pauseable_success_;
  std::optional<bool> default_resumable_success_;
  std::optional<bool> default_accept_zone_set_success_;
  std::optional<std::set<std::string, std::less<>>> default_accept_zone_sets_;

protected:
  ///
  ///\brief Read all module settings from the ConfigNode.
  ///
  ///\param node the ConfigNode to read from
  ///
  void getFrom(const ConstConfigNode &node) override;

  ///
  ///\brief Write all module setting sto the ConfigNode.
  ///
  ///\param node the ConfigNode to write to
  ///
  void putTo(ConfigNode &node) const override;

public:
  ///
  ///\brief Set the default PauseableSuccess value. If no handler is set, this determines the
  /// answer.
  ///
  ///\param value std::nullopt if there is no default, otherwise use the given default success.
  ///
  void setDefaultPauseableSuccess(std::optional<bool> value);

  ///
  ///\brief Get the default PauseableSuccess value.
  ///
  ///\return std::optional<bool>
  ///
  std::optional<bool> getDefaultPauseableSuccess() const;

  ///
  ///\brief Set the default ResumableSuccess value. If no handler is set, this determines the
  /// answer.
  ///
  ///\param value std::nullopt if there is no default, otherwise use the given default success.
  ///
  void setDefaultResumableSuccess(std::optional<bool> value);

  ///
  ///\brief Get the default ResumableSuccess value.
  ///
  ///\return std::optional<bool>
  ///
  std::optional<bool> getDefaultResumableSuccess() const;

  ///
  ///\brief Set the default AcceptZoneSet value. If no handler is set, this determines the answer.
  ///
  ///\param value std::nullopt if there is no default, otherwise use the given default success.
  ///
  void setDefaultAcceptZoneSetSuccess(std::optional<bool> value);

  ///
  ///\brief Get the default AcceptZoneSetSuccess value.
  ///
  ///\return std::optional<bool>
  ///
  std::optional<bool> getDefaultAcceptZoneSetSuccess() const;

  ///
  ///\brief Set the a list if allowed zone-sets to check against, if ther is no handler answering
  /// this query.
  ///
  ///\param value std::nullopt if this check shall not be used. Otherwise check against the set.
  ///
  void setDefaultAcceptZoneSets(const std::set<std::string, std::less<>> &value);

  ///
  ///\brief Get the default AcceptZoneSets.
  ///
  ///\return const std::optional<std::set<std::string>>&
  ///
  const std::optional<std::set<std::string, std::less<>>> &getDefaultAcceptZoneSets() const;
};

}  // namespace vda5050pp::config

#endif  // PUBLIC_VDA5050_2B_2B_CONFIG_QUERY_EVENT_HANDLER_SUBCONFIG_H_
