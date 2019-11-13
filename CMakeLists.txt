CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
FIND_PACKAGE(OpenCV REQUIRED)

PROJECT(Xtracking)

# ADD_EXECUTABLE(x-tracking main.cpp homographie.cpp homographie.h video.cpp video.h image.cpp image.h)
ADD_EXECUTABLE(video video.cpp video.h)
# TARGET_LINK_LIBRARIES(x-tracking ${OpenCV_LIBS})
TARGET_LINK_LIBRARIES(video ${OpenCV_LIBS})
