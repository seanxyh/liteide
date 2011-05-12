TARGET = golangast
TEMPLATE = lib

include (../../liteideplugin.pri)
include (../../utils/symboltreeview/symboltreeview.pri)

DEFINES += GOLANGAST_LIBRARY

SOURCES += golangastplugin.cpp \
    golangast.cpp \
    golangasticon.cpp

HEADERS += golangastplugin.h\
        golangast_global.h \
    golangast.h \
    golangasticon.h

RESOURCES += \
    golangast.qrc
