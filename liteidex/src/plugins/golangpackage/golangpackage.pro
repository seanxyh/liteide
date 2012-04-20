TARGET = golangpackage
TEMPLATE = lib

include(../../liteideplugin.pri)
include(../../api/golangdocapi/golangdocapi.pri)
include (../../utils/fileutil/fileutil.pri)
include (../../utils/symboltreeview/symboltreeview.pri)
include (../../3rdparty/qjson/qjson.pri)


DEFINES += GOLANGPACKAGE_LIBRARY

SOURCES += golangpackageplugin.cpp \
    filepathmodel.cpp \
    packagebrowser.cpp \
    gotool.cpp \
    setupgopathdialog.cpp \
    packageproject.cpp \
    packagetree.cpp

HEADERS += golangpackageplugin.h\
        golangpackage_global.h \
    filepathmodel.h \
    packagebrowser.h \
    gotool.h \
    setupgopathdialog.h \
    packageproject.h \
    packagetree.h

FORMS += \
    setupgopathdialog.ui
