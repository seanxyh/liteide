TARGET = qsqleditor
TEMPLATE = lib

include(../../liteideplugin.pri)

DEFINES += QSQLEDITOR_LIBRARY

SOURCES += qsqleditorplugin.cpp

HEADERS += qsqleditorplugin.h\
        qsqleditor_global.h
