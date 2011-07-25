TARGET = liteenv
TEMPLATE = lib

include(../../liteideplugin.pri)

DEFINES += LITEENV_LIBRARY

SOURCES += liteenvplugin.cpp

HEADERS += liteenvplugin.h\
        liteenv_global.h
