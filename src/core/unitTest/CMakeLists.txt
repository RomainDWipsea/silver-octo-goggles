add_gmock_test(MissionTest observationTest.cpp missionTest.cpp itemTest.cpp)
target_link_libraries(MissionTest wildlifeCounterCMDLib)
target_link_libraries(MissionTest easyLogging)

if(USE_ALGO)
  target_link_libraries(MissionTest ${Caffe_LIBRARIES})
endif()