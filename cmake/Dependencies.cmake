 # These lists are later turned into target properties on main Wipro library target
set(Wipro_LINKER_LIBS "")
set(Wipro_INCLUDE_DIRS "")
set(Wipro_DEFINITIONS "")
set(Wipro_COMPILE_OPTIONS "")

# adding specific path to external libraries
if(UNIX)
  set(CMAKE_PREFIX_PATH "./lib/Qt5.4.1/5.4/gcc_64/")
  set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/lib/boost_1_56_0")
elseif(WIN32)
  message("Here enter the path to boost windows root")
  set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/lib/boost_1_56_0")
endif()

# ---[ Qt5.4
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Declarative)

# ---[ Exiv2
find_package(Exiv2 MODULE REQUIRED)
if(EXIV2_FOUND)
  list(APPEND Wipro_INCLUDE_DIRS PUBLIC ${EXIV2_INCLUDE_DIR})
  list(APPEND Wipro_LINKER_LIBS PUBLIC ${EXIV2_LIBRARIES})
  list(APPEND Wipro_DEFINITIONS PUBLIC ${EXIV2_DEFINITIONS})
endif()

# ---[ Boost
find_package(Boost 1.56 REQUIRED COMPONENTS random system thread filesystem chrono atomic date_time regex)
list(APPEND Wipro_INCLUDE_DIRS PUBLIC ${Boost_INCLUDE_DIRS})
list(APPEND Wipro_LINKER_LIBS PUBLIC ${Boost_LIBRARIES})

# ---[ OpenCV
find_package(OpenCV QUIET COMPONENTS core highgui imgproc imgcodecs)
if(NOT OpenCV_FOUND) # if not OpenCV 3.x, then imgcodecs are not found
  find_package(OpenCV REQUIRED COMPONENTS core highgui imgproc)
endif()
message("opencv include dir is :  ${OpenCV_INCLUDE_DIRS}")
list(APPEND Wipro_INCLUDE_DIRS PUBLIC ${OpenCV_INCLUDE_DIRS})
list(APPEND Wipro_LINKER_LIBS PUBLIC ${OpenCV_LIBS})

# ---[ easyLogging++ and json
list(APPEND Wipro_DEFINITIONS PUBLI -DELPP_FEATURE_PERFORMANCE_TRACKING -DELPP_NO_DEFAULT_LOG_FILE -DELPP_FEATURE_CRASH_LOG)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  list(APPEND Wipro_DEFINITIONS PUBLIC -DELPP_DISABLE_TRACE_LOGS -DELPP_DISABLE_DEBUG_LOGS -DELPP_DISABLE_PERFORMANCE_TRACKING)
endif()
include_directories(SYSTEM PUBLIC ${PROJECT_SOURCE_DIR}/include/ext)
 
