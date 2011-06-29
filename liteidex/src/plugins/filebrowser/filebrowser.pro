TARGET = filebrowser
TEMPLATE = lib

include(../../liteideplugin.pri)

DEFINES += FILEBROWSER_LIBRARY

SOURCES += filebrowserplugin.cpp \
    filebrowser.cpp \
    filebrowseroptionfactory.cpp \
    filebrowseroption.cpp

HEADERS += filebrowserplugin.h\
        filebrowser_global.h \
    filebrowser.h \
    filebrowseroptionfactory.h \
    filebrowseroption.h

RESOURCES += \
    filebrowser.qrc

FORMS += \
    filebrowseroption.ui
