TARGET = litedebug
TEMPLATE = lib

include(../../liteideplugin.pri)
include(../../api/litedebugapi/litedebugapi.pri)

DEFINES += LITEDEBUG_LIBRARY

SOURCES += litedebugplugin.cpp \
    debugmanager.cpp \
    debugwidget.cpp \
    litedebug.cpp

HEADERS += litedebugplugin.h\
        litedebug_global.h \
    debugmanager.h \
    debugwidget.h \
    litedebug.h
