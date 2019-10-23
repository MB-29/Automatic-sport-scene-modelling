CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
FIND_PACKAGE(OpenCV REQUIRED)

PROJECT(Xtracking)

ADD_EXECUTABLE(homographie homographie.cpp image.cpp image.h)
# ADD_EXECUTABLE(openvideo openvideo.cpp image.cpp image.h)
TARGET_LINK_LIBRARIES(homographie ${OpenCV_LIBS})
