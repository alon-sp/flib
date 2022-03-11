

include_directories(./container ./entity .)

add_library(fl fl.c flError.c ./container/flArray.c ./entity/flEntity.c)
