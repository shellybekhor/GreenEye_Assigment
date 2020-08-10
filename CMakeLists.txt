cmake_minimum_required(VERSION 2.8)
project( GrayImages )
find_package( OpenCV REQUIRED )
find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( GrayImages GrayImages.cpp )
target_link_libraries( GrayImages ${OpenCV_LIBS} )
target_link_libraries( GrayImages 
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
