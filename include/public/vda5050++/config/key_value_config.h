// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//

#ifndef PUBLIC_VDA5050_2B_2B_CONFIG_CUSTOM_CONFIG_H_
#define PUBLIC_VDA5050_2B_2B_CONFIG_CUSTOM_CONFIG_H_

#include <functional>
#include <map>
#include <string>
#include <variant>

#include "vda5050++/config/types.h"
#include "vda5050++/misc/any_ptr.h"

namespace vda5050pp::config {

///
///\brief A KeyValueConfig can be used by the user to register a custom config.
///
/// It does not expose the toml dependency and provides the option to add primitive key value
/// entries.
///
class KeyValueConfig : public SubConfig {
private:
  ///\brief Restore this object from a ConstConfigNode
  ///\param node the config node
  void getFrom(const ConstConfigNode &node) override;

  ///\brief Dump this object to a ConfigNode
  ///\param node the config node
  void putTo(ConfigNode &node) const override;

  ///
  ///\brief Storage struct for the key-value pairs.
  ///
  /// Contains type info, required flag, actual value, default value and a validator.
  ///
  struct Container {
    std::reference_wrapper<const std::type_info> type;
    bool required = false;
    std::variant<std::optional<int64_t>, std::optional<double>, std::optional<bool>,
                 std::optional<std::string>, std::shared_ptr<vda5050pp::config::SubConfig>>
        value;
    std::variant<std::optional<int64_t>, std::optional<double>, std::optional<bool>,
                 std::optional<std::string>>
        default_value;
    std::variant<std::function<bool(int64_t)>, std::function<bool(double)>,
                 std::function<bool(bool)>, std::function<bool(std::string_view)>>
        validator;
  };

  std::map<std::string, Container, std::less<>> entries_;

public:
  ~KeyValueConfig() override = default;

  ///
  ///\brief Add a new integer (signed 64-bit) entry to the config.
  ///
  ///\param key The entry's key
  ///\param required Is this entry required?
  ///\param defaultValue An optional default value.
  ///\param validator A validator for read values.
  ///
  void addIntegerEntry(std::string_view key, bool required,
                       std::optional<int64_t> defaultValue = std::nullopt,
                       const std::function<bool(int64_t)> &validator = nullptr);

  ///
  ///\brief Add a new float (64-bit) entry to the config.
  ///
  ///\param key The entry's key
  ///\param required Is this entry required?
  ///\param defaultValue An optional default value.
  ///\param validator A validator for read values.
  ///
  void addFloatEntry(std::string_view key, bool required,
                     std::optional<double> defaultValue = std::nullopt,
                     const std::function<bool(double)> &validator = nullptr);

  ///
  ///\brief Add a new string entry to the config.
  ///
  ///\param key The entry's key
  ///\param required Is this entry required?
  ///\param defaultValue An optional default value.
  ///\param validator A validator for read values.
  ///
  void addStringEntry(std::string_view key, bool required,
                      std::optional<std::string> defaultValue = std::nullopt,
                      const std::function<bool(std::string_view)> &validator = nullptr);

  ///
  ///\brief Add a new boolean entry to the config.
  ///
  ///\param key The entry's key
  ///\param required Is this entry required?
  ///\param defaultValue An optional default value.
  ///\param validator A validator for read values.
  ///
  void addBooleanEntry(std::string_view key, bool required,
                       std::optional<bool> defaultValue = std::nullopt,
                       const std::function<bool(bool)> &validator = nullptr);

  ///
  ///\brief Add a sub configuration to this config. (Has its own toml sub-table)
  ///
  ///\param table_name the name of the sub-table (in the same namespace as all other entries)
  ///\param sub_config the sub_config pointer (must not be nullptr)
  ///\throws vda5050pp::VDA5050InvalidArgumentError when sub_config is nullptr
  ///
  void addSubConfigEntry(std::string_view table_name,
                         std::shared_ptr<vda5050pp::config::SubConfig> sub_config) noexcept(false);

