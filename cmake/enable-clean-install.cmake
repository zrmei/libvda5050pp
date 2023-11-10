# Copyright Open Logistics Foundation
# 
# Licensed under the Open Logistics Foundation License 1.3.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: OLFL-1.3
# 

# cleanup the installation
if(LIBVDA5050PP_CLEAN_INSTALL)
  message(STATUS "Enabling PostInstall CleanUp.")
  install(CODE "set(CPACK_PACKAGE_DIRECTORY \"${CPACK_PACKAGE_DIRECTORY}\")")
  install(CODE "set(CPACK_PACKAGE_FILE_NAME \"${CPACK_PACKAGE_FILE_NAME}\")")
  install(CODE "set(CPACK_SYSTEM_NAME \"${CPACK_SYSTEM_NAME}\")")
  install(CODE "set(CPACK_PACKAGE_NAME \"${CPACK_PACKAGE_NAME}\")")
  install(CODE "set(CPACK_PACKAGE_VERSION \"${CPACK_PACKAGE_VERSION}\")")
  install(CODE "set(LIBVDA5050PP_BUILD_DEB ${LIBVDA5050PP_BUILD_DEB})")
  install(SCRIPT ${CMAKE_SOURCE_DIR}/cmake/PostInstall.cmake)
endif()