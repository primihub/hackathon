cmake_minimum_required(VERSION 3.16)

project(ppptdgk)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(CMAKE_EXE_LINKER_FLAGS "-static")

find_library(GMP gmp /usr/local/lib)
find_library(LIBM m /usr/lib/x86_64-linux-gnu)

# build dgklib
add_compile_options(-O3 -Wall -c -fPIC)
add_library(dgklib dgklib/dgk_keygen.cc dgklib/dgk.cc dgklib/gdsa.cc
            dgklib/maurer.cc dgklib/random.cc)
target_include_directories(dgklib PRIVATE dgklib/include)
target_link_libraries(dgklib ${GMP} ${LIBM})

# build main
add_executable(main main.cc)
target_include_directories(main PRIVATE dgklib/include)
target_link_libraries(main dgklib)