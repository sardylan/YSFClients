cmake_minimum_required(VERSION 3.15)
project(ysf-clients)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

find_package(Threads REQUIRED)
message("Threads flags: ${CMAKE_THREAD_LIBS_INIT}")

add_subdirectory(YSFReflector)
