cmake_minimum_required(VERSION 3.1)

project(hsg VERSION 0.1
    DESCRIPTION "A fast grep tool using the HyperScan Regex Engine"
    LANGUAGES C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

add_executable(hsg hsg.c)

add_library(scanner scanner.c)

target_link_libraries(hsg scanner)

find_package(hs REQUIRED)

if (hs_FOUND)
    include_directories(${hs_INCLUDE_DIRS})
    target_link_libraries(hsg ${hs_LIBRARIES})
endif (hs_FOUND)
