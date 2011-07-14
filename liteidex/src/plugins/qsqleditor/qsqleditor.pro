TARGET = qsqleditor
TEMPLATE = lib
QT += sql

include(../../liteideplugin.pri)

DEFINES += QSQLEDITOR_LIBRARY

SOURCES += qsqleditorplugin.cpp \
    qsqleditor.cpp \
    sqliterfile.cpp \
    qsqlfilefactory.cpp \
    qsqldbfile.cpp

HEADERS += qsqleditorplugin.h\
        qsqleditor_global.h \
    qsqleditor.h \
    sqlitefile.h \
    qsqlfilefactory.h \
    qsqldbfile.h
