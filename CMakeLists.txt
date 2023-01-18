cmake_minimum_required(VERSION 2.8)
project(compress)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_BUILD_TYPE "Debug")

#add_definitions(-DDEBUG)

add_subdirectory(internal/test)
add_executable(compress main.c
        internal/bitstream.c internal/vector.c
        internal/pqueue.c internal/str.c
        huffman.c comp.c)
