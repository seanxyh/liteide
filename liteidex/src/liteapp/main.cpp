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
#include <QSettings>
#include <QDebug>
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

static inline QString getRootPath()
{
    QDir rootDir = QApplication::applicationDirPath();
    rootDir.cdUp();
    return rootDir.canonicalPath();
}

static inline QString getPluginPath()
{
    QString root = getRootPath();
#ifdef Q_OS_MAC
    return root+"/PlugIns";
#else
    return root+"/lib/liteide/plugins";
#endif
}

static inline QString getResoucePath()
{
    QString root = getRootPath();
#ifdef Q_OS_MAC
    return root+"/Resources";
#else
    return root+"/share/liteide";
#endif
}

int  main(int argc, char *argv[])
{
#if defined(_MSC_VER) && defined(_DEBUG)
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif
    QApplication app(argc, argv);

    QTranslator translator;
    QTranslator qtTranslator;
    const QSettings settings(QSettings::IniFormat,QSettings::UserScope,"liteide","liteide");
    QString locale = QLocale::system().name();
    locale = settings.value("General/Language",locale).toString();
    if (!locale.isEmpty()) {
        const QString &liteideTrPath = getResoucePath()+"/translations";
        if (translator.load(QLatin1String("liteide_") + locale, liteideTrPath)) {
            const QString &qtTrPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
            const QString &qtTrFile = QLatin1String("qt_") + locale;
            // Binary installer puts Qt tr files into creatorTrPath            
            app.installTranslator(&translator);
            if (qtTranslator.load(qtTrFile, qtTrPath) || qtTranslator.load(qtTrFile, liteideTrPath)) {
                app.installTranslator(&qtTranslator);
            }
            app.setProperty("liteide_locale", locale);
        }
    }
    QString resPath = getResoucePath();
    QDir::addSearchPath("icon",resPath);
    QDir::addSearchPath("icon",resPath+"/liteapp");
    QDir::addSearchPath("icon",":/");
    LiteApp *liteApp = new LiteApp;
    liteApp->setPluginPath(getPluginPath());
    liteApp->setResourcePath(resPath);
    liteApp->load();

    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            QString fileName = argv[i];
            if (QFile::exists(fileName)) {
                liteApp->fileManager()->openFile(fileName);
            }
        }
    }
    int ret = app.exec();
    delete liteApp;
    return ret;
}
