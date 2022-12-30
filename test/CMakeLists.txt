add_executable(test_gaussian_down
                test_gaussian_downsampler.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/../src/GaussianDownsampler.cpp)
target_link_libraries(test_gaussian_down PRIVATE ${OpenCV_LIBS})
target_include_directories(test_gaussian_down PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)



