// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains the AGVDescriptionSubConfig implementation.
//

#include "vda5050++/config/agv_description_subconfig.h"

#include <filesystem>
#include <iostream>

#include "vda5050++/core/config.h"

using namespace vda5050pp::config;
using namespace std::string_view_literals;

static vda5050pp::agv_description::Battery fromBatteryTable(
    toml::node_view<const toml::node> node) {
  vda5050pp::agv_description::Battery battery;

  battery.charge_type = node["charge_type"].value_or<std::string_view>(""sv);
  battery.max_charge =
      node["max_charge"].value_or<double>(std::numeric_limits<double>::signaling_NaN());
  battery.max_reach = node["max_reach"].value<double>();
  battery.max_voltage =
      node["max_voltage"].value_or<double>(std::numeric_limits<double>::signaling_NaN());

  return battery;
}

static toml::table toBatteryTable(const vda5050pp::agv_description::Battery &battery) {
  toml::table table;

  table.insert("charge_type", battery.charge_type);
  table.insert("max_charge", battery.max_charge);
  if (battery.max_reach) {
    table.insert("max_reach", *battery.max_reach);
  }
  table.insert("max_voltage", battery.max_voltage);

  return table;
}

template <typename Target>
inline Target fromJSONTable(toml::node_view<const toml::node> node,
                            std::optional<AGVDescriptionSubConfig::JSONFile> &file,
                            std::string_view name) {
  Target target;

  if (auto json_str = node["json"].value<std::string_view>(); json_str) {
    try {
      target = vda5050::json::parse(*json_str);
      file = std::nullopt;
    } catch (const vda5050::json::exception &e) {
      throw vda5050pp::VDA5050PPInvalidConfiguration(
          MK_FN_EX_CONTEXT(fmt::format("Cannot parse {} json string: {}", name, e.what())));
    }
  } else if (auto json_file = node["json_file"].value<std::string_view>(); json_file) {
    std::filesystem::path path(*json_file);
    std::ifstream fs(path);
    fs.exceptions(std::ios_base::failbit);
    try {
      std::ostringstream ss;
      ss << fs.rdbuf();
      target = vda5050::json::parse(std::string_view(ss.str()));
      file = AGVDescriptionSubConfig::JSONFile{node["json_file_overwrite"].value_or<bool>(false),
                                               path};
    } catch (const std::ios_base::failure &) {
      // Format errno, set by ofs
      std::system_error sys_error{errno, std::iostream_category(), path.c_str()};
      throw vda5050pp::VDA5050PPInvalidConfiguration(
          MK_FN_EX_CONTEXT(fmt::format("Could not load {} json file: {}", name, sys_error.what())));
    } catch (const vda5050::json::exception &e) {
      throw vda5050pp::VDA5050PPInvalidConfiguration(
          MK_FN_EX_CONTEXT(fmt::format("Cannot parse {} json string: {}", name, e.what())));
    }
  }

  return target;
}

template <typename Target>
inline toml::table toJSONTable(const Target &target,
                               const std::optional<AGVDescriptionSubConfig::JSONFile> &file,
                               std::string_view name) {
  toml::table table;

  if (file && file->overwrite) {
    std::ofstream ofs(file->json_file);
    ofs.exceptions(std::ios_base::failbit);
    std::string contents = vda5050::json(target).dump(2);

    try {
      ofs << contents;
    } catch (const std::ios_base::failure &) {
      // Format errno, set by ofs
      std::system_error sys_error{errno, std::iostream_category(), file->json_file.c_str()};
      throw vda5050pp::VDA5050PPTOMLError(
          MK_FN_EX_CONTEXT(fmt::format("Could not write {} file: {}", name, sys_error.what())));
    }
    table.insert("json_file_overwrite", file->overwrite);
    table.insert("json_file", file->json_file.c_str());
  } else if (file && !file->overwrite) {
    table.insert("json_file_overwrite", file->overwrite);
    table.insert("json_file", file->json_file.c_str());
  } else {
    table.insert("json", vda5050::json(target).dump(2));
  }

  return table;
}

static vda5050::TypeSpecification fromTypeSpecificationTable(
    toml::node_view<const toml::node> node,
    std::optional<AGVDescriptionSubConfig::JSONFile> &file) {
  return fromJSONTable<vda5050::TypeSpecification>(node, file, "TypeSpecification");
}

