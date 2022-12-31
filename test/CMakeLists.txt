add_executable(test_gaussian_down
                test_gaussian_downsampler.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src/lsd.cpp)

target_link_libraries(test_gaussian_down PRIVATE ${OpenCV_LIBS}  ${lib_name})
target_include_directories(test_gaussian_down PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src)


add_executable(test_lsd
                test_lsd.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src/lsd.cpp)

target_link_libraries( test_lsd ${OpenCV_LIBS} ${lib_name})
target_include_directories(test_lsd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src)




