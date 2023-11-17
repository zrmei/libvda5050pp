# Copyright Open Logistics Foundation
# 
# Licensed under the Open Logistics Foundation License 1.3.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: OLFL-1.3
# 

find_package(Threads REQUIRED)

set(LIBVDA5050PP_CATCH2_VERSION "2.13.10" CACHE STRING "Overwrite catch2 version")
find_package(Catch2 ${LIBVDA5050PP_CATCH2_VERSION} QUIET)

if(Catch2_FOUND)
  message(STATUS "Found Catch2 installation.")
else()
  CPMAddPackage(
    NAME Catch2
    GITHUB_REPOSITORY catchorg/Catch2
    VERSION ${LIBVDA5050PP_CATCH2_VERSION}
  )
endif()

if(Catch2_SOURCE_DIR)
  list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/contrib)
endif()

include(Catch)

add_executable(vda5050++_test
  ${PROJECT_SOURCE_DIR}/test/main.cpp
  ${PROJECT_SOURCE_DIR}/test/src/data.cpp
  ${PROJECT_SOURCE_DIR}/test/src/test_action_handler.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/common/any_ptr.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/config.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/action_event_manager.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/agv_handler/action_event_handler.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/agv_handler/functional.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/agv_handler/navigation_event_handler.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/agv_handler/node_reached_handler.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/agv_handler/query_event_handler.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/checks/action.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/checks/header.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/checks/order.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/common/blocking_queue.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/common/conversion.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/common/exception.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/common/formatters.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/common/interruptable_timer.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/common/math/geometry.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/common/math/linear_path_length_calculator.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/common/scoped_thread.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/common/semaphore.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/events/event_control_blocks.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/factsheet/gather.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/generic_event_manager.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/handler/action_event_handler.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/handler/action_state.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/interpreter/functional.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/messages/message_event_handler.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/order/action_task.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/order/navigation_task.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/order/scheduler.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/state/graph.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/state/state_event_handler.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/core/validation/validation_event_handler.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/event_behaviour/interpreter_event_behaviour.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/event_behaviour/status_event_behaviour.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/events/event_handle.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/events/synchronized_event.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/handle.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/handler/base_navigation_handler.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/misc/action_parameter_view.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/sinks/navigation_sink.cpp
  ${PROJECT_SOURCE_DIR}/test/vda5050++/sinks/status_sink.cpp
)
target_link_libraries(vda5050++_test
  Catch2::Catch2
  vda5050++
  Threads::Threads
  spdlog::spdlog
  eventpp::eventpp
  $<BUILD_INTERFACE:tomlplusplus::tomlplusplus>
)

target_include_directories(vda5050++_test
  PRIVATE
  ${PROJECT_SOURCE_DIR}/test/include
  ${PROJECT_SOURCE_DIR}/include/private
)

# Let CTest discover the Catch2 test cases
catch_discover_tests(vda5050++_test)
