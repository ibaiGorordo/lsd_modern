add_executable(test_gaussian_blur
        test_gaussian_blur.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src/lsd.cpp test_utils.cpp test_utils.h)

target_link_libraries(test_gaussian_blur PRIVATE ${OpenCV_LIBS}  ${lib_name})
target_include_directories(test_gaussian_blur PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src)

add_executable(test_gaussian_resize
        test_gaussian_resize.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src/lsd.cpp test_utils.cpp test_utils.h)

target_link_libraries(test_gaussian_resize PRIVATE ${OpenCV_LIBS}  ${lib_name})
target_include_directories(test_gaussian_resize PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src)

add_executable(test_angle_gradient
        test_angle_gradient.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src/lsd.cpp test_utils.cpp test_utils.h)

target_link_libraries(test_angle_gradient PRIVATE ${OpenCV_LIBS}  ${lib_name})
target_include_directories(test_angle_gradient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src)

add_executable(test_pixel_sorter
        test_pixel_sorter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src/lsd.cpp test_utils.cpp test_utils.h)

target_link_libraries(test_pixel_sorter PRIVATE ${OpenCV_LIBS}  ${lib_name})
target_include_directories(test_pixel_sorter PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src)

add_executable(test_region_detection
        test_region_detection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src/lsd.cpp test_utils.cpp test_utils.h)

target_link_libraries(test_region_detection PRIVATE ${OpenCV_LIBS}  ${lib_name})
target_include_directories(test_region_detection PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src)

add_executable(test_rect_nfa
        test_rect_nfa.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src/lsd.cpp test_utils.cpp test_utils.h)

target_link_libraries(test_rect_nfa PRIVATE ${OpenCV_LIBS}  ${lib_name})
target_include_directories(test_rect_nfa PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src)

add_executable(test_lsd
                test_lsd.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src/lsd.cpp test_utils.cpp test_utils.h)

target_link_libraries( test_lsd ${OpenCV_LIBS} ${lib_name})
target_include_directories(test_lsd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../pytlsd/src)




