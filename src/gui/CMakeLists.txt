# ---[ gui
file(GLOB_RECURSE gui_srcs ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(Harmony_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/gui)

add_executable(Harmony ${gui_srcs})

target_compile_definitions(Harmony PUBLIC ${Wipro_DEFINITIONS})

target_include_directories(Harmony ${Wipro_INCLUDE_DIRS} PUBLIC
  $<BUILD_INTERFACE:${Harmony_INCLUDE_DIR}>
  $<INSTALL_INTERFACE:include>)

target_link_libraries(Harmony ${Wipro_LINKER_LIBS})
target_link_libraries(Harmony PUBLIC core)
target_link_libraries(Harmony PUBLIC Qt5::Widgets)
if(USE_SERVER)
  target_link_libraries(Harmony PUBLIC server)
endif()

# ---[ Install
install(TARGETS Harmony DESTINATION bin)
install(DIRECTORY ${Harmony_INCLUDE_DIR} DESTINATION include)
