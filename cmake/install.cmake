# Copyright Open Logistics Foundation
# 
# Licensed under the Open Logistics Foundation License 1.3.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: OLFL-1.3
# 

message(STATUS "Generating libVDA5050++ install targets")

# # Install part ################################################################
# gather install targets
install(TARGETS vda5050++ EXPORT libvda5050++Targets)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/public/vda5050++ DESTINATION include)

# make targets available
install(EXPORT libvda5050++Targets
  FILE libvda5050++BaseTargets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION lib/cmake/${PROJECT_NAME}
)

# make targets available, if this is a subdirectory
export(EXPORT libvda5050++Targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/libvda5050++BaseTargets.cmake"
)

# generate the config file that is includes the exports
include(CMakePackageConfigHelpers)
configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/libvda5050++Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/libvda5050++Config.cmake"
  INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# generate the version matching file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/libvda5050++ConfigVersion.cmake"
  VERSION "${libvda5050++_VERSION_MAJOR}.${libvda5050++_VERSION_MINOR}"
  COMPATIBILITY AnyNewerVersion
)

# install helper files
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/libvda5050++Config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/libvda5050++ConfigVersion.cmake
  DESTINATION lib/cmake/${PROJECT_NAME}
)

# ##################################################################################################