static toml::table toTypeSpecificationTable(
    const vda5050::TypeSpecification &type_spec,
    const std::optional<AGVDescriptionSubConfig::JSONFile> &file) {
  return toJSONTable(type_spec, file, "TypeSpecification");
}

static vda5050::PhysicalParameters fromPhysicalParametersTable(
    toml::node_view<const toml::node> node,
    std::optional<AGVDescriptionSubConfig::JSONFile> &file) {
  return fromJSONTable<vda5050::PhysicalParameters>(node, file, "PhysicalParameters");
}

static toml::table toPhysicalParametersTable(
    const vda5050::PhysicalParameters &phys_par,
    const std::optional<AGVDescriptionSubConfig::JSONFile> &file) {
  return toJSONTable<vda5050::PhysicalParameters>(phys_par, file, "PhysicalParameters");
}

static vda5050::AgvGeometry fromAgvGeometryTable(
    toml::node_view<const toml::node> node,
    std::optional<AGVDescriptionSubConfig::JSONFile> &file) {
  return fromJSONTable<vda5050::AgvGeometry>(node, file, "AgvGeometry");
}

static toml::table toAgvGeometryTable(
    const vda5050::AgvGeometry &agv_geom,
    const std::optional<AGVDescriptionSubConfig::JSONFile> &file) {
  return toJSONTable<vda5050::AgvGeometry>(agv_geom, file, "AgvGeometry");
}

static vda5050::LoadSpecification fromLoadSpecificationTable(
    toml::node_view<const toml::node> node,
    std::optional<AGVDescriptionSubConfig::JSONFile> &file) {
  return fromJSONTable<vda5050::LoadSpecification>(node, file, "LoadSpecification");
}

static toml::table toLoadSpecificationTable(
    const vda5050::LoadSpecification &load_spec,
    const std::optional<AGVDescriptionSubConfig::JSONFile> &file) {
  return toJSONTable<vda5050::LoadSpecification>(load_spec, file, "LoadSpecification");
}

static vda5050::LocalizationParameters fromLocalizationParametersTable(
    toml::node_view<const toml::node> node,
    std::optional<AGVDescriptionSubConfig::JSONFile> &file) {
  return fromJSONTable<vda5050::LocalizationParameters>(node, file, "LocalizationParameters");
}

static toml::table toLocalizationParametersTable(
    const vda5050::LocalizationParameters &loc_par,
    const std::optional<AGVDescriptionSubConfig::JSONFile> &file) {
  return toJSONTable<vda5050::LocalizationParameters>(loc_par, file, "LocalizationParameters");
}

static vda5050pp::agv_description::SimpleProtocolLimits fromSimpleProtocolLimitsTable(
    toml::node_view<const toml::node> node) {
  vda5050pp::agv_description::SimpleProtocolLimits simple_protocol_limits;

  if (auto mil = node["max_id_len"].value<uint32_t>(); mil) {
    simple_protocol_limits.max_id_len = *mil;
  }
  if (auto ino = node["id_numerical_only"].value<bool>(); ino) {
    simple_protocol_limits.id_numerical_only = *ino;
  }
  if (auto mlil = node["max_load_id_len"].value<uint32_t>(); mlil) {
    simple_protocol_limits.max_load_id_len = *mlil;
  }
  if (auto ml = node["max_loads"].value<uint32_t>(); ml) {
    simple_protocol_limits.max_loads = *ml;
  }

  return simple_protocol_limits;
}

static toml::table toSimpleProtocolLimitsTable(
    const vda5050pp::agv_description::SimpleProtocolLimits &simple_protocol_limits) {
  toml::table table;

  if (simple_protocol_limits.max_id_len) {
    table.insert("max_id_len", *simple_protocol_limits.max_id_len);
  }
  if (simple_protocol_limits.id_numerical_only) {
    table.insert("id_numerical_only", *simple_protocol_limits.id_numerical_only);
  }
  if (simple_protocol_limits.max_load_id_len) {
    table.insert("max_load_id_len", *simple_protocol_limits.max_load_id_len);
  }
  if (simple_protocol_limits.max_id_len) {
    table.insert("max_id_len", *simple_protocol_limits.max_id_len);
  }

  return table;
}

