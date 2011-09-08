TARGET = gdbdebuger
TEMPLATE = lib

include(../../liteideplugin.pri)
include(../../api/litedebugapi/litedebugapi.pri)
include(../../utils/fileutil/fileutil.pri)
include(../../3rdparty/qtc_gdbmi/qtc_gdbmi.pri)

DEFINES += GDBDEBUGER_LIBRARY

SOURCES += gdbdebugerplugin.cpp \
    gdbdebuger.cpp \

HEADERS += gdbdebugerplugin.h\
        gdbdebuger_global.h \
    gdbdebuger.h \
