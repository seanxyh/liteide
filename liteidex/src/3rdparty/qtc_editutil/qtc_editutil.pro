TARGET = qtc_editutil
TEMPLATE = lib

CONFIG += staticlib

include(../../liteideutils.pri)

HEADERS += \
    filterlineedit.h \
    fancylineedit.h

SOURCES += \
    filterlineedit.cpp \
    fancylineedit.cpp

RESOURCES += \
    qtc_editutil.qrc
