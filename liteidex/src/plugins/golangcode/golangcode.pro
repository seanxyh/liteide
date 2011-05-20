TARGET = golangcode
TEMPLATE = lib

include(../../liteideplugin.pri)
include(../../utils/fileutil/fileutil.pri)
include(../../utils/processex/processex.pri)

DEFINES += GOLANGCODE_LIBRARY

SOURCES += golangcodeplugin.cpp \
    golangcode.cpp

HEADERS += golangcodeplugin.h\
        golangcode_global.h \
    golangcode.h
