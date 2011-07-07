TARGET = golangdoc
TEMPLATE = lib

include(../../liteideplugin.pri)

DEFINES += GOLANGDOC_LIBRARY

SOURCES += golangdocplugin.cpp

HEADERS += golangdocplugin.h\
        golangdoc_global.h
