cmake_minimum_required(VERSION 3.16) # Adds instrumentation to all targets

if(LIBVDA5050PP_MAIN_PROJECT)
  set(library_target vda5050++)
else()
  project(libvda5050++_examples
    VERSION 0.0.1
    DESCRIPTION ""
    LANGUAGES CXX)

  find_package(libvda5050++ REQUIRED)
  set(library_target libvda5050++::vda5050++)
endif()

add_executable(handler_demo src/handler_demo.cpp)
target_link_libraries(handler_demo PUBLIC ${library_target})

add_executable(event_demo src/event_demo.cpp)
target_link_libraries(event_demo PUBLIC ${library_target})

add_executable(detailed_action_handler_demo src/detailed_action_handler_demo.cpp)
target_link_libraries(detailed_action_handler_demo PUBLIC ${library_target})

add_executable(full_config src/full_config.cpp)
target_link_libraries(full_config PUBLIC ${library_target})
