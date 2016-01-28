cmake_minimum_required(VERSION 3.3)
project(RevBuild001)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    connector.c
    connector.h
    fileIOHandler.c
    fileIOHandler.h
    global.h
    main.c
    main.h
    performConnection.c
    performConnection.h
    thinker.c
    thinker.h)

add_executable(RevBuild001 ${SOURCE_FILES})