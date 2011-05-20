TARGET = welcome
TEMPLATE = lib

include(../../liteideplugin.pri)
include(../../utils/browsereditor/browsereditor.pri)

DEFINES += WELCOME_LIBRARY

SOURCES += welcomeplugin.cpp \
    welcomebrowser.cpp

HEADERS += welcomeplugin.h\
        welcome_global.h \
    welcomebrowser.h

FORMS += \
    welcomewidget.ui

OTHER_FILES +=

RESOURCES += \
    welcome.qrc
