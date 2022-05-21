cmake_minimum_required(VERSION 3.10)

project(Flib)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED true)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(FLIB_SRC_HOME "${PROJECT_SOURCE_DIR}/src")

set(FLIB_INCLUDE_DIRS
    "${PROJECT_SOURCE_DIR}/extlib"
    "${PROJECT_SOURCE_DIR}/extlib/glew"
    "${FLIB_SRC_HOME}/com"
    "${FLIB_SRC_HOME}/util"
    "${FLIB_SRC_HOME}/entity"
    "${FLIB_SRC_HOME}/container"
    "${FLIB_SRC_HOME}/file"
    "${FLIB_SRC_HOME}/gl"
    "${FLIB_SRC_HOME}/geom"
    "${FLIB_SRC_HOME}/math"
)

include_directories( ${FLIB_INCLUDE_DIRS} )

add_subdirectory( "${PROJECT_SOURCE_DIR}/_test/" )
add_subdirectory( "${PROJECT_SOURCE_DIR}/_testProg/glRect" )
add_subdirectory( "${PROJECT_SOURCE_DIR}/_testProg/glTexture" )

add_compile_definitions(_FLIB_DEBUG_)

add_library(flib 
    ${FLIB_SRC_HOME}/com/fl.c ${FLIB_SRC_HOME}/com/flError.c 
    ${FLIB_SRC_HOME}/container/flArray.c ${FLIB_SRC_HOME}/entity/flEntity.c
    ${FLIB_SRC_HOME}/entity/flentIOport.c
    ${FLIB_SRC_HOME}/entity/flentXenv.c
    ${FLIB_SRC_HOME}/entity/flentstd.c
    ${FLIB_SRC_HOME}/file/flfi.c
    ${FLIB_SRC_HOME}/gl/flgl.c
    ${FLIB_SRC_HOME}/util/fluti.c
    ${FLIB_SRC_HOME}/geom/flgm.c
    ${FLIB_SRC_HOME}/math/flm.c
)
target_link_libraries(flib PUBLIC ${SDL2_LIBRARIES} PUBLIC m GL GLU GLEW)