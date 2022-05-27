
add_executable(glBmRect main.c )

target_link_libraries(glBmRect PUBLIC flib)

target_include_directories(glBmRect PUBLIC . )
