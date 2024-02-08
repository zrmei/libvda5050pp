// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
#include "vda5050++/config/key_value_config.h"

#include "vda5050++/core/config.h"

using namespace vda5050pp::config;

template <typename T>
inline std::optional<T> doGetFromTable(toml::node_view<const toml::node> table,
                                       std::string_view key, std::optional<T> defaultValue,
                                       bool required, std::function<bool(T &)> validator) {
  if (auto val = table[key].value<T>(); val.has_value()) {
    // Run validator on parsed input
    if (validator && !validator(val.value())) {
      throw vda5050pp::VDA5050PPTOMLError(MK_FN_EX_CONTEXT(fmt::format(
          "Validator for custom config entry (k={},v={}) returned false", key, val.value())));
    }
    return val.value();
  } else if (defaultValue.has_value()) {
    // Return default value
    return defaultValue;

  } else if (required) {
    // Required but not value available
    throw vda5050pp::VDA5050PPTOMLError(
        MK_FN_EX_CONTEXT(fmt::format("Required custom config entry (k={}) not found", key)));
  } else {
    // Not require and no value available
    return std::nullopt;
  }
}

void KeyValueConfig::getFrom(const ConstConfigNode &node) {
  auto table = vda5050pp::core::config::ConstConfigNode::upcast(node).get();

  // Iterate over all entries and get them from the table
  for (auto &[key, container] : this->entries_) {
    if (container.type.get() == typeid(int64_t)) {
      container.value = doGetFromTable<int64_t>(
          table, key, std::get<std::optional<int64_t>>(container.default_value), container.required,
          std::get<std::function<bool(int64_t)>>(container.validator));
    } else if (container.type.get() == typeid(double)) {
      container.value = doGetFromTable<double>(
          table, key, std::get<std::optional<double>>(container.default_value), container.required,
          std::get<std::function<bool(double)>>(container.validator));
    } else if (container.type.get() == typeid(bool)) {
      container.value = doGetFromTable<bool>(
          table, key, std::get<std::optional<bool>>(container.default_value), container.required,
          std::get<std::function<bool(bool)>>(container.validator));
    } else if (container.type.get() == typeid(std::string)) {
      container.value = doGetFromTable<std::string>(
          table, key, std::get<std::optional<std::string>>(container.default_value),
          container.required, std::get<std::function<bool(std::string_view)>>(container.validator));
    } else if (container.type.get() == typeid(std::shared_ptr<vda5050pp::config::SubConfig>)) {
      vda5050pp::core::config::ConstConfigNode node_wrapped(table[key].node());
      auto sub_config = std::get<std::shared_ptr<vda5050pp::config::SubConfig>>(container.value);
      sub_config->getFrom(node_wrapped);
    } else {
      throw vda5050pp::VDA5050PPInvalidConfiguration(
          MK_EX_CONTEXT(fmt::format("Unknown type for custom config entry (k={})", key)));
    }
  }
}

void KeyValueConfig::putTo(ConfigNode &node) const {
  auto table = vda5050pp::core::config::ConfigNode::upcast(node).get().as_table();

  for (const auto &[key, container] : this->entries_) {
    if (container.type.get() == typeid(int64_t)) {
      if (const auto &v = std::get<std::optional<int64_t>>(container.value); v) {
        table->insert(key, *v);
      }
    } else if (container.type.get() == typeid(double)) {
      if (const auto &v = std::get<std::optional<double>>(container.value); v) {
        table->insert(key, *v);
      }
    } else if (container.type.get() == typeid(bool)) {
      if (const auto &v = std::get<std::optional<bool>>(container.value); v) {
        table->insert(key, *v);
      }
    } else if (container.type.get() == typeid(std::string)) {
      if (const auto &v = std::get<std::optional<std::string>>(container.value); v) {
        table->insert(key, *v);
      }
    } else if (container.type.get() == typeid(std::shared_ptr<vda5050pp::config::SubConfig>)) {
      toml::table sub_table;
      vda5050pp::core::config::ConfigNode node_wrapped(sub_table);
      auto sub_config = std::get<std::shared_ptr<vda5050pp::config::SubConfig>>(container.value);
      sub_config->putTo(node_wrapped);
      table->insert(key, sub_table);
    } else {
      throw vda5050pp::VDA5050PPInvalidConfiguration(
          MK_EX_CONTEXT(fmt::format("Unknown type for custom config entry (k={})", key)));
    }
  }
}

