TARGET = golangtool
TEMPLATE = lib

include(../../liteideplugin.pri)

DEFINES += GOLANGTOOL_LIBRARY

SOURCES += golangtoolplugin.cpp \
    gopathbrowser.cpp \
    gopathmodel.cpp

HEADERS += golangtoolplugin.h\
        golangtool_global.h \
    gopathbrowser.h \
    gopathmodel.h

FORMS += \
    gopathbrowser.ui

RESOURCES += \
    golangtool.qrc
