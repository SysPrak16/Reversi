cmake_minimum_required(VERSION 3.3)
project(Reversi)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu99")

set(SOURCE_FILES
        lib/appParamHandler.h
        lib/errorHandler.h
        lib/fileIOHandler.h
        main.c
        lib/performConnection.c
        lib/performConnection.h
        README.md lib/appParamHandler.c lib/global.h lib/fileIOHandler.c lib/fileIOHandler.h lib/errorHandler.c lib/thinker.c lib/thinker.h lib/connector.c lib/connector.h)

add_executable(Reversi ${SOURCE_FILES})