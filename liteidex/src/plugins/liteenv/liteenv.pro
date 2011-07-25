TARGET = liteenv
TEMPLATE = lib

include(../../liteideplugin.pri)
include(../../api/liteenvapi/liteenvapi.pri)

DEFINES += LITEENV_LIBRARY

SOURCES += liteenvplugin.cpp \
    envmanager.cpp

HEADERS += liteenvplugin.h\
        liteenv_global.h \
    envmanager.h
