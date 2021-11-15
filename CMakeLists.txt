cmake_minimum_required(VERSION 3.5.0)
project(getdata VERSION 0.1.0)

set( CMAKE_BUILD_TYPE "Release" )
set( CMAKE_CXX_FLAGS "-std=c++11 -O3" )

include(CTest)
enable_testing()

######## packages ########
set(OpenCV_DIR "/home/jjj/CodeSrc/opencv-4.5.4/build")
find_package(OpenCV 4.0 QUIET)

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")