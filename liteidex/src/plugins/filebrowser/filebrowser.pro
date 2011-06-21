TARGET = filebrowser
TEMPLATE = lib

include(../../liteideplugin.pri)

DEFINES += FILEBROWSER_LIBRARY

SOURCES += filebrowserplugin.cpp \
    filebrowser.cpp

HEADERS += filebrowserplugin.h\
        filebrowser_global.h \
    filebrowser.h

RESOURCES += \
    filebrowser.qrc
