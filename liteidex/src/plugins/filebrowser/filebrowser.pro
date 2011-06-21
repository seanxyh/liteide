TARGET = filebrowser
TEMPLATE = lib

include(../../liteideplugin.pri)

DEFINES += FILEBROWSER_LIBRARY

SOURCES += filebrowserplugin.cpp

HEADERS += filebrowserplugin.h\
        filebrowser_global.h
