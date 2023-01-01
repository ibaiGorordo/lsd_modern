add_executable(test_gaussian_blur
        test_gaussian_blur.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src/lsd.cpp)

target_link_libraries(test_gaussian_blur PRIVATE ${OpenCV_LIBS}  ${lib_name})
target_include_directories(test_gaussian_blur PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src)

add_executable(test_gaussian_resize
        test_gaussian_downsampler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src/lsd.cpp)

target_link_libraries(test_gaussian_resize PRIVATE ${OpenCV_LIBS}  ${lib_name})
target_include_directories(test_gaussian_resize PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src)
#
#add_executable(test_angle_gradient
#        test_angle_gradient.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src/lsd.cpp)
#
#target_link_libraries(test_angle_gradient PRIVATE ${OpenCV_LIBS}  ${lib_name})
#target_include_directories(test_angle_gradient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src)

add_executable(test_lsd
                test_lsd.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src/lsd.cpp)

target_link_libraries( test_lsd ${OpenCV_LIBS} ${lib_name})
target_include_directories(test_lsd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src)




