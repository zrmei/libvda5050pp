# Copyright Open Logistics Foundation
# 
# Licensed under the Open Logistics Foundation License 1.3.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: OLFL-1.3
# 

message(STATUS "Running libVDA5050++ PostInstall:")

function(rmdir dir)
  file(GLOB dir_to_rm_list "${dir}/*")

  if("${dir_to_rm_list}" STREQUAL "")
    file(REMOVE_RECURSE "${dir}")
    message(STATUS "PostInstall - CleanUpDir: ${dir}")
    get_filename_component(parent_dir ${dir} DIRECTORY)
    rmdir(${parent_dir})
  endif()
endfunction()

foreach(file ${CMAKE_INSTALL_MANIFEST_FILES})
  if(${file} MATCHES "toml\\+\\+|tomlplusplus|meson\\.build|eventpp|nlohmann")
    if(LIBVDA5050PP_BUILD_DEB)
      set(true_file "${CPACK_PACKAGE_DIRECTORY}/_CPack_Packages/${CPACK_SYSTEM_NAME}/DEB/${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}${file}")
    else()
      set(true_file ${file})
    endif()

    message(STATUS "PostInstall - CleanUp: ${true_file}")
    get_filename_component(parent_dir ${true_file} DIRECTORY)
    file(REMOVE ${true_file})
    rmdir(${parent_dir})
  endif()
endforeach()
