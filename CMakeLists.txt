CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
FIND_PACKAGE(OpenCV REQUIRED)

PROJECT(Xtracking)


ADD_EXECUTABLE(main main.cpp calibration.cpp calibration.h image.cpp image.h tracking.h tracking.cpp detection.cpp detection.h)
# ADD_EXECUTABLE(tracking tracking.cpp tracking.h rectangles.h rectangles.cpp)
# ADD_EXECUTABLE(detection detection.cpp detection.h rectangles.h rectangles.cpp)
TARGET_LINK_LIBRARIES(main ${OpenCV_LIBS})
# TARGET_LINK_LIBRARIES(tracking ${OpenCV_LIBS})
# TARGET_LINK_LIBRARIES(detection ${OpenCV_LIBS})

