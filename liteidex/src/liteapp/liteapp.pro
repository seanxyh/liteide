#-------------------------------------------------
#
# Project created by QtCreator 2011-03-18T16:21:43
#
#-------------------------------------------------
include (../../liteide.pri)
include (../rpath.pri)
include (../api/liteapi/liteapi.pri)
include (../utils/fileutil/fileutil.pri)
include (../utils/mimetype/mimetype.pri)
include (../utils/textoutput/textoutput.pri)
include (../utils/extension/extension.pri)
include (../utils/browsereditor/browsereditor.pri)

QT       += core gui xml

TARGET = $$IDE_APP_TARGET
TEMPLATE = app

DESTDIR = $$IDE_APP_PATH
LIBS += -L$$IDE_LIB_PATH
INCLUDEPATH += $$IDE_SOURCE_TREE/src/utils

SOURCES += main.cpp\
        mainwindow.cpp \
    liteapp.cpp \
    filemanager.cpp \
    editormanager.cpp \
    projectmanager.cpp \
    pluginmanager.cpp \
    dockmanager.cpp \
    outputmanager.cpp \
    actionmanager.cpp \
    aboutdialog.cpp \
    pluginsdialog.cpp \
    mimetypemanager.cpp \
    litetabwidget.cpp \
    optionmanager.cpp \
    newfiledialog.cpp \
    optionsbrowser.cpp

HEADERS  += mainwindow.h \
    liteapp.h \
    filemanager.h \
    editormanager.h \
    projectmanager.h \
    pluginmanager.h \
    dockmanager.h \
    outputmanager.h \
    actionmanager.h \
    aboutdialog.h \
    pluginsdialog.h \
    mimetypemanager.h \
    litetabwidget.h \
    optionmanager.h \
    newfiledialog.h \
    optionsbrowser.h

FORMS += \
    aboutdialog.ui \
    pluginsdialog.ui \
    newfiledialog.ui \
    optionswidget.ui

RESOURCES += \
    liteapp.qrc

win32 {
    target.path = /bin
    INSTALLS += target
    RC_FILE += liteapp.rc
} else:macx {
    QMAKE_INFO_PLIST = Info.plist
} else {
    target.path  = /bin
    INSTALLS    += target
}
