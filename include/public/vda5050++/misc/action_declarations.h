// Copyright Open Logistics Foundation
//
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
//
// This file contains the standard action declarations, as defined in the VDA5050
//
//

#ifndef PUBLIC_VDA5050_2B_2B_MISC_ACTION_DECLARATIONS_H_
#define PUBLIC_VDA5050_2B_2B_MISC_ACTION_DECLARATIONS_H_

#include "vda5050++/agv_description/action_declaration.h"

namespace vda5050pp::misc::action_parameter {

const vda5050pp::agv_description::ParameterRange k_reason{
    "reason",                                                  // key
    vda5050pp::agv_description::ParameterValueType::k_string,  // type
    "The reason for the logReport action",                     // Description
    std::nullopt,                                              // Ordinal Min
    std::nullopt,                                              // Ordinal Max
    std::nullopt,                                              // Allowed Value Set
};

const vda5050pp::agv_description::ParameterRange k_lhd{
    "lhd",                                                     // key
    vda5050pp::agv_description::ParameterValueType::k_string,  // type
    "The load handling device to use",                         // Description
    std::nullopt,                                              // Ordinal Min
    std::nullopt,                                              // Ordinal Max
    std::nullopt,                                              // Allowed Value Set
};

const vda5050pp::agv_description::ParameterRange k_station_type{
    "stationType",                                             // key
    vda5050pp::agv_description::ParameterValueType::k_string,  // type
    "The type of the load's station",                          // Description
    std::nullopt,                                              // Ordinal Min
    std::nullopt,                                              // Ordinal Max
    std::nullopt,                                              // Allowed Value Set
};

const vda5050pp::agv_description::ParameterRange k_station_name{
    "stationName",                                             // key
    vda5050pp::agv_description::ParameterValueType::k_string,  // type
    "The name of the load's station",                          // Description
    std::nullopt,                                              // Ordinal Min
    std::nullopt,                                              // Ordinal Max
    std::nullopt,                                              // Allowed Value Set
};

const vda5050pp::agv_description::ParameterRange k_load_type{
    "loadType",                                                // key
    vda5050pp::agv_description::ParameterValueType::k_string,  // type
    "The type of the load",                                    // Description
    std::nullopt,                                              // Ordinal Min
    std::nullopt,                                              // Ordinal Max
    std::nullopt,                                              // Allowed Value Set
};

const vda5050pp::agv_description::ParameterRange k_load_id{
    "loadId",                                                  // key
    vda5050pp::agv_description::ParameterValueType::k_string,  // type
    "The ID of the load",                                      // Description
    std::nullopt,                                              // Ordinal Min
    std::nullopt,                                              // Ordinal Max
    std::nullopt,                                              // Allowed Value Set
};

const vda5050pp::agv_description::ParameterRange k_height{
    "height",                                                 // key
    vda5050pp::agv_description::ParameterValueType::k_float,  // type
    "The distance from floor to the load's bottom height",    // Description
    std::nullopt,                                             // Ordinal Min
    std::nullopt,                                             // Ordinal Max
    std::nullopt,                                             // Allowed Value Set
};

const vda5050pp::agv_description::ParameterRange k_depth{
    "depth",                                                  // key
    vda5050pp::agv_description::ParameterValueType::k_float,  // type
    "The depth of the load (useful for forklifts)",           // Description
    std::nullopt,                                             // Ordinal Min
    std::nullopt,                                             // Ordinal Max
    std::nullopt,                                             // Allowed Value Set
};

const vda5050pp::agv_description::ParameterRange k_side{
    "side",                                                    // key
    vda5050pp::agv_description::ParameterValueType::k_string,  // type
    "The side on which the load is stored (e.g. conveyor)",    // Description
    std::nullopt,                                              // Ordinal Min
    std::nullopt,                                              // Ordinal Max
    std::nullopt,                                              // Allowed Value Set
};

const vda5050pp::agv_description::ParameterRange k_object_type{
    "objectType",                                              // key
    vda5050pp::agv_description::ParameterValueType::k_string,  // type
    "The type of the object to detect",                        // Description
    std::nullopt,                                              // Ordinal Min
    std::nullopt,                                              // Ordinal Max
    std::nullopt,                                              // Allowed Value Set
};

const vda5050pp::agv_description::ParameterRange k_trigger_type{
    "triggerType",                                             // key
    vda5050pp::agv_description::ParameterValueType::k_string,  // type
    "The type of the external trigger",                        // Description
    std::nullopt,                                              // Ordinal Min
    std::nullopt,                                              // Ordinal Max
    std::nullopt,                                              // Allowed Value Set
};

const vda5050pp::agv_description::ParameterRange k_x{
    "x",                                                      // key
    vda5050pp::agv_description::ParameterValueType::k_float,  // type
    "The x coordinate",                                       // Description
    std::nullopt,                                             // Ordinal Min
    std::nullopt,                                             // Ordinal Max
    std::nullopt,                                             // Allowed Value Set
};

const vda5050pp::agv_description::ParameterRange k_y{
    "y",                                                      // key
    vda5050pp::agv_description::ParameterValueType::k_float,  // type
    "The y coordinate",                                       // Description
    std::nullopt,                                             // Ordinal Min
    std::nullopt,                                             // Ordinal Max
    std::nullopt,                                             // Allowed Value Set
};

const vda5050pp::agv_description::ParameterRange k_theta{
    "theta",                                                  // key
    vda5050pp::agv_description::ParameterValueType::k_float,  // type
    "The angle",                                              // Description
    std::nullopt,                                             // Ordinal Min
    std::nullopt,                                             // Ordinal Max
    std::nullopt,                                             // Allowed Value Set
};

const vda5050pp::agv_description::ParameterRange k_map_id{
    "mapId",                                                   // key
    vda5050pp::agv_description::ParameterValueType::k_string,  // type
    "The map ID",                                              // Description
    std::nullopt,                                              // Ordinal Min
    std::nullopt,                                              // Ordinal Max
    std::nullopt,                                              // Allowed Value Set
};

const vda5050pp::agv_description::ParameterRange k_last_node_id{
    "lastNodeId",                                              // key
    vda5050pp::agv_description::ParameterValueType::k_string,  // type
    "The ID of the last node",                                 // Description
    std::nullopt,                                              // Ordinal Min
    std::nullopt,                                              // Ordinal Max
    std::nullopt,                                              // Allowed Value Set
};

}  // namespace vda5050pp::misc::action_parameter

