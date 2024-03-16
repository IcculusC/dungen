cmake_minimum_required(VERSION 3.19)
project(dungen LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/godot-cpp/cmake/")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/godot-cpp)

file(GLOB register_types CMAKE_CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/register_types.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/register_types.[hc]pp")

file(GLOB_RECURSE module_sources CMAKE_CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/core/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/*.[hc]pp"
    "${CMAKE_CURRENT_SOURCE_DIR}/editor/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/editor/*.[hc]pp"
    "${CMAKE_CURRENT_SOURCE_DIR}/editor/components/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/editor/components/*.[hc]pp")

list(APPEND SOURCES ${register_types} ${module_sources})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/demo/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/demo/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/demo/bin)

set(CMAKE_DEBUG_POSTFIX "d")

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_compile_definitions(${PROJECT_NAME} PUBLIC 
  $<$<CONFIG:Debug>:
    TOOLS_ENABLED
  >
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(system_bits 64)
else()
    set(system_bits 32)
endif()

string(TOLOWER
    "${PROJECT_NAME}.${CMAKE_SYSTEM_NAME}.${system_bits}.${CMAKE_BUILD_TYPE}"
    dungen_lib_name
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${dungen_lib_name}")

target_include_directories(${PROJECT_NAME}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/core"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/editor"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/editor/components")
target_link_libraries(${PROJECT_NAME} PUBLIC godot::cpp)

