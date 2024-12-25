CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(WBSM4)

include_directories(include)

aux_source_directory(src/WBMatrix WBSM4_SRC)
aux_source_directory(src WBSM4_SRC)

add_library(WBSM4_LIB ${WBSM4_SRC})

add_executable(WBSM4 test/main.c)

target_link_libraries(WBSM4 WBSM4_LIB)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O2")