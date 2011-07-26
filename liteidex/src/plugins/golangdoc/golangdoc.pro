TARGET = golangdoc
TEMPLATE = lib

include (../../liteideplugin.pri)
include (../../utils/fileutil/fileutil.pri)
include (../../utils/processex/processex.pri)
include (../../utils/browsereditor/browsereditor.pri)
include (../../utils/documentbrowser/documentbrowser.pri)

DEFINES += GOLANGDOC_LIBRARY

SOURCES += golangdocplugin.cpp \
    golangdoc.cpp

HEADERS += golangdocplugin.h\
        golangdoc_global.h \
    golangdoc.h
