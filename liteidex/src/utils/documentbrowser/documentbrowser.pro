TARGET = documentbrowser
TEMPLATE = lib
CONFIG += staticlib

include (../../liteideutils.pri)

SOURCES += documentbrowser.cpp \
    documentbrowserfactory.cpp

HEADERS += documentbrowser.h \
    documentbrowserfactory.h
