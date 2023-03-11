cmake_minimum_required(VERSION 3.10)
project(benchmark-test)

set(CMAKE_CXX_STANDARD 17)

include_directories(source/driver)
include_directories(source/driver/Config)
include_directories(source/driver/LibraryInterface)

add_executable(irreducible_polynomial_benchmark_test
        LibraryInterface/LibraryInterface.cpp
        LibraryInterface/LibraryInterface.h
        benchmark.cpp
        benchmark.h
        main.cpp)

target_link_libraries(irreducible_polynomial_benchmark_test irredpoly ${CMAKE_DL_LIBS})