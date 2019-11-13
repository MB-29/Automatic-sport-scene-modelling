CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
FIND_PACKAGE(OpenCV REQUIRED)

PROJECT(Xtracking)

ADD_EXECUTABLE(x-tracking main.cpp homographie.cpp video.cpp image.cpp image.h)
TARGET_LINK_LIBRARIES(x-tracking ${OpenCV_LIBS})
