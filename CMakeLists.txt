cmake_minimum_required(VERSION 3.0.0)
project(test VERSION 0.1.0)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(test_lsd 
                main.cpp 
                src/lsd.cpp
                src/lsd.h src/LineSegmentDetector.cpp src/LineSegmentDetector.h)

target_link_libraries( test_lsd ${OpenCV_LIBS} )
target_include_directories(test_lsd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Copy the assets folder to the build directory
file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
