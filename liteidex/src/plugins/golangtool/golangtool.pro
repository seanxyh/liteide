TARGET = golangtool
TEMPLATE = lib

include(../../liteideplugin.pri)

DEFINES += GOLANGTOOL_LIBRARY

SOURCES += golangtoolplugin.cpp \
    gopathbrowser.cpp \
    gopathmodel.cpp \
    gopathproject.cpp \
    gopathprojectfactor.cpp

HEADERS += golangtoolplugin.h\
        golangtool_global.h \
    gopathbrowser.h \
    gopathmodel.h \
    gopathproject.h \
    gopathprojectfactor.h

FORMS +=

RESOURCES += \
    golangtool.qrc