namespace vda5050pp::misc::action_declarations {

const vda5050pp::agv_description::ActionDeclaration k_start_charging{
    "startCharging",                          // Type
    "Start the charging process of the AGV",  // Description
    std::nullopt,                             // Result Description
    {},                                       // Parameter
    {},                                       // Optional Parameter
    {vda5050::BlockingType::HARD},            // Allowed BlockingTypes
    true,                                     // Can be instant
    true,                                     // Can be on Node
    false,                                    // Can be on Edge
};

const vda5050pp::agv_description::ActionDeclaration k_stop_charging{
    "stopCharging",                          // Type
    "Stop the charging process of the AGV",  // Description
    std::nullopt,                            // Result Description
    {},                                      // Parameter
    {},                                      // Optional Parameter
    {vda5050::BlockingType::HARD},           // Allowed BlockingTypes
    true,                                    // Can be instant
    true,                                    // Can be on Node
    false,                                   // Can be on Edge
};

const vda5050pp::agv_description::ActionDeclaration k_log_report{
    "logReport",                                   // Type
    "Instruct the AGV to generate a log report.",  // Description
    std::nullopt,                                  // Result Description
    {action_parameter::k_reason},                  // Parameter
    {},                                            // Optional Parameter
    {vda5050::BlockingType::HARD},                 // Allowed BlockingTypes
    true,                                          // Can be instant
    false,                                         // Can be on Node
    false,                                         // Can be on Edge
};

const vda5050pp::agv_description::ActionDeclaration k_pick{
    "pick",                                 // Type
    "Instruct the AGV to pick up a load.",  // Description
    std::nullopt,                           // Result Description
    {
        action_parameter::k_station_type,
        action_parameter::k_load_type,
    },  // Parameter
    {
        action_parameter::k_lhd,
        action_parameter::k_station_name,
        action_parameter::k_load_id,
        action_parameter::k_height,
        action_parameter::k_depth,
        action_parameter::k_side,
    },                              // Optional Parameter
    {vda5050::BlockingType::HARD},  // Allowed BlockingTypes
    false,                          // Can be instant
    true,                           // Can be on Node
    true,                           // Can be on Edge
};

const vda5050pp::agv_description::ActionDeclaration k_drop{
    "drop",                              // Type
    "Instruct the AGV to drop a load.",  // Description
    std::nullopt,                        // Result Description
    {},                                  // Parameter
    {
        action_parameter::k_lhd,
        action_parameter::k_station_type,
        action_parameter::k_station_name,
        action_parameter::k_load_type,
        action_parameter::k_load_id,
        action_parameter::k_height,
        action_parameter::k_depth,
        action_parameter::k_side,
    },                              // Optional Parameter
    {vda5050::BlockingType::HARD},  // Allowed BlockingTypes
    false,                          // Can be instant
    true,                           // Can be on Node
    true,                           // Can be on Edge
};

const vda5050pp::agv_description::ActionDeclaration k_detect_object{
    "detectObject",                           // Type
    "Instruct the AGV to detect an object.",  // Description
    std::nullopt,                             // Result Description
    {},                                       // Parameter
    {action_parameter::k_object_type},        // Optional Parameter
    {
        vda5050::BlockingType::HARD,
        vda5050::BlockingType::SOFT,
        vda5050::BlockingType::NONE,
    },      // Allowed BlockingTypes
    false,  // Can be instant
    true,   // Can be on Node
    true,   // Can be on Edge
};

const vda5050pp::agv_description::ActionDeclaration k_fine_positioning{
    "finePositioning",         // Type
    "Fine position the AGV.",  // Description
    std::nullopt,              // Result Description
    {},                        // Parameter
    {
        action_parameter::k_station_type,
        action_parameter::k_station_name,
    },                              // Optional Parameter
    {vda5050::BlockingType::HARD},  // Allowed BlockingTypes
    false,                          // Can be instant
    true,                           // Can be on Node
    true,                           // Can be on Edge
};

const vda5050pp::agv_description::ActionDeclaration k_wait_for_trigger{
    "waitForTrigger",                                     // Type
    "Instruct the AGV to wait for an external trigger.",  // Description
    std::nullopt,                                         // Result Description
    {action_parameter::k_trigger_type},                   // Parameter
    {},                                                   // Optional Parameter
    {vda5050::BlockingType::HARD},                        // Allowed BlockingTypes
    false,                                                // Can be instant
    true,                                                 // Can be on Node
    true,                                                 // Can be on Edge
};

const vda5050pp::agv_description::ActionDeclaration k_init_position{
    "initPosition",                                          // Type
    "Reset the pose of the AGV with the given parameters.",  // Description
    std::nullopt,                                            // Result Description
    {
        action_parameter::k_x,
        action_parameter::k_y,
        action_parameter::k_theta,
        action_parameter::k_map_id,
        action_parameter::k_last_node_id,
    },                              // Parameter
    {},                             // Optional Parameter
    {vda5050::BlockingType::HARD},  // Allowed BlockingTypes
    true,                           // Can be instant
    true,                           // Can be on Node
    false,                          // Can be on Edge
};

}  // namespace vda5050pp::misc::action_declarations

#endif  // PUBLIC_VDA5050_2B_2B_MISC_ACTION_DECLARATIONS_H_
