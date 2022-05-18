
add_executable(glRect main.c )

target_link_libraries(glRect PUBLIC flib)

target_include_directories(glRect PUBLIC . )
