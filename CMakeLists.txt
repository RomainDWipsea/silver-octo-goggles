cmake_minimum_required(VERSION 3.9.0)

cmake_policy(SET CMP0048 NEW)

# ---[ Harmony version
set (Harmony_VERSION_MAJOR 2)
set (Harmony_VERSION_MINOR 0)
set (Harmony_VERSION_PATCH 0)

# ---[ Wipro project
project(Wipro VERSION ${Harmony_VERSION_MAJOR}.${Harmony_VERSION_MINOR}.${Harmony_VERSION_PATCH} DESCRIPTION "Wildlife Image Processing" LANGUAGES C CXX)

# ---[ Using cmake scripts and modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
include(cmake/Misc.cmake)
include(cmake/Summary.cmake)
#find_package(GetPrerequisites)
#include(cmake/GetPrerequisites.cmake)

# ---[ Dependencies
include(cmake/Dependencies.cmake)

# ---[ Build Flags
if(UNIX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=gnu++11") #added -DQT_COMPILING_QSTRING_COMPAT_CPP; more info at https://gcc.gnu.org/onlinedocs/gcc/Invoking-GCC.html#Invoking-GCC
endif()

# ---[ Configure generation
configure_file(cmake/Templates/wipro_config.h.in "${PROJECT_BINARY_DIR}/wipro_config.h")

# ---[ Includes
include_directories(${PROJECT_BINARY_DIR})

# ---[ Subdirectories
add_subdirectory(src/core)
add_subdirectory(src/gui)

# ---[ Configuration summary
wipro_print_configuration_summary()

set(CONFIGURED_ONCE TRUE CACHE INTERNAL
    "A flag showing that CMake has configured at least once.")
