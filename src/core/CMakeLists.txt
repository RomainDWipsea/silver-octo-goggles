# ---[ Sources
file(GLOB srcs *.cpp)
file(GLOB srcs_ext ext/*)
set_property(SOURCE  ext/jsoncpp.cpp ext/easylogging++.cc APPEND_STRING PROPERTY COMPILE_FLAGS " -w")

list(APPEND srcs ${srcs_ext})

add_library(core ${srcs})

# ---[ link libraries
target_link_libraries(core ${Wipro_LINKER_LIBS})

# ---[ include dirs
set(Core_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/core)
target_include_directories(core ${Wipro_INCLUDE_DIRS}
                                 PUBLIC
                                 $<BUILD_INTERFACE:${Core_INCLUDE_DIR}>
                                 $<INSTALL_INTERFACE:include/core>)

# ---[ definitions and properties
target_compile_definitions(core PUBLIC ${Wipro_DEFINITIONS})
set_target_properties(core PROPERTIES DEBUG_POSTFIX "d")

# ---[ Tests
enable_testing()
include(gmock)
#add_test(coreTest MissionTest)
#add_subdirectory(unitTest)

# ---[ Install
# install(TARGETS core DESTINATION lib)
# install(DIRECTORY ${Core_INCLUDE_DIR}  DESTINATION include/core)