void AGVDescriptionSubConfig::getFrom(const ConfigNode &node) {
  auto node_view = core::config::ConfigNode::upcast(node).get();

  this->agv_description_.agv_id = node_view["agv_id"].value_or<std::string_view>(""sv);
  this->agv_description_.manufacturer = node_view["manufacturer"].value_or<std::string_view>(""sv);
  this->agv_description_.serial_number =
      node_view["serial_number"].value_or<std::string_view>(""sv);
  this->agv_description_.description = node_view["description"].value<std::string_view>();

  if (auto bat = node_view["battery"]; bat) {
    this->agv_description_.battery = fromBatteryTable(bat);
  }
  if (auto ts = node_view["type_specification"]; ts) {
    this->agv_description_.type_specification =
        fromTypeSpecificationTable(ts, this->type_specification_mode_);
  }
  if (auto pp = node_view["physical_parameters"]; pp) {
    this->agv_description_.physical_parameters =
        fromPhysicalParametersTable(pp, this->physical_parameters_mode_);
  }
  if (auto ag = node_view["agv_geometry"]; ag) {
    this->agv_description_.agv_geometry = fromAgvGeometryTable(ag, this->agv_geometry_mode_);
  }
  if (auto ls = node_view["load_specification"]; ls) {
    this->agv_description_.load_specification =
        fromLoadSpecificationTable(ls, this->load_specifications_mode_);
  }
  if (auto lp = node_view["localization_parameters"]; lp) {
    this->agv_description_.localization_parameters =
        fromLocalizationParametersTable(lp, this->localization_parameters_mode_);
  }
  if (auto sp = node_view["simple_protocol_limits"]; sp) {
    this->agv_description_.simple_protocol_limits = fromSimpleProtocolLimitsTable(sp);
  }
}

void AGVDescriptionSubConfig::putTo(ConfigNode &node) const {
  auto node_view = core::config::ConfigNode::upcast(node).get();
  auto &table = *node_view.as_table();

  table.insert("agv_id", this->agv_description_.agv_id);
  table.insert("manufacturer", this->agv_description_.manufacturer);
  table.insert("serial_number", this->agv_description_.serial_number);
  if (this->agv_description_.description) {
    table.insert("description", *this->agv_description_.description);
  }

  table.insert("battery", toBatteryTable(this->agv_description_.battery));
  table.insert("type_specification",
               toTypeSpecificationTable(this->agv_description_.type_specification,
                                        this->type_specification_mode_));
  table.insert("physical_parameters",
               toPhysicalParametersTable(this->agv_description_.physical_parameters,
                                         this->physical_parameters_mode_));
  table.insert("agv_geometry",
               toAgvGeometryTable(this->agv_description_.agv_geometry, this->agv_geometry_mode_));
  table.insert("load_specification",
               toLoadSpecificationTable(this->agv_description_.load_specification,
                                        this->load_specifications_mode_));
  table.insert("localization_parameters",
               toLocalizationParametersTable(this->agv_description_.localization_parameters,
                                             this->localization_parameters_mode_));
  table.insert("simple_protocol_limits",
               toSimpleProtocolLimitsTable(this->agv_description_.simple_protocol_limits));
}

void AGVDescriptionSubConfig::setTypeSpecificationJSONMode(const std::optional<JSONFile> &mode) {
  this->type_specification_mode_ = mode;
}

void AGVDescriptionSubConfig::setPhysicalParametersJSONMode(const std::optional<JSONFile> &mode) {
  this->physical_parameters_mode_ = mode;
}

void AGVDescriptionSubConfig::setAGVGeometryJSONMode(const std::optional<JSONFile> &mode) {
  this->agv_geometry_mode_ = mode;
}

void AGVDescriptionSubConfig::setLoadSpecificationsJSONMode(const std::optional<JSONFile> &mode) {
  this->load_specifications_mode_ = mode;
}

void AGVDescriptionSubConfig::setLocalizationParametersJSONMode(
    const std::optional<JSONFile> &mode) {
  this->localization_parameters_mode_ = mode;
}

void AGVDescriptionSubConfig::setAGVDescription(
    const vda5050pp::agv_description::AGVDescription &new_description) {
  this->agv_description_ = new_description;
}

const vda5050pp::agv_description::AGVDescription &AGVDescriptionSubConfig::getAGVDescription()
    const {
  return this->agv_description_;
}

vda5050pp::agv_description::AGVDescription &AGVDescriptionSubConfig::refAGVDescription() {
  return this->agv_description_;
}