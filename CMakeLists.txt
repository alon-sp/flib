
cmake_minimum_required(VERSION 3.10)

project(FLtest)


set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED true)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_subdirectory("${PROJECT_SOURCE_DIR}/common")

add_executable(flTest ./common/_test/main.c ./common/_test/_flArray.c ./common/_test/entity/_flEntity.c)

target_link_libraries(flTest PUBLIC fl)

target_include_directories(flTest PUBLIC 
    "${PROJECT_SOURCE_DIR}/common/_test"
    "${PROJECT_SOURCE_DIR}/common/_test/entity"
)