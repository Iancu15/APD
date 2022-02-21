cmake_minimum_required(VERSION 3.20)
project(sol C)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(sol
        genetic_algorithm.c
        genetic_algorithm.h
        individual.h
        sack_object.h
        tema1.c)
target_link_libraries (sol ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_NAME} m)
