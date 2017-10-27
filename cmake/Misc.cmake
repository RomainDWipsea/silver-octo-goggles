# ---[ Configuration types
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Possible configurations" FORCE)
mark_as_advanced(CMAKE_CONFIGURATION_TYPES)

if(DEFINED CMAKE_BUILD_TYPE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES})
endif()

# --[ If user doesn't specify build type then assume release
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
  set(CMAKE_BUILD_TYPE Release)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_COMPILER_IS_CLANGXX TRUE)
endif()

# ---[ Install options
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/install" CACHE PATH "Default install path" FORCE)
endif()

# ---[ Set debug postfix
set(Harmony_DEBUG_POSTFIX "-d")

set(Harmony_POSTFIX "")
if(CMAKE_BUILD_TYPE MATCHES "Debug")
  set(Harmony_POSTFIX ${Harmony_DEBUG_POSTFIX})
endif()