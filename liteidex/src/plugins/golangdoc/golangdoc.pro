TARGET = golangdoc
TEMPLATE = lib

include (../../liteideplugin.pri)
include (../../liteenvapi/liteenvapi.pri)
include (../../utils/fileutil/fileutil.pri)
include (../../utils/processex/processex.pri)
include (../../utils/browsereditor/browsereditor.pri)

DEFINES += GOLANGDOC_LIBRARY

SOURCES += golangdocplugin.cpp \
    golangdoc.cpp \
    documentbrowser.cpp

HEADERS += golangdocplugin.h\
        golangdoc_global.h \
    golangdoc.h \
    documentbrowser.h
