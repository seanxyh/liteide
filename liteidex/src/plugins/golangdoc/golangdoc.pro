TARGET = golangdoc
TEMPLATE = lib
QT += xml

include(../../liteideplugin.pri)
include(../../api/golangdocapi/golangdocapi.pri)
include(../../utils/documentbrowser/documentbrowser.pri)
include(../../utils/fileutil/fileutil.pri)
include(../../utils/processex/processex.pri)
include(../../utils/htmlutil/htmlutil.pri)

DEFINES += GOLANGDOC_LIBRARY

SOURCES += golangdocplugin.cpp \
    golangdoc.cpp

HEADERS += golangdocplugin.h\
        golangdoc_global.h \
    golangdoc.h
