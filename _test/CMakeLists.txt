
add_executable(flibTest main.c _flArray.c _flfil.c  _flgm.c)

target_link_libraries(flibTest PUBLIC flib)

target_include_directories(flibTest PUBLIC . ./entity )
