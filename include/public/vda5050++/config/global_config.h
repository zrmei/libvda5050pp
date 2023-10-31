//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_CONFIG_GLOBAL_CONFIG_H_
#define PUBLIC_VDA5050_2B_2B_CONFIG_GLOBAL_CONFIG_H_

#include <optional>

#include "vda5050++/agv_description/agv_description.h"
#include "vda5050++/config/event_manager_options.h"
#include "vda5050++/config/logging_subconfig.h"
#include "vda5050++/config/types.h"

namespace vda5050pp::config {

///
///\brief The GlobalConfig contains common libvda5050++ configuration settings.
///
/// Contents:
/// - The AGVDescription
/// - EventManagerOptions
/// - A Module black/white list
///
class GlobalConfig : public vda5050pp::config::LoggingSubConfig {
  std::optional<vda5050pp::agv_description::AGVDescription> agv_description_;
  vda5050pp::config::EventManagerOptions event_manager_options_;
  bool is_black_list_ = true;
  std::set<std::string, std::less<>> module_bw_list_;

protected:
  /// \brief Read EventManagerOptions and ModuleBWList from ConfigNode.
  /// \param node the ConfigNode to read from.
  void getFrom(const ConfigNode &node) override;

  /// \brief Write EventManagerOptions and ModuleBWList to ConfigNode.
  /// \param node the ConfigNode to write to.
  void putTo(ConfigNode &node) const override;

public:
  /// \brief Check if a module key is allowed (not blacklisted / whitelisted).
  /// \param key the module key
  /// \return should this module be loaded?
  bool isListedModule(std::string_view key) const;

  ///
  ///\brief Use the module list as blacklist
  ///
  void useBlackList();

  ///
  ///\brief Use the module list as whitelist
  ///
  void useWhiteList();

  ///
  ///\brief Add a module key to the module black/white list.
  ///
  ///\param key the key to add.
  ///
  void bwListModule(std::string_view key);

  ///
  ///\brief Remove a module key from the module black/white list.
  ///
  ///\param key the key to remove.
  ///
  void bwUnListModule(std::string_view key);

  ///
  ///\brief Get the current EventManagerOptions.
  ///
  ///\return const vda5050pp::config::EventManagerOptions&
  ///
  const vda5050pp::config::EventManagerOptions &getEventManagerOptions() const;

  ///
  ///\brief Get a writeable reference to the current EventManagerOptions.
  ///
  ///\return vda5050pp::config::EventManagerOptions&
  ///
  vda5050pp::config::EventManagerOptions &refEventManagerOptions();

  ///
  ///\brief Overwrite the current EventManagerOptions.
  ///
  ///\param opts the new EventManagerOptions.
  ///
  void setEventManagerOptions(const vda5050pp::config::EventManagerOptions &opts);
};

}  // namespace vda5050pp::config

#endif  // PUBLIC_VDA5050_2B_2B_CONFIG_GLOBAL_CONFIG_H_
