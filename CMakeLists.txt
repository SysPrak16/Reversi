cmake_minimum_required(VERSION 3.3)
project(Reversi)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

<<<<<<< HEAD
set(SOURCE_FILES main.c main.h)
=======
set(SOURCE_FILES
    lib/appParamHandler.h
    lib/errorHandler.h
    lib/fileIOHandler.h
    lib/serverHandler.h
    main.c
    lib/performConnection.c
    lib/performConnection.h
    README.md)

>>>>>>> master
add_executable(Reversi ${SOURCE_FILES})