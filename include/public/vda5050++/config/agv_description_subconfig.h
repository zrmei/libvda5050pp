// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains the AGVDescriptionSubConfig.
//
//

#ifndef PUBLIC_VDA5050_2B_2B_CONFIG_AGV_DESCRIPTION_SUBCONFIG_H_
#define PUBLIC_VDA5050_2B_2B_CONFIG_AGV_DESCRIPTION_SUBCONFIG_H_

#include "vda5050++/agv_description/agv_description.h"
#include "vda5050++/config/types.h"

namespace vda5050pp::config {

class AGVDescriptionSubConfig : public vda5050pp::config::SubConfig {
public:
  struct JSONFile {
    ///\brief allow the file contents to be overwritten
    bool overwrite = false;
    ///\brief the json file path
    std::filesystem::path json_file;
  };

private:
  vda5050pp::agv_description::AGVDescription agv_description_;
  std::optional<JSONFile> type_specification_mode_;
  std::optional<JSONFile> physical_parameters_mode_;
  std::optional<JSONFile> agv_geometry_mode_;
  std::optional<JSONFile> load_specifications_mode_;
  std::optional<JSONFile> localization_parameters_mode_;

protected:
  ///\brief Restore this object from a ConstConfigNode
  ///\param node the config node
  void getFrom(const ConstConfigNode &node) override;

  ///\brief Dump this object to a ConfigNode
  ///\param node the config node
  void putTo(ConfigNode &node) const override;

public:
  ///
  ///\brief Overwrite the current AGVDescription.
  ///
  ///\param new_description the new description.
  ///
  void setAGVDescription(const vda5050pp::agv_description::AGVDescription &new_description);

  ///
  ///\brief Set the TypeSpecification JSON Serialization mode
  ///
  /// std::nullopt means serialize to string
  /// JSONFile means serialize to file and store filename
  ///
  ///\param mode the serialization mode
  ///
  void setTypeSpecificationJSONMode(const std::optional<JSONFile> &mode);

  ///
  ///\brief Set the PhysicalParameters JSON Serialization mode
  ///
  /// std::nullopt means serialize to string
  /// JSONFile means serialize to file and store filename
  ///
  ///\param mode the serialization mode
  ///
  void setPhysicalParametersJSONMode(const std::optional<JSONFile> &mode);

  ///
  ///\brief Set the AGVGeometry JSON Serialization mode
  ///
  /// std::nullopt means serialize to string
  /// JSONFile means serialize to file and store filename
  ///
  ///\param mode the serialization mode
  ///
  void setAGVGeometryJSONMode(const std::optional<JSONFile> &mode);

  ///
  ///\brief Set the LoadSpecifications JSON Serialization mode
  ///
  /// std::nullopt means serialize to string
  /// JSONFile means serialize to file and store filename
  ///
  ///\param mode the serialization mode
  ///
  void setLoadSpecificationsJSONMode(const std::optional<JSONFile> &mode);

  ///
  ///\brief Set the LocalizationParameters JSON Serialization mode
  ///
  /// std::nullopt means serialize to string
  /// JSONFile means serialize to file and store filename
  ///
  ///\param mode the serialization mode
  ///
  void setLocalizationParametersJSONMode(const std::optional<JSONFile> &mode);

  ///
  ///\brief Get the current AGVDescription.
  ///
  ///\return const vda5050pp::agv_description::AGVDescription&
  ///
  const vda5050pp::agv_description::AGVDescription &getAGVDescription() const;

  ///
  ///\brief Get a writable reference to the current AGVDescription.
  ///
  ///\return vda5050pp::agv_description::AGVDescription&
  ///
  vda5050pp::agv_description::AGVDescription &refAGVDescription();
};

}  // namespace vda5050pp::config

#endif  // PUBLIC_VDA5050_2B_2B_CONFIG_AGV_DESCRIPTION_SUBCONFIG_H_
