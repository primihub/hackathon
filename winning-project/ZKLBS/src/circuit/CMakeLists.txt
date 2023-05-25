cmake_minimum_required(VERSION 3.8)
project(circuit)

add_subdirectory(../../ethsnarks ../../.build/ethsnarks EXCLUDE_FROM_ALL)

add_library(PoseidonClient SHARED PoseidonClient.cpp)
target_link_libraries(PoseidonClient ethsnarks_common)

add_library(PoseidonServer SHARED PoseidonServer.cpp)
target_link_libraries(PoseidonServer ethsnarks_common)

