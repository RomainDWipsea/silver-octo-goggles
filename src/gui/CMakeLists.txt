# ---[ gui
file(GLOB_RECURSE gui_srcs ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(Harmony_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/gui)

add_executable(Harmony ${gui_srcs})

target_compile_definitions(Harmony PUBLIC ${Wipro_DEFINITIONS})

target_include_directories(Harmony ${Wipro_INCLUDE_DIRS} PUBLIC
  $<BUILD_INTERFACE:${Harmony_INCLUDE_DIR}>
  $<INSTALL_INTERFACE:include/gui>)

LINK_DIRECTORIES(${CMAKE_INSTALL_PREFIX}/lib)
target_link_libraries(Harmony ${Wipro_LINKER_LIBS})
target_link_libraries(Harmony PUBLIC core)
target_link_libraries(Harmony PUBLIC Qt5::Widgets)
if(USE_SERVER)
  target_link_libraries(Harmony PUBLIC server)
endif()

# ---[ Install
install(TARGETS Harmony DESTINATION bin)
install(DIRECTORY ${Harmony_INCLUDE_DIR} DESTINATION include/gui)
# set(Harmony_Prerequisite "")
# get_prerequisites(${PROJECT_BINARY_DIR}/src/gui/Harmony
#   ${Harmony_Prerequisite}
#   0
#   0
#   ${PROJECT_BINARY_DIR}/src/gui/Harmony
#   ${CMAKE_SOURCE_DIR}/lib)
get_prerequisites(${PROJECT_BINARY_DIR}/src/gui/Harmony DEPENDENCIES 0 0 "" "")
install(FILES ${DEPENDENCIES} DESTINATION bin)

