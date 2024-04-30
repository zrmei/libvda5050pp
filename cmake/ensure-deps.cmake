# Copyright Open Logistics Foundation
# 
# Licensed under the Open Logistics Foundation License 1.3.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: OLFL-1.3
# 

find_package(Threads REQUIRED)

# include vda5050_message_structs ##################################################################
set(LIBVDA5050PP_VDA5050_MESSAGE_STRUCTS_VERSION "2.0.0.6" CACHE STRING "Overwrite vda5050_message_structs version")

if(NOT LIBVDA5050PP_VDA5050_MESSAGE_STRUCTS_DEP_LOCAL)
  find_package(vda5050_message_structs ${LIBVDA5050PP_VDA5050_MESSAGE_STRUCTS_VERSION} QUIET)
endif()

if(vda5050_message_structs_FOUND)
  message(STATUS "Found vda5050_message_structs installation.")
else()
  set(VDA5050_MESSAGE_STRUCTS_URL "https://git.openlogisticsfoundation.org/silicon-economy/libraries/vda5050/vda5050_message_structs.git"
    CACHE STRING "Overwrite the vda5050_message_structs repo URL")
  cpmaddpackage(NAME vda5050_message_structs
    GIT_REPOSITORY ${VDA5050_MESSAGE_STRUCTS_URL}
    VERSION ${LIBVDA5050PP_VDA5050_MESSAGE_STRUCTS_VERSION}
    EXCLUDE_FROM_ALL NO
  )
endif()

# ##################################################################################################

# include eventpp ##################################################################################
set(LIBVDA5050PP_EVENTPP_VERSION "0.1.3" CACHE STRING "Overwrite eventpp version")
find_package(eventpp ${LIBVDA5050PP_EVENTPP_VERSION} QUIET)

if(eventpp_FOUND)
  message(STATUS "Found eventpp installation.")
else()
  set(eventpp_URL "https://github.com/wqking/eventpp.git"
    CACHE STRING "Overwrite the eventpp repo URL")
  cpmaddpackage(NAME eventpp
    GIT_REPOSITORY ${eventpp_URL}
    VERSION ${LIBVDA5050PP_EVENTPP_VERSION}
    EXCLUDE_FROM_ALL YES
    OPTIONS "EVENTPP_INSTALL ON"
    SYSTEM YES
  )
  export(EXPORT eventppTargets
    FILE "${eventpp_BINARY_DIR}/eventppTargets.cmake"
  )
endif()

# ##################################################################################################

# include spdlog ###################################################################################
set(LIBVDA5050PP_SPDLOG_VERSION "1.10.0" CACHE STRING "Overwrite spdlog version")

# Force git version
set(spdlog_URL "https://github.com/gabime/spdlog.git"
  CACHE STRING "Overwrite the spdlog repo URL")
cpmaddpackage(NAME spdlog
  GIT_REPOSITORY ${spdlog_URL}
  GIT_TAG "v${LIBVDA5050PP_SPDLOG_VERSION}"
  EXCLUDE_FROM_ALL YES
  OPTIONS "SPDLOG_FMT_EXTERNAL_HO OFF" "SPDLOG_INSTALL On"
  SYSTEM YES
)
set_property(TARGET spdlog PROPERTY COMPILE_FLAGS "-fpic")
export(EXPORT spdlog
  FILE "${spdlog_BINARY_DIR}/spdlogTargets.cmake"
)
set(spdlog_FOUND ON CACHE BOOL "")

# ##################################################################################################

# include tomlplusplus #############################################################################
set(LIBVDA5050PP_TOMLPLUSPLUS_VERSION "3.3.0" CACHE STRING "Overwrite tomlplusplus version")
find_package(tomlplusplus ${LIBVDA5050PP_TOMLPLUSPLUS_VERSION} QUIET)

if(tomlplusplus_FOUND)
  message(STATUS "Found tomlplusplus installation.")
else()
  set(tomlplusplus_URL "https://github.com/marzer/tomlplusplus.git"
    CACHE STRING "Overwrite the tomlplusplus repo URL")
  cpmaddpackage(NAME tomlplusplus
    GIT_REPOSITORY ${tomlplusplus_URL}
    GIT_TAG "v${LIBVDA5050PP_TOMLPLUSPLUS_VERSION}"
    EXCLUDE_FROM_ALL YES
    SYSTEM YES
  )
  set(OLD_PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR})
  set(PROJECT_SOURCE_DIR ${tomlplusplus_SOURCE_DIR})
  include(${tomlplusplus_SOURCE_DIR}/cmake/install-rules.cmake)
  export(EXPORT tomlplusplusTargets
    FILE "${tomlplusplus_BINARY_DIR}/tomlplusplusTargets.cmake"
  )
  set(PROJECT_SOURCE_DIR ${OLD_PROJECT_SOURCE_DIR})
  set(tomlplusplus_FOUND ON CACHE BOOL "")
endif()

# ##################################################################################################

# include paho #####################################################################################
set(LIBVDA5050PP_PAHO_MQTT_CPP_VERSION "1.3.2" CACHE STRING "Overwrite paho-mqtt-cpp version")
find_package(paho-mqtt-cpp ${LIBVDA5050PP_PAHO_MQTT_CPP_VERSION} QUIET)
if(PahoMqttCpp_FOUND)
  message(STATUS "Found paho-mqtt-cpp installation.")
else()
  set(paho_mqtt_cpp_URL "https://github.com/eclipse/paho.mqtt.cpp.git"
    CACHE STRING "Overwrite the paho-mqtt-cpp repo URL")
  cpmaddpackage(NAME paho-mqtt-cpp
    GIT_REPOSITORY ${paho_mqtt_cpp_URL}
    GIT_SUBMODULES_RECURSE YES
    GIT_TAG v${LIBVDA5050PP_PAHO_MQTT_CPP_VERSION}
    EXCLUDE_FROM_ALL YES
    OPTIONS "PAHO_WITH_MQTT_C ON" 
            "PAHO_WITH_SSL ON"
            "PAHO_BUILD_STATIC ON"
            "PAHO_BUILD_SHARED OFF"
            "PAHO_ENABLE_TESTING OFF"
            "PAHO_HIGH_PERFORMANCE ON"
    SYSTEM YES
    # This is currently required, see below
    PATCH_COMMAND git apply --reject ${CMAKE_CURRENT_SOURCE_DIR}/patches/0001-build-FindPahoMqttC-use-static-suffix.patch
  )
  # The patch is needed, since paho currently searches the static paho-c-lib on non WIN32 systems without the
  # "static" suffix, such that the shared library must be available and will be linked, see:
  # https://github.com/eclipse/paho.mqtt.cpp/blob/5e0d1bf37b4826d680ec066ec42afd133851a681/cmake/FindPahoMqttC.cmake#L10
  # Related issue: https://github.com/eclipse/paho.mqtt.cpp/issues/469
endif()

# ##################################################################################################