void KeyValueConfig::addIntegerEntry(std::string_view key, bool required,
                                     std::optional<int64_t> defaultValue,
                                     const std::function<bool(int64_t)> &validator) {
  this->entries_.insert_or_assign(std::string(key), Container{
                                                        typeid(int64_t),
                                                        required,
                                                        std::optional<int64_t>{},
                                                        defaultValue,
                                                        validator,
                                                    });
}

void KeyValueConfig::addFloatEntry(std::string_view key, bool required,
                                   std::optional<double> defaultValue,
                                   const std::function<bool(double)> &validator) {
  this->entries_.insert_or_assign(std::string(key), Container{
                                                        typeid(double),
                                                        required,
                                                        std::optional<double>{},
                                                        defaultValue,
                                                        validator,
                                                    });
}

void KeyValueConfig::addStringEntry(std::string_view key, bool required,
                                    std::optional<std::string> defaultValue,
                                    const std::function<bool(std::string_view)> &validator) {
  this->entries_.insert_or_assign(std::string(key), Container{
                                                        typeid(std::string),
                                                        required,
                                                        std::optional<std::string>{},
                                                        defaultValue,
                                                        validator,
                                                    });
}

void KeyValueConfig::addBooleanEntry(std::string_view key, bool required,
                                     std::optional<bool> defaultValue,
                                     const std::function<bool(bool)> &validator) {
  this->entries_.insert_or_assign(std::string(key), Container{
                                                        typeid(bool),
                                                        required,
                                                        std::optional<bool>{},
                                                        defaultValue,
                                                        validator,
                                                    });
}

void KeyValueConfig::addSubConfigEntry(std::string_view table_name,
                                       std::shared_ptr<vda5050pp::config::SubConfig> sub_config) {
  if (sub_config == nullptr) {
    throw vda5050pp::VDA5050PPInvalidArgument(MK_EX_CONTEXT("sub_config must not be nullptr"));
  }

  this->entries_.insert_or_assign(
      std::string(table_name),
      Container{
          typeid(std::shared_ptr<vda5050pp::config::SubConfig>), false, sub_config,
          std::optional<bool>{},        // Use arbitrary type, since this is unused
          std::function<bool(bool)>{},  // Use arbitrary type, since this is unused
      });
}

std::optional<int64_t> KeyValueConfig::getInteger(std::string_view key) const noexcept(false) {
  if (auto it = this->entries_.find(key); it != this->entries_.end()) {
    if (std::holds_alternative<std::optional<int64_t>>(it->second.value)) {
      return std::get<std::optional<int64_t>>(it->second.value);
    } else {
      throw vda5050pp::VDA5050PPInvalidArgument(
          MK_EX_CONTEXT(fmt::format("Entry with key {} is not an integer", key)));
    }
  } else {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("Entry with key {} not assigned", key)));
  }
}

std::optional<double> KeyValueConfig::getFloat(std::string_view key) const noexcept(false) {
  if (auto it = this->entries_.find(key); it != this->entries_.end()) {
    if (std::holds_alternative<std::optional<double>>(it->second.value)) {
      return std::get<std::optional<double>>(it->second.value);
    } else {
      throw vda5050pp::VDA5050PPInvalidArgument(
          MK_EX_CONTEXT(fmt::format("Entry with key {} is not a float", key)));
    }
  } else {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("Entry with key {} not assigned", key)));
  }
}

