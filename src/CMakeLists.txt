# Copyright Open Logistics Foundation
# 
# Licensed under the Open Logistics Foundation License 1.3.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: OLFL-1.3
# 

option(LIBVDA5050PP_BUILD_STATIC "libVDA5050++: build static library (instead of dynamic)" OFF)

set(LIBVDA5050PP_SOURCES
  ${PROJECT_SOURCE_DIR}/src/vda5050++/config.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/config/agv_description_subconfig.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/config/global_config.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/config/logging_subconfig.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/config/module_subconfig.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/config/mqtt_subconfig.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/config/mqtt_subconfig.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/config/node_reached_subconfig.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/config/query_event_handler_subconfig.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/config/state_update_timer_subconfig.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/config/types.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/config/visualization_timer_subconfig.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/action_event_manager.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/action_status_manager.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/agv_handler/action_event_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/agv_handler/action_state.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/agv_handler/functional.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/agv_handler/navigation_event_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/agv_handler/node_reached_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/agv_handler/query_event_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/checks/action.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/checks/header.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/checks/order.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/common/conversion.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/common/exception.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/common/type_traits.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/config.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/events/event_control_blocks.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/factsheet/factsheet_event_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/factsheet/gather.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/instance.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/interpreter/control_instant_actions.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/interpreter/functional.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/interpreter/interpreter_event_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/logger.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/messages/message_event_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/messages/mqtt_module.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/navigation_event_manager.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/navigation_status_manager.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/order/action_task.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/order/navigation_task.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/order/order_event_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/order/scheduler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/query_event_manager.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/state/graph.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/state/order_manager.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/state/state_event_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/state/state_update_timer.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/state/state_update_urgency.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/state/status_manager.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/state/visualization_timer.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/status_event_manager.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/core/validation/validation_event_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/events/event_handle.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/events/synchronized_event.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/exception.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/handle.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/handler/action_state.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/handler/base_navigation_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/handler/base_query_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/handler/init_position_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/handler/simple_action_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/handler/simple_multi_action_handler.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/misc/action_parameter_view.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/sinks/navigation_sink.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/sinks/status_sink.cpp
  ${PROJECT_SOURCE_DIR}/src/vda5050++/version.cpp
)

if(${LIBVDA5050PP_BUILD_STATIC})
  # The main libvda5050++.a
  add_library(vda5050++ STATIC
    ${LIBVDA5050PP_SOURCES}
  )
else()
  # The main libvda5050++.so
  add_library(vda5050++ SHARED
    ${LIBVDA5050PP_SOURCES}
  )
endif()

set_target_properties(vda5050++ PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION 0
)

target_link_libraries(vda5050++
  PUBLIC
  $<IF:$<BOOL:${LIBVDA5050PP_BUILD_STATIC}>,vda5050_message_structs-static,vda5050_message_structs>
  Threads::Threads

  PRIVATE

  # dynamic
  PahoMqttCpp::paho-mqttpp3

  # static
  $<BUILD_INTERFACE:spdlog::spdlog>
  $<BUILD_INTERFACE:eventpp::eventpp>
  $<BUILD_INTERFACE:tomlplusplus::tomlplusplus>
)
target_include_directories(vda5050++
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/public>
  $<INSTALL_INTERFACE:include>

  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/private>
)
target_compile_features(vda5050++ PUBLIC cxx_std_17)
target_compile_options(vda5050++ PRIVATE ${LIBVDA5050PP_CMAKE_CXX_FLAGS})
target_compile_definitions(vda5050++ PRIVATE
  LIBVDA5050PP_VERSION=${PROJECT_VERSION}
  ${LIBVDA5050PP_PLATFORM_DEFINITIONS}
)

# ###############################################################################

# This leads to problems with ROS
# target_compile_options(vda5050++ PUBLIC "$<$<CONFIG:DEBUG>:-D_GLIBCXX_DEBUG>")