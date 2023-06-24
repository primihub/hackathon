cmake_minimum_required(VERSION 3.16)

project(ppptbfv)

# check if support c++17
set(CMAKE_BUILD_TYPE Debug) # lib is release version
set(CMAKE_CXX_STANDARD 17)

find_package(SEAL 4.1 REQUIRED)

set(DEP SEAL::seal)
set(SEAL_USE_STATIC_LIBS ON)

# non-interactive-demo
add_executable(non-interactive-demo demo.cc)
target_include_directories(non-interactive-demo PRIVATE ./include)
target_link_libraries(non-interactive-demo PRIVATE ${DEP})

# client
add_executable(client client.cc)
target_include_directories(client PRIVATE ./include)
target_link_libraries(client PRIVATE ${DEP})

# server
add_executable(server server.cc)
target_include_directories(server PRIVATE ./include)
target_link_libraries(server PRIVATE ${DEP})

# testclient
add_executable(testclient test/test_client.cc)
target_include_directories(testclient PRIVATE ./include)
target_link_libraries(testclient PRIVATE ${DEP})

# testserver
add_executable(testserver test/test_server.cc)
target_include_directories(testserver PRIVATE ./include)
target_link_libraries(testserver PRIVATE ${DEP})


