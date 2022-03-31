cmake_minimum_required(VERSION 3.10)

project(Flib)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED true)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_subdirectory( "${PROJECT_SOURCE_DIR}/_test/" )

include_directories( ${PROJECT_SOURCE_DIR} )

add_library(flib fl.c ./common/flError.c ./common/container/flArray.c ./common/entity/flEntity.c)
