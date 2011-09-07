TARGET = litedebug
TEMPLATE = lib

include(../../liteideplugin.pri)

DEFINES += LITEDEBUG_LIBRARY

SOURCES += litedebugplugin.cpp

HEADERS += litedebugplugin.h\
        litedebug_global.h
