cmake_minimum_required(VERSION 3.5)

project(ZKLBS LANGUAGES CXX)

# set(CMAKE_PREFIX_PATH "/home/xt/Qt5.14.2/5.14.2/gcc_64")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(QRC_FILE source.qrc images.qrc)
qt5_add_resources(QRC ${QRC_FILE})

add_executable(ZKLBS
main.cpp
mainwindow.cpp
mainwindow.h
mainwindow.ui
successwindow.cpp
successwindow.h
successwindow.ui
failwindow.cpp
failwindow.h
failwindow.ui
${QRC}
)

add_subdirectory(../grpc ../../.build/grpc EXCLUDE_FROM_ALL)
include_directories(../grpc/build)

target_link_libraries(ZKLBS 
  Qt5::Widgets
  TypeUtil
  CoordinateUtil
  RandomUtil
  greeterClient
)
