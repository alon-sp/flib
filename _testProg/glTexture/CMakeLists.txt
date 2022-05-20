
add_executable(glTexture main.c )

target_link_libraries(glTexture PUBLIC flib)

target_include_directories(glTexture PUBLIC . )
