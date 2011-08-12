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
// Module: welcomebrowser.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: welcomebrowser.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "welcomebrowser.h"
#include "ui_welcomewidget.h"
#include "liteapi/litefindobj.h"
#include "golangdocapi/golangdocapi.h"
#include "litedoc.h"
#include <QMenu>
#include <QDir>
#include <QFileInfo>
#include <QAction>
#include <QFile>
#include <QTextBrowser>
#include <QDesktopServices>
#include <QDebug>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

WelcomeBrowser::WelcomeBrowser(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IBrowserEditor(parent),
      m_liteApp(app),
      m_widget(new QWidget),
      ui (new Ui::WelcomeWidget)
{
    ui->setupUi(m_widget);

    connect(ui->newFileButton,SIGNAL(clicked()),m_liteApp->fileManager(),SLOT(newFile()));
    connect(ui->openFileButton,SIGNAL(clicked()),m_liteApp->fileManager(),SLOT(openFiles()));
    connect(ui->openProjectButton,SIGNAL(clicked()),m_liteApp->fileManager(),SLOT(openProjects()));
    connect(ui->openEditorButton,SIGNAL(clicked()),m_liteApp->fileManager(),SLOT(openEditors()));
    connect(ui->optionsButton,SIGNAL(clicked()),m_liteApp->optionManager(),SLOT(exec()));
    connect(ui->textBrowser,SIGNAL(anchorClicked(QUrl)),this,SLOT(openUrl(QUrl)));
    connect(m_liteApp->fileManager(),SIGNAL(recentProjectsChanged()),this,SLOT(loadData()));
    connect(m_liteApp->fileManager(),SIGNAL(recentFilesChanged()),this,SLOT(loadData()));

    ui->textBrowser->setSearchPaths(QStringList() << m_liteApp->resourcePath()+"/doc");
    ui->textBrowser->setOpenLinks(false);

    QString path = m_liteApp->resourcePath()+"/doc/welcome.html";
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        m_templateData = file.readAll();
        file.close();
    }
    loadData();
}

void WelcomeBrowser::openUrl(const QUrl &url)
{
    if (url.scheme() == "http" || url.scheme() == "mailto") {
        QDesktopServices::openUrl(url);
    } else if (url.scheme() == "session") {
        m_liteApp->loadSession(url.path());
    } else if (url.scheme() == "proj") {
        m_liteApp->fileManager()->openProject(url.path());
    } else if (url.scheme() == "file") {
        m_liteApp->fileManager()->openEditor(url.path());
    } else if (url.scheme() == "doc") {
        LiteDoc *doc = LiteApi::findExtensionObject<LiteDoc*>(m_liteApp,"LiteApi.ILiteDoc");
        if (doc) {
            doc->openUrl(url.path());
            doc->activeBrowser();
        }
    } else if (url.scheme() == "godoc") {
        LiteApi::IGolangDoc *doc = LiteApi::findExtensionObject<LiteApi::IGolangDoc*>(m_liteApp,"LiteApi.IGolangDoc");
        if (doc) {
            doc->openUrl(url.path());
            doc->activeBrowser();
        }
    }
}

void WelcomeBrowser::loadData()
{
    QString data = m_templateData;

    QStringList sessionList;
    sessionList.append("<ul>");
    sessionList.append(QString("<li><a href=session:default>default</a></li>"));
    sessionList.append("</ul>");


    QStringList projectList;
    projectList.append("<ul>");
    QStringList recentProjects = m_liteApp->fileManager()->recentProjects();
    foreach (QString file, recentProjects) {
        QFileInfo info(file);
        projectList.append(QString("<li><a href=proj:%1>%2</a> <span class=\"recent\">%3</span></li>")
                           .arg(info.filePath())
                           .arg(info.fileName())
                           .arg(QDir::toNativeSeparators(info.filePath())));
    }
    projectList.append("</ul>");

    QStringList fileList;
    fileList.append("<ul>");
    QStringList recentFiles = m_liteApp->fileManager()->recentFiles();
    foreach (QString file, recentFiles) {
        QFileInfo info(file);
        fileList.append(QString("<li><a href=file:%1>%2</a> <span class=\"recent\">%3</span></li>")
                        .arg(info.filePath())
                        .arg(info.fileName())
                        .arg(QDir::toNativeSeparators(info.filePath())));
    }
    fileList.append("</ul>");

    data.replace("{recent_sessions}",sessionList.join("\n"));
    data.replace("{recent_projects}",projectList.join("\n"));
    data.replace("{recent_files}",fileList.join("\n"));

    ui->textBrowser->setHtml(data);

}

WelcomeBrowser::~WelcomeBrowser()
{
   delete ui;
   delete m_widget;
}

QWidget *WelcomeBrowser::widget()
{
    return m_widget;
}

QString WelcomeBrowser::name() const
{
    return tr("Welcome");
}

QString WelcomeBrowser::mimeType() const
{
    return "browser/welcome";
}
