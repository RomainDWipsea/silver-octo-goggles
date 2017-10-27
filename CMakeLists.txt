cmake_minimum_required(VERSION 3.7.11)

cmake_policy(SET CMP0048 NEW)

# ---[ Harmony version
set (Harmony_VERSION_MAJOR 2)
set (Harmony_VERSION_MINOR 0)
set (Harmony_VERSION_PATCH 0)

# ---[ Wipro project
project(Wipro VERSION ${Harmony_VERSION_MAJOR}.${Harmony_VERSION_MINOR}.${Harmony_VERSION_PATCH} LANGUAGES C CXX)

# ---[ Using cmake scripts and modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
include(ucm)
include(cmake/Misc.cmake)
include(cmake/Summary.cmake)

# adding specific path to external libraries
if(UNIX)
  set(CMAKE_PREFIX_PATH "./lib/Qt5.4.1/5.4/gcc_64/")
elseif(WIN32)
  set(CMAKE_PREFIX_PATH "C:/Qt/5.4/msvc2013_64/") #to be defined
  ucm_print_flags()
  ucm_set_runtime(DYNAMIC)
  ucm_print_flags()
  list(APPEND CMAKE_PREFIX_PATH C:/Users/romain/Wipsea/logiciels/opencv300/opencv/build/)
endif()

# ---[ Dependencies
include(cmake/Dependencies.cmake)

# ---[ Flags
if(UNIX)
#    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -std=gnu++11 -DQT_COMPILING_QSTRING_COMPAT_CPP") #added qstring_compat for linux executable
     SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=gnu++11") #added qstring_compat for linux executable
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
