TARGET = gdbdebuger
TEMPLATE = lib

include(../../liteideplugin.pri)

DEFINES += GDBDEBUGER_LIBRARY

SOURCES += gdbdebugerplugin.cpp

HEADERS += gdbdebugerplugin.h\
        gdbdebuger_global.h
