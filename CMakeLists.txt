CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
FIND_PACKAGE(OpenCV REQUIRED)

PROJECT(Xtracking)


ADD_EXECUTABLE(main main.cpp calibration.cpp calibration.h image.cpp image.h tracking.h tracking.cpp detection.cpp detection.h output.cpp output.h)
TARGET_LINK_LIBRARIES(main ${OpenCV_LIBS})
