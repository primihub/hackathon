cmake_minimum_required(VERSION 3.16)

project(ppptaby)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost 1.66.0 REQUIRED COMPONENTS thread system)
find_library(ABY aby abytarget/lib)
find_library(ENC encrypto_utils abytarget/lib)
find_library(GMPXX gmpxx /usr/local/lib)
find_library(GMP gmp /usr/local/lib)
find_library(OT otextension abytarget/lib)
find_library(RELIC relic_s abytarget/lib)
find_library(SSL ssl /lib/x86_64-linux-gnu)
find_library(CRYPTO crypto /lib/x86_64-linux-gnu)
find_library(DL dl /lib/x86_64-linux-gnu)

# circ_ay
add_executable(circ_ay circ_ay/pppt.cpp circ_ay/mytest.cpp)
target_include_directories(circ_ay PRIVATE abytarget/include)
target_link_libraries(circ_ay ${ABY} ${ENC} ${GMPXX} ${GMP} ${OT} ${RELIC} Boost::thread ${SSL} ${CRYPTO} ${DL})

# circ_y
add_executable(circ_y circ_y/pppt.cpp circ_y/mytest.cpp)
target_include_directories(circ_y PRIVATE abytarget/include)
target_link_libraries(circ_y ${ABY} ${ENC} ${GMPXX} ${GMP} ${OT} ${RELIC} Boost::thread ${SSL} ${CRYPTO} ${DL})

# poly_ay
add_executable(poly_ay poly_ay/pppt.cpp poly_ay/mytest.cpp)
target_include_directories(poly_ay PRIVATE abytarget/include)
target_link_libraries(poly_ay ${ABY} ${ENC} ${GMPXX} ${GMP} ${OT} ${RELIC} Boost::thread ${SSL} ${CRYPTO} ${DL})

# poly_y
add_executable(poly_y poly_y/pppt.cpp poly_y/mytest.cpp)
target_include_directories(poly_y PRIVATE abytarget/include)
target_link_libraries(poly_y ${ABY} ${ENC} ${GMPXX} ${GMP} ${OT} ${RELIC} Boost::thread ${SSL} ${CRYPTO} ${DL})
