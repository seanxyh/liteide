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

WelcomeBrowser::WelcomeBrowser(LiteApi::IApplication *app)
    : m_liteApp(app),
      m_widget(new QWidget),
      ui (new Ui::WelcomeWidget)
{
    ui->setupUi(m_widget);

    m_recentProjectsModel = new QStringListModel(this);
    ui->recentProjectslistView->setModel(m_recentProjectsModel);
    m_recentProjectsModel->setStringList(m_liteApp->fileManager()->recentProjects());
    ui->recentProjectslistView->setEditTriggers(0);

    m_recentFilesModel = new QStringListModel(this);
    ui->recentFileslistView->setModel(m_recentFilesModel);
    m_recentFilesModel->setStringList(m_liteApp->fileManager()->recentFiles());
    ui->recentFileslistView->setEditTriggers(0);

    connect(ui->openProjectButton,SIGNAL(clicked()),m_liteApp->fileManager(),SLOT(openProjects()));
    connect(ui->newProjectButton,SIGNAL(clicked()),m_liteApp->fileManager(),SLOT(newFile()));
    connect(ui->recentProjectslistView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(openRecentProjectList(QModelIndex)));
    connect(ui->recentFileslistView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(openRecentFileList(QModelIndex)));
    connect(m_liteApp->fileManager(),SIGNAL(recentProjectsChanged()),this,SLOT(recentProjectsChanged()));
    connect(m_liteApp->fileManager(),SIGNAL(recentFilesChanged()),this,SLOT(recentFilesChanged()));
    connect(ui->optionsButton,SIGNAL(clicked()),m_liteApp->optionManager(),SLOT(exec()));
}

WelcomeBrowser::~WelcomeBrowser()
{
   delete ui;
   delete m_widget;
}

QString WelcomeBrowser::displayName() const
{
    return tr("Welcome");
}

QWidget *WelcomeBrowser::widget()
{
    return m_widget;
}

void WelcomeBrowser::openRecentProjectList(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    QString fileName = index.data().toString();
    m_liteApp->fileManager()->openProject(fileName);
}

void WelcomeBrowser::recentProjectsChanged()
{
    m_recentProjectsModel->setStringList(m_liteApp->fileManager()->recentProjects());
}

void WelcomeBrowser::openRecentFileList(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    QString fileName = index.data().toString();
    m_liteApp->fileManager()->openEditor(fileName);
}

void WelcomeBrowser::recentFilesChanged()
{
    m_recentFilesModel->setStringList(m_liteApp->fileManager()->recentFiles());
}
