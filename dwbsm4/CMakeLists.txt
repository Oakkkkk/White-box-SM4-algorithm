cmake_minimum_required(VERSION 3.19)
project(dwbsm4 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(dwbsm4
        main.c
        random.c
        random.h
        sbox.h
        sm4.c
        sm4.h
        structure.h
        WBMatrix.c
        WBMatrix.h
        wbsm4.c
        wbsm4.h)
