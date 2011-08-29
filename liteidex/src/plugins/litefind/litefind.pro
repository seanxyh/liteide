TARGET = litefind
TEMPLATE = lib

include(../../liteideplugin.pri)

DEFINES += LITEFIND_LIBRARY

SOURCES += litefindplugin.cpp

HEADERS += litefindplugin.h\
        litefind_global.h
