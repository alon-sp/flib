cmake_minimum_required(VERSION 3.10)

project(Flib)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED true)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_subdirectory( "${PROJECT_SOURCE_DIR}/_test/" )

include_directories( "${PROJECT_SOURCE_DIR}/include" )

add_compile_definitions(_FLIB_DEBUG_)

add_library(flib 
    ./include/fl.c ./include/flError.c 
    ./include/flArray.c ./include/flEntity.c
    ./include/flentIOport.c
    ./include/flentXenv.c
    ./include/flentstd.c
)
target_link_libraries(flib ${SDL2_LIBRARIES} m)