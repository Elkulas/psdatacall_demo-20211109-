cmake_minimum_required(VERSION 3.5.0)
project(getdata VERSION 0.1.0)

set( CMAKE_BUILD_TYPE "Release" )
set( CMAKE_CXX_FLAGS "-std=c++11 -O3" )

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include(CTest)
enable_testing()

######## packages ########
set(OpenCV_DIR "/home/jjj/CodeSrc/opencv-3.4.16/build")
find_package(OpenCV 3.0 QUIET)

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

######## include #########
include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/HCNetSDKCom
  ${OpenCV_INCLUDE_DIRS} 
)

link_directories(
  ${PROJECT_SOURCE_DIR}/HCNetSDKCom
  ${PROJECT_SOURCE_DIR}
)

######## lib ########
# add_library(${PROJECT_NAME} SHARED
#   HCNetSDKCom/libanalyzedata.so
# )

######## executable #######
add_executable(getsdata 
  main.cpp
)

target_link_libraries( getsdata
  ${OpenCV_LIBS}
  libanalyzedata.so
  libAudioIntercom.so
  libHCAlarm.so
  libHCCoreDevCfg.so
  libHCDisplay.so
  libHCGeneralCfgMgr.so
  libHCIndustry.so
  libHCPlayBack.so
  libHCPreview.so
  libHCVoiceTalk.so
  libiconv2.so
  libStreamTransClient.so
  libSystemTransform.so
  libAudioRender.so
  libcrypto.so
  libcrypto.so
  libHCCore.so
  libhcnetsdk.so
  libhpr.so
  libNPQos.so
  # openal
  libPlayCtrl.so
  libssl.so
  libSuperRender.so
  libz.so
)



