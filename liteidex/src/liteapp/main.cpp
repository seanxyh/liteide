/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** In addition, as a special exception,  that plugins developed for LiteIDE,
** are allowed to remain closed sourced and can be distributed under any license .
** These rights are included in the file LGPL_EXCEPTION.txt in this package.
**
**************************************************************************/
// Module: main.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: main.cpp,v 1.0 2011-5-12 visualfc Exp $

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QDir>
#include "mainwindow.h"
#include "liteapp.h"

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

#ifdef Q_OS_MAC
#  define SHARE_PATH "/../Resources"
#else
#  define SHARE_PATH "/../share/liteide"
#endif

static inline QString getPluginPath()
{
    // Figure out root:  Up one from 'bin'
    QDir rootDir = QApplication::applicationDirPath();
    rootDir.cdUp();
    const QString rootDirPath = rootDir.canonicalPath();
    QString pluginPath = rootDirPath;
#ifdef Q_OS_MAC
    pluginPath += QLatin1String("/PlugIns");
#else
    pluginPath += QLatin1String("/plugins");
#endif
    return pluginPath;
}

static inline QString getResoucePath()
{
    QDir rootDir = QApplication::applicationDirPath();
    rootDir.cdUp();
    const QString rootDirPath = rootDir.canonicalPath();
    QString pluginPath = rootDirPath;
#ifdef Q_OS_MAC
    pluginPath += QLatin1String("/Resources");
#else
    pluginPath += QLatin1String("/data");
#endif
    return pluginPath;
}

int  main(int argc, char *argv[])
{
#if defined(_MSC_VER) && defined(_DEBUG)
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif
    QApplication a(argc, argv);

    QTranslator translator;
    QTranslator qtTranslator;
    QString locale = QLocale::system().name();

    const QString &liteideTrPath = getResoucePath()+"/translations";
    if (translator.load(QLatin1String("liteide_") + locale, liteideTrPath)) {
        const QString &qtTrPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
        const QString &qtTrFile = QLatin1String("qt_") + locale;
        // Binary installer puts Qt tr files into creatorTrPath
        if (qtTranslator.load(qtTrFile, qtTrPath) || qtTranslator.load(qtTrFile, liteideTrPath)) {
            a.installTranslator(&translator);
            a.installTranslator(&qtTranslator);
            a.setProperty("liteide_locale", locale);
        } else {
            translator.load(QString()); // unload()
        }
    }

    LiteApp *app = new LiteApp;
    app->setPluginPath(getPluginPath());
    app->setResourcePath(getResoucePath());
    app->loadMimeType();
    app->loadPlugins();
    app->initPlugins();
    app->mainWindow()->show();

    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            QString fileName = QDir::toNativeSeparators(argv[i]);
            if (QFile::exists(fileName)) {
                app->fileManager()->openFile(fileName);
            }
        }
    }
    int ret = a.exec();
    delete app;
    return ret;
}
