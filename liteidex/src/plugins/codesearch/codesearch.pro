TARGET = codesearch
TEMPLATE = lib

include(../../liteideplugin.pri)

DEFINES += CODESEARCH_LIBRARY

SOURCES += codesearchplugin.cpp \
    editorsearchdialog.cpp \
    editorreplacedialog.cpp

HEADERS += codesearchplugin.h\
        codesearch_global.h \
    editorsearchdialog.h \
    editorreplacedialog.h

FORMS += \
    editorsearchdialog.ui \
    editorreplacedialog.ui
