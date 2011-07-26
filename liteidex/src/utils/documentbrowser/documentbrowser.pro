TARGET = documentbrowser
TEMPLATE = lib
CONFIG += staticlib

include (../../liteideutils.pri)
include (../browsereditor/browsereditor.pri)

SOURCES += documentbrowser.cpp

HEADERS += documentbrowser.h
