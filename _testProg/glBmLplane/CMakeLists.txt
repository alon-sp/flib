
add_executable(glBmLplane main.c )

target_link_libraries(glBmLplane PUBLIC flib)

target_include_directories(glBmLplane PUBLIC . )
