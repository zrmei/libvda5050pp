find_package(Threads REQUIRED)

# include vda5050_message_structs ##################################################################
set(LIBVDA5050PP_VDA5050_MESSAGE_STRUCTS_VERSION "2.0.0" CACHE STRING "Overwrite vda5050_message_structs version")

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
set(LIBVDA5050PP_EVENTPP_VERSION "0.1.2" CACHE STRING "Overwrite eventpp version")
find_package(eventpp ${LIBVDA5050PP_EVENTPP_VERSION} QUIET)

if(eventpp_FOUND)
  message(STATUS "Found eventpp installation.")
else()
  set(eventpp_URL "https://github.com/wqking/eventpp.git"
    CACHE STRING "Overwrite the eventpp repo URL")
  cpmaddpackage(NAME eventpp
    GIT_REPOSITORY ${eventpp_URL}
    GIT_TAG v0.1.2-for-hunter-pm # This release contains fixes for -Wshadow
    EXCLUDE_FROM_ALL YES
    OPTIONS "EVENTPP_INSTALL ON"
  )
  export(EXPORT eventppTargets
    FILE "${eventpp_BINARY_DIR}/eventppTargets.cmake"
  )
endif()

# ##################################################################################################

# include fmt ######################################################################################
set(LIBVDA5050PP_FMT_VERSION "8.1.1" CACHE STRING "Overwrite fmt version")
find_package(fmt ${LIBVDA5050PP_FMT_VERSION} QUIET)

if(fmt_FOUND)
  message(STATUS "Found fmt installation.")
else()
  set(fmt_URL "https://github.com/fmtlib/fmt.git"
    CACHE STRING "Overwrite the fmt repo URL")
  cpmaddpackage(NAME fmt
    GIT_REPOSITORY ${fmt_URL}
    GIT_TAG ${LIBVDA5050PP_FMT_VERSION}
    EXCLUDE_FROM_ALL YES
    OPTIONS "FMT_INSTALL ON"
  )
  set_property(TARGET fmt PROPERTY COMPILE_FLAGS "-fpic")
  set(fmt_FOUND ON CACHE BOOL "")
endif()

# ##################################################################################################

# include spdlog ###################################################################################
set(LIBVDA5050PP_SPDLOG_VERSION "1.10.0" CACHE STRING "Overwrite spdlog version")
find_package(spdlog ${LIBVDA5050PP_SPDLOG_VERSION} QUIET)

if(spdlog_FOUND)
  message(STATUS "Found spdlog installation.")
else()
  set(spdlog_URL "https://github.com/gabime/spdlog.git"
    CACHE STRING "Overwrite the spdlog repo URL")
  cpmaddpackage(NAME spdlog
    GIT_REPOSITORY ${spdlog_URL}
    GIT_TAG "v${LIBVDA5050PP_SPDLOG_VERSION}"
    EXCLUDE_FROM_ALL YES
    OPTIONS "SPDLOG_FMT_EXTERNAL On" "SPDLOG_INSTALL On"
  )
  set_property(TARGET spdlog PROPERTY COMPILE_FLAGS "-fpic")
  export(EXPORT spdlog
    FILE "${spdlog_BINARY_DIR}/spdlogTargets.cmake"
  )
  set(spdlog_FOUND ON CACHE BOOL "")
endif()

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
find_package(PahoMqttCpp REQUIRED)

# ##################################################################################################