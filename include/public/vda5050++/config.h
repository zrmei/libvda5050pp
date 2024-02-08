//  Copyright Open Logistics Foundation
//
//  Licensed under the Open Logistics Foundation License 1.3.
//  For details on the licensing terms, see the LICENSE file.
//  SPDX-License-Identifier: OLFL-1.3
//

#ifndef INCLUDE_PUBLIC_VDA5050_2B_2B_CONFIG_H_
#define INCLUDE_PUBLIC_VDA5050_2B_2B_CONFIG_H_

#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "vda5050++/agv_description/agv_description.h"
#include "vda5050++/config/agv_description_subconfig.h"
#include "vda5050++/config/event_manager_options.h"
#include "vda5050++/config/global_config.h"
#include "vda5050++/config/module_subconfig.h"
#include "vda5050++/config/mqtt_subconfig.h"
#include "vda5050++/config/node_reached_subconfig.h"
#include "vda5050++/config/query_event_handler_subconfig.h"
#include "vda5050++/config/state_update_timer_subconfig.h"
#include "vda5050++/config/types.h"
#include "vda5050++/config/visualization_timer_subconfig.h"

namespace vda5050pp {

///\brief This class represents the configuration of the libvda5050++.
class Config {
private:
  config::AGVDescriptionSubConfig agv_description_sub_config_;
  config::GlobalConfig global_config_;
  std::map<std::string, std::shared_ptr<config::ModuleSubConfig>, std::less<>>
      module_config_registry_;
  std::map<std::string, std::shared_ptr<config::SubConfig>, std::less<>> custom_config_registry_;

protected:
  ///
  ///\brief Register a module config. This is currently only used by the library itself. Will be
  /// inside the "module" TOML Table
  /// \param key the module key \param sub_config the sub_config
  /// object \throws vda5050pp::VDA5050PPInvalidArgument if a module with this key is already
  /// registered
  ///
  void registerModuleConfig(std::string_view key,
                            std::shared_ptr<config::ModuleSubConfig> sub_config) noexcept(false);

public:
  /// \brief Construct a default config.
  Config();

  /// \brief Load a stored config from a TOML file. (Does not restore agv config).
  /// \param toml_file The TOML file path
  ///\throws vda5050pp::VDA5050PPTOMLError when the file could not be parser properly
  /// \return the loaded config
  static Config loadFrom(const std::filesystem::path &toml_file) noexcept(false);

  /// \brief Load a stored config from a TOML string. (Does not restore agv config).
  /// \param toml_string The TOML string
  ///\throws vda5050pp::VDA5050PPTOMLError when the string could not be parser properly
  /// \return the loaded config
  static Config loadFrom(std::string_view toml_string) noexcept(false);

  ///\brief Get a module sub config by key.
  ///\param key the module key
  ///\throws vda5050pp::VDA5050PPInvalidArgument when the key is not registered
  ///\return a shared_ptr to the ModuleSubConfig
  [[nodiscard]] std::shared_ptr<config::ModuleSubConfig> lookupModuleConfig(
      std::string_view key) const noexcept(false);

  ///\brief Get a module sub config by key and try to upcast it.
  ///\tparam T the specialized ModuleSubConfig type
  ///\param key the module key
  ///\throws vda5050pp::VDA5050PPInvalidArgument when the key is not registered.
  ///\throws vda5050pp::VDA5050PPBadCast on invalid upcast
  ///\return a shared_ptr to the upcasted module.
  template <typename T>
  [[nodiscard]] std::shared_ptr<T> inline lookupModuleConfigAs(std::string_view key) const
      noexcept(false) {
    return config::SubConfig::ptr_as<T>(lookupModuleConfig(key));
  }

  ///\brief Register a new custom SubConfig. Will be inside the "custom" TOML Table.
  ///\param key the subconfig key (unique amongst custom configs)
  ///\param sub_config a shared_ptr to the sub_config
  ///\throws vda5050pp::VDA5050PPInvalidArgument when the key is already in use
  void registerCustomConfig(std::string_view key,
                            std::shared_ptr<config::SubConfig> sub_config) noexcept(false);

  ///\brief Get a custom sub config by key.
  ///\param key the sub config key
  ///\throws vda5050pp::VDA5050PPInvalidArgument when the key is not registered.
  ///\return the sub config
  [[nodiscard]] std::shared_ptr<config::SubConfig> lookupCustomConfig(std::string_view key) const;

  ///\brief Get a custom sub config by key and try to upcast it.
  ///\tparam T the specialized SubConfig type
  ///\param key the config key
  ///\throws vda5050pp::VDA5050PPInvalidArgument when the key is not registered.
  ///\throws vda5050pp::VDA5050PPBadCast on invalid upcast
  ///\return a shared_ptr to the upcasted module.
  template <typename T>
  [[nodiscard]] std::shared_ptr<T> inline lookupCustomConfigAs(std::string_view key) const
      noexcept(false) {
    return config::SubConfig::ptr_as<T>(lookupCustomConfig(key));
  }

  ///\brief Remove a custom sub config
  ///\param key the key of the custom sub config
  ///\throws vda5050pp::VDA5050PPInvalidArgument when the key is not registered.
  ///\return std::shared_ptr<config::SubConfig> the removed SubConfig
  std::shared_ptr<config::SubConfig> removeCustomConfig(std::string_view key) noexcept(false);

  ///\brief Get the global config
  ///\return const config::GlobalConfig&
  const config::GlobalConfig &getGlobalConfig() const;

  ///\brief Get a reference to the global config
  ///\return config::GlobalConfig&
  config::GlobalConfig &refGlobalConfig();

  ///\brief Set the global config
  ///\param cfg the new global config
  void setGlobalConfig(const config::GlobalConfig &cfg);

  ///\brief Get a reference to the mqtt SubConfig
  ///\return vda5050pp::config::MqttSubConfig&
  vda5050pp::config::MqttSubConfig &refMqttSubConfig();

  ///\brief Get a const reference to the mqtt SubConfig
  ///\return const vda5050pp::config::MqttSubConfig&
  const vda5050pp::config::MqttSubConfig &getMqttSubConfig() const;

  ///\brief Load a config file
  ///\param toml_file the config file path
  void load(const std::filesystem::path &toml_file);

  ///\brief Load a config string
  ///\param toml_file the config string
  void load(std::string_view toml_string);

  ///\brief Save this config to a TOML file. (Without agv description)
  ///\param toml_file the config file path
  void save(const std::filesystem::path &toml_file) const;

  ///\brief Save this config to a TOML string. (Without agv description)
  ///\param to_toml_string the config string to write to
  void save(std::string &to_toml_string) const;

  ///\brief Set the AGVDescription
  ///\param desc the new AGVDescription
  void setAgvDescription(const vda5050pp::agv_description::AGVDescription &desc);

  ///\brief Get the current AGVDescription
  ///\return const vda5050pp::agv_description::AGVDescription&
  const vda5050pp::agv_description::AGVDescription &getAgvDescription() const;

  ///\brief Get a writeable reference to the current AGVDescription
  ///\return const vda5050pp::agv_description::AGVDescription&
  vda5050pp::agv_description::AGVDescription &refAgvDescription();

  ///
  ///\brief Get a writeable reference to the current AGVDescriptionSubConfig.
  ///
  ///\return vda5050pp::config::AGVDescriptionSubConfig&
  ///
  vda5050pp::config::AGVDescriptionSubConfig &refAgvDescriptionSubConfig();

  ///
  ///\brief Get a writeable reference to the current NodeReachedSubConfig.
  ///
  ///\return vda5050pp::config::NodeReachedSubConfig&
  ///
  vda5050pp::config::NodeReachedSubConfig &refNodeReachedSubConfig();

  ///
  ///\brief Get a writeable reference to the current QueryEventHandlerSubConfig.
  ///
  ///\return vda5050pp::config::QueryEventHandlerSubConfig&
  ///
  vda5050pp::config::QueryEventHandlerSubConfig &refQueryEventHandlerSubConfig();

  ///
  ///\brief Get a writeable reference to the current StateUpdateTimerSubConfig.
  ///
  ///\return vda5050pp::config::StateUpdateTimerSubConfig&
  ///
  vda5050pp::config::StateUpdateTimerSubConfig &refStateUpdateTimerSubConfig();

  ///
  ///\brief Get a writeable reference to the current VisualizationTimerSubConfig.
  ///
  ///\return vda5050pp::config::VisualizationTimerSubConfig&
  ///
  vda5050pp::config::VisualizationTimerSubConfig &refVisualizationTimerSubConfig();
};

}  // namespace vda5050pp

#endif  // INCLUDE_PUBLIC_VDA5050_2B_2B_CONFIG_H_
