cmake_minimum_required(VERSION 3.0.0)
project(test VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

option(BUILD_TEST "Build the test subdirectory" ON)

add_executable(test_lsd 
                main.cpp
                pytlsd/src/lsd.cpp
                pytlsd/src/lsd.h
                src/LineSegmentDetector.cpp
                src/LineSegmentDetector.h
                src/Segment.h
                src/GaussianDownsampler.cpp
                src/GaussianDownsampler.h)

target_link_libraries( test_lsd ${OpenCV_LIBS} )
target_include_directories(test_lsd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(test_lsd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/pytlsd/src)

if (BUILD_TEST)
    add_subdirectory(test)
endif ()

# Copy the assets folder to the build directory
file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
