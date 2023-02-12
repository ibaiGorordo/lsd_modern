cmake_minimum_required(VERSION 3.0.0)
project(test VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

option(BUILD_TEST "Build the test subdirectory" ON)

set(lib_name "lsd_modern")

add_library(${lib_name}
            src/LineSegmentDetector.cpp
            src/GaussianDownsampler.cpp
            src/GradientCalculator.cpp
            src/GradientCalculator.h
            src/RegionDetector.cpp
            src/RegionDetector.h
            src/utils.cpp
            src/utils.h)

target_include_directories(${lib_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if (BUILD_TEST)
    find_package( OpenCV REQUIRED )
    include_directories( ${OpenCV_INCLUDE_DIRS} )
    add_subdirectory(test)

    # Copy the assets folder to the build directory
    file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test)
endif ()


