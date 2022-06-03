
add_executable(glBmCube main.c )

target_link_libraries(glBmCube PUBLIC flib)

target_include_directories(glBmCube PUBLIC . )
