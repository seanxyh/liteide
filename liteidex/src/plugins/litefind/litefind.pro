TARGET = litefind
TEMPLATE = lib

include(../../liteideplugin.pri)

DEFINES += LITEFIND_LIBRARY

SOURCES += litefindplugin.cpp \
    findeditor.cpp \
    replaceeditor.cpp \
    filesearch.cpp

HEADERS += litefindplugin.h\
        litefind_global.h \
    findeditor.h \
    replaceeditor.h \
    filesearch.h
