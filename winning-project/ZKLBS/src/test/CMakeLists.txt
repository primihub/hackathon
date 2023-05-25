cmake_minimum_required(VERSION 3.8)
project(test)

add_subdirectory(../circuit ../../.build/circuit EXCLUDE_FROM_ALL)

# add_executable(test_ZKposeidon test_ZKposeidon.cpp)
# target_link_libraries(test_ZKposeidon PoseidonClient)
# target_link_libraries(test_ZKposeidon PoseidonServer)

# add_executable(test_coordinate test_coordinate.cpp)
# target_link_libraries(test_coordinate curl)
# add_executable(test_thread test_thread.cpp)

add_executable(test_type test_type.cpp)