  ///
  ///\brief Get the value of an integer entry. The config must have been loaded before.
  ///
  ///\param key The entry's key
  ///\return std::optional<int64_t> the value if it is present
  ///\throws vda5050pp::VDA5050InvalidArgumentError when this entry was not added or not integer
  ///
  std::optional<int64_t> getInteger(std::string_view key) const noexcept(false);

  ///
  ///\brief Get the value of an float entry. The config must have been loaded before.
  ///
  ///\param key The entry's key
  ///\return std::optional<double> the value if it is present
  ///\throws vda5050pp::VDA5050InvalidArgumentError when this entry was not added or not float
  ///
  std::optional<double> getFloat(std::string_view key) const noexcept(false);

  ///
  ///\brief Get the value of an string entry. The config must have been loaded before.
  ///
  ///\param key The entry's key
  ///\return std::optional<std::string> the value if it is present
  ///\throws vda5050pp::VDA5050InvalidArgumentError when this entry was not added or not string
  ///
  std::optional<std::string_view> getString(std::string_view key) const noexcept(false);

  ///
  ///\brief Get the value of an boolean entry. The config must have been loaded before.
  ///
  ///\param key The entry's key
  ///\return std::optional<bool> the value if it is present
  ///\throws vda5050pp::VDA5050InvalidArgumentError when this entry was not added or not boolean
  ///
  std::optional<bool> getBoolean(std::string_view key) const noexcept(false);

  ///
  ///\brief Get the value of an SubConfig entry.
  ///
  ///\param table_name The table's name
  ///\return std::shared_ptr<vda5050pp::config::SubConfig> the value if it is present
  ///\throws vda5050pp::VDA5050InvalidArgumentError when this table was not added or is not a
  /// SubConfig
  ///
  std::shared_ptr<vda5050pp::config::SubConfig> getSubConfig(std::string_view table_name) const
      noexcept(false);

  ///
  ///\brief Get the value of an SubConfig entry and cast it to T.
  ///
  ///\tparam T the specialized SubConfig type
  ///\param table_name The table's name
  ///\return std::shared_ptr<T> the value if it is present
  ///\throws vda5050pp::VDA5050InvalidArgumentError when this table was not added or is not a
  /// SubConfig
  ///
  template <typename T>
  inline std::shared_ptr<T> getSubConfigAs(std::string_view table_name) const noexcept(false) {
    return vda5050pp::config::SubConfig::ptr_as<T>(this->getSubConfig(table_name));
  }

  ///
  ///\brief Set the value of an integer entry
  ///
  ///\param key the entry's key
  ///\param value the entry's value
  ///\throws vda5050pp::VDA5050InvalidArgumentError when this entry was not added or not boolean
  ///
  void setInteger(std::string_view key, int64_t value) noexcept(false);

  ///
  ///\brief Set the value of a float entry
  ///
  ///\param key the entry's key
  ///\param value the entry's value
  ///\throws vda5050pp::VDA5050InvalidArgumentError when this entry was not added or not boolean
  ///
  void setFloat(std::string_view key, double value) noexcept(false);

  ///
  ///\brief Set the value of a string entry
  ///
  ///\param key the entry's key
  ///\param value the entry's value
  ///\throws vda5050pp::VDA5050InvalidArgumentError when this entry was not added or not boolean
  ///
  void setString(std::string_view key, std::string_view value) noexcept(false);

  ///
  ///\brief Set the value of a boolean entry
  ///
  ///\param key the entry's key
  ///\param value the entry's value
  ///\throws vda5050pp::VDA5050InvalidArgumentError when this entry was not added or not boolean
  ///
  void setBoolean(std::string_view key, bool value) noexcept(false);
};
}  // namespace vda5050pp::config

#endif  // PUBLIC_VDA5050_2B_2B_CONFIG_CUSTOM_CONFIG_H_
