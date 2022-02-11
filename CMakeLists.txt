cmake_minimum_required(VERSION 3.1.0 )

project(KilobotArena VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)



find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} ./)

# Print some message showing some of them
message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "Qt5 library status:")
message(STATUS "    version: ${Qt5_VERSION}")
message(STATUS "    config: ${Qt5_DIR}")
message(STATUS "    include path: ${Qt5_INCLUDE_DIRS}")

set(SOURCES
        main.cpp
        mainwindow.cpp
        clicksignalqlabel.cpp
        dragzoomqlabel.cpp
        kilobottracker.cpp
        toolchain/kilobot.cpp
        kilobotoverheadcontroller.cpp
        ohc/calibrate.cpp
        ohc/intelhex.cpp
        ohc/serialconn.cpp
        ohc/serialwin.cpp
        kilobotidassignment.cpp
        kilobotcalibrate.cpp
        kilobotcalibrateenv.cpp
        userthread.cpp
        mainwindow.h
        clicksignalqlabel.h
        dragzoomqlabel.h
        kilobottracker.h
        toolchain/kilobot.h
        kilobotoverheadcontroller.h
        ohc/calibrate.h
        ohc/intelhex.h
        ohc/packet.h
        ohc/serialconn.h
        ohc/serialwin.h
        toolchain/kilobotenvironment.h
        toolchain/kilobotexperiment.h
        userthread.h
        kilobotidassignment.h
        kilobotcalibrate.h
        kilobotcalibrateenv.h
        )

qt5_add_resources(SOURCES ressources.qrc)
qt5_wrap_ui(SOURCES mainwindow.ui)

add_executable(KilobotArena ${SOURCES})

target_compile_definitions(KilobotArena PUBLIC USE_OPENCV3)

target_link_libraries(KilobotArena Qt5::Widgets)
target_link_libraries (KilobotArena ${OpenCV_LIBS})