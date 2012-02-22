TARGET = golangtool
TEMPLATE = lib

include(../../liteideplugin.pri)

DEFINES += GOLANGTOOL_LIBRARY

SOURCES += golangtoolplugin.cpp

HEADERS += golangtoolplugin.h\
        golangtool_global.h
