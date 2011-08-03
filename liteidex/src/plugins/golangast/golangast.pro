TARGET = golangast
TEMPLATE = lib

include (../../liteideplugin.pri)
include (../../utils/symboltreeview/symboltreeview.pri)

DEFINES += GOLANGAST_LIBRARY

SOURCES += golangastplugin.cpp \
    golangast.cpp \
    golangasticon.cpp \
    astwidget.cpp

HEADERS += golangastplugin.h\
        golangast_global.h \
    golangast.h \
    golangasticon.h \
    astwidget.h \
    golangastitem.h

RESOURCES += \
    golangast.qrc
