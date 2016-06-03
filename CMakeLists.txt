cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(slimmer CXX)
set (APP_VERSION_NUMBER 1.0)

#################################################################################
# Libraries
set(LIBS ${LIBS} /usr/local/lib)

#################################################################################
# Configuration files
string(TIMESTAMP BUILD_DATE UTC)

configure_file(
  "${PROJECT_SOURCE_DIR}/ApplicationInfo.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/ApplicationInfo.h"
  )
configure_file(
  "${PROJECT_SOURCE_DIR}/BuildInfo.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/BuildInfo.h"
  )

#################################################################################
# Executable
file(GLOB_RECURSE SRC_LIST
        "*.h"
        "*.cpp"
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/libev)
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries (${PROJECT_NAME} lcdapi curl icuuc)

add_definitions(-flto -fuse-linker-plugin -pipe)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

#################################################################################
# Clean
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "ApplicationInfo.h;BuildInfo.h")