std::optional<std::string_view> KeyValueConfig::getString(std::string_view key) const
    noexcept(false) {
  if (auto it = this->entries_.find(key); it != this->entries_.end()) {
    if (std::holds_alternative<std::optional<std::string>>(it->second.value)) {
      return std::get<std::optional<std::string>>(it->second.value);
    } else {
      throw vda5050pp::VDA5050PPInvalidArgument(
          MK_EX_CONTEXT(fmt::format("Entry with key {} is not a string", key)));
    }
  } else {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("Entry with key {} not assigned", key)));
  }
}

std::optional<bool> KeyValueConfig::getBoolean(std::string_view key) const noexcept(false) {
  if (auto it = this->entries_.find(key); it != this->entries_.end()) {
    if (std::holds_alternative<std::optional<bool>>(it->second.value)) {
      return std::get<std::optional<bool>>(it->second.value);
    } else {
      throw vda5050pp::VDA5050PPInvalidArgument(
          MK_EX_CONTEXT(fmt::format("Entry with key {} is not a boolean", key)));
    }
  } else {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("Entry with key {} not assigned", key)));
  }
}

std::shared_ptr<vda5050pp::config::SubConfig> KeyValueConfig::getSubConfig(
    std::string_view table_name) const noexcept(false) {
  if (auto it = this->entries_.find(table_name); it != this->entries_.end()) {
    if (std::holds_alternative<std::shared_ptr<vda5050pp::config::SubConfig>>(it->second.value)) {
      return std::get<std::shared_ptr<vda5050pp::config::SubConfig>>(it->second.value);
    } else {
      throw vda5050pp::VDA5050PPInvalidArgument(
          MK_EX_CONTEXT(fmt::format("Entry with table_name {} is not a SubConfig", table_name)));
    }
  } else {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("Entry with table_name {} not assigned", table_name)));
  }
}

void KeyValueConfig::setInteger(std::string_view key, int64_t value) noexcept(false) {
  if (auto it = this->entries_.find(key); it != this->entries_.end()) {
    if (it->second.type.get() == typeid(int64_t)) {
      it->second.value = std::make_optional(value);
    } else {
      throw vda5050pp::VDA5050PPInvalidArgument(
          MK_EX_CONTEXT(fmt::format("Entry with key {} is not an integer", key)));
    }
  } else {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("Entry with key {} not assigned", key)));
  }
}

void KeyValueConfig::setFloat(std::string_view key, double value) noexcept(false) {
  if (auto it = this->entries_.find(key); it != this->entries_.end()) {
    if (it->second.type.get() == typeid(double)) {
      it->second.value = std::make_optional(value);
    } else {
      throw vda5050pp::VDA5050PPInvalidArgument(
          MK_EX_CONTEXT(fmt::format("Entry with key {} is not a float", key)));
    }
  } else {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("Entry with key {} not assigned", key)));
  }
}

void KeyValueConfig::setString(std::string_view key, std::string_view value) noexcept(false) {
  if (auto it = this->entries_.find(key); it != this->entries_.end()) {
    if (it->second.type.get() == typeid(std::string)) {
      it->second.value = std::make_optional<std::string>(value);
    } else {
      throw vda5050pp::VDA5050PPInvalidArgument(
          MK_EX_CONTEXT(fmt::format("Entry with key {} is not a string", key)));
    }
  } else {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("Entry with key {} not assigned", key)));
  }
}

void KeyValueConfig::setBoolean(std::string_view key, bool value) noexcept(false) {
  if (auto it = this->entries_.find(key); it != this->entries_.end()) {
    if (it->second.type.get() == typeid(bool)) {
      it->second.value = std::make_optional(value);
    } else {
      throw vda5050pp::VDA5050PPInvalidArgument(
          MK_EX_CONTEXT(fmt::format("Entry with key {} is not a boolean", key)));
    }
  } else {
    throw vda5050pp::VDA5050PPInvalidArgument(
        MK_EX_CONTEXT(fmt::format("Entry with key {} not assigned", key)));
  }
}