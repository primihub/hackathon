cmake_minimum_required(VERSION 3.8)
project(utils)

add_library(TimeUtil SHARED TimeUtil.cpp)
add_library(TypeUtil SHARED TypeUtil.cpp)
add_library(RandomUtil SHARED RandomUtil.cpp)

add_library(IpUtil SHARED IpUtil.cpp)
target_link_libraries(IpUtil curl)

add_library(CoordinateUtil SHARED CoordinateUtil.cpp)
target_link_libraries(CoordinateUtil IpUtil)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
include_directories(${JSONCPP_LIBRARIES})
target_link_libraries(CoordinateUtil ${JSONCPP_LIBRARIES})