
add_executable(flibTest main.c _flArray.c ./entity/_flEntity.c)

target_link_libraries(flibTest PUBLIC flib)

target_include_directories(flibTest PUBLIC . ./entity "${PROJECT_SOURCE_DIR}/include" )
