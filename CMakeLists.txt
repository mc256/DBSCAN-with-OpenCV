cmake_minimum_required(VERSION 3.10)
project(DBSCAN)


set(CMAKE_CXX_STANDARD 14)


IF(APPLE)
    message("Using MacOS")
    include_directories(/usr/local/Cellar/opencv/3.4.1_5/include)
    include_directories(/usr/local/Cellar/opencv/3.4.1_5/include/opencv)
    link_directories(/usr/local/Cellar/opencv/3.4.1_5/lib)

ELSE(APPLE)
    message("Using non-MacOS")
    include_directories(/usr/local/include)
    include_directories(/usr/local/include/opencv)
    link_directories(/usr/local/lib64)

ENDIF(APPLE)


link_libraries(opencv_dnn)
link_libraries(opencv_ml)
link_libraries(opencv_objdetect)
link_libraries(opencv_shape)
link_libraries(opencv_stitching)
link_libraries(opencv_superres)
link_libraries(opencv_videostab)
link_libraries(opencv_calib3d)
link_libraries(opencv_features2d)
link_libraries(opencv_highgui)
link_libraries(opencv_videoio)
link_libraries(opencv_imgcodecs)
link_libraries(opencv_video)
link_libraries(opencv_photo)
link_libraries(opencv_imgproc)
link_libraries(opencv_flann)
link_libraries(opencv_core)

configure_file("${PROJECT_SOURCE_DIR}/main.hpp.in" "${PROJECT_BINARY_DIR}/main.hpp")

include_directories("${PROJECT_BINARY_DIR}")

add_executable(DBSCAN main.cpp)