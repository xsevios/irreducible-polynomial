cmake_minimum_required(VERSION 3.10)
project(libirredpoly)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

include_directories(source/lib)
include_directories(source/lib/Polynom)
include_directories(source/lib/Scheduler)

add_library(irredpoly SHARED
        Polynom/checkIrreducible.cpp
        Polynom/checkIrreducible.h
        Polynom/polynom.cpp
        Polynom/polynom.h
        Polynom/polynom_ext.cpp
        Polynom/polynom_ext.h
        Polynom/Field.cpp
        Polynom/FieldExt.cpp
        Scheduler/scheduler.cpp
        Scheduler/scheduler.h
        BigInt/bigint.cpp
        BigInt/bigint.h
        BigInt/fft.cpp
        BigInt/fft.h
        lib.h
        Utils/utils.h)

target_link_libraries(irredpoly Threads::Threads)
