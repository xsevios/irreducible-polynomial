cmake_minimum_required(VERSION 3.10)
project(irreducible_polynomial)

set(CMAKE_CXX_STANDARD 14)

include_directories(source/driver)
include_directories(source/driver/Config)
include_directories(source/driver/LibraryInterface)

add_executable(irreducible_polynomial
        Config/config.cpp
        Config/config.h
        LibraryInterface/LibraryInterface.cpp
        LibraryInterface/LibraryInterface.h
        driver.cpp
        driver.h
        main.cpp)

target_link_libraries(irreducible_polynomial ${CMAKE_DL_LIBS})