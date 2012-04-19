TARGET = golangpackage
TEMPLATE = lib

include(../../liteideplugin.pri)
include (../../utils/fileutil/fileutil.pri)
include (../../3rdparty/qjson/qjson.pri)


DEFINES += GOLANGPACKAGE_LIBRARY

SOURCES += golangpackageplugin.cpp \
    filepathmodel.cpp \
    packagebrowser.cpp \
    gotool.cpp \
    setupgopathdialog.cpp \
    packageproject.cpp

HEADERS += golangpackageplugin.h\
        golangpackage_global.h \
    filepathmodel.h \
    packagebrowser.h \
    gotool.h \
    setupgopathdialog.h \
    packageproject.h

FORMS += \
    setupgopathdialog.ui
