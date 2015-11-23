TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    performConnection.c \
    connector.c \
    config.c \
    thinker.c

HEADERS += \
    constants.h \
    performConnection.h \
    connector.h \
    config.h \
    thinker.h

QMAKE_CFLAGS += -std=gnu99 -Wall -Wcomment

OTHER_FILES += \
    Makefile \
    client.conf
