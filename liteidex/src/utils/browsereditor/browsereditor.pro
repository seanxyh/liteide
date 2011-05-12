TARGET = browsereditor
TEMPLATE = lib

CONFIG += staticlib

include(../../liteideutils.pri)


HEADERS += browsereditor.h \ 
    browsereditormanager.h \
    browsereditorimpl.h \
	

SOURCES += browsereditor.cpp \
    browsereditormanager.cpp \
    browsereditorimpl.cpp \
