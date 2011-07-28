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
#include "docbrowserapi/docbrowserapi.h"
#include "documentbrowser/documentbrowser.h"
#include "documentbrowser/documentbrowserfactory.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>
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

    m_recentProjectsModel = new QStandardItemModel(0,2,this);
    ui->recentProjectsTreeView->setModel(m_recentProjectsModel);
    ui->recentProjectsTreeView->setRootIsDecorated(false);
    ui->recentProjectsTreeView->setEditTriggers(0);
    ui->recentProjectsTreeView->setHeaderHidden(true);
    ui->recentProjectsTreeView->setTextElideMode(Qt::ElideMiddle);
    ui->recentProjectsTreeView->header()->setResizeMode(0,QHeaderView::ResizeToContents);
    //ui->recentProjectsTreeView->header()->setResizeMode(1,QHeaderView::Stretch);
    //ui->recentProjectsTreeView->header()->setStretchLastSection(true);

    m_recentFilesModel = new QStandardItemModel(0,2,this);
    ui->recentFilesTreeView->setModel(m_recentFilesModel);
    ui->recentFilesTreeView->setRootIsDecorated(false);
    ui->recentFilesTreeView->setEditTriggers(0);
    ui->recentFilesTreeView->setHeaderHidden(true);
    ui->recentFilesTreeView->setTextElideMode(Qt::ElideMiddle);
    ui->recentFilesTreeView->header()->setResizeMode(QHeaderView::ResizeToContents);

    m_docModel = new QStandardItemModel(0,2,this);
    ui->docTreeView->setModel(m_docModel);
    ui->docTreeView->setRootIsDecorated(false);
    ui->docTreeView->setEditTriggers(0);
    ui->docTreeView->setHeaderHidden(true);
    ui->docTreeView->setTextElideMode(Qt::ElideMiddle);
    ui->docTreeView->header()->setResizeMode(0,QHeaderView::ResizeToContents);

    connect(ui->newFileButton,SIGNAL(clicked()),m_liteApp->fileManager(),SLOT(newFile()));
    connect(ui->openFileButton,SIGNAL(clicked()),m_liteApp->fileManager(),SLOT(openFiles()));
    connect(ui->openProjectButton,SIGNAL(clicked()),m_liteApp->fileManager(),SLOT(openProjects()));
    connect(ui->openEditorButton,SIGNAL(clicked()),m_liteApp->fileManager(),SLOT(openEditors()));
    connect(ui->recentProjectsTreeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(openRecentProject(QModelIndex)));
    connect(ui->recentFilesTreeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(openRecentFile(QModelIndex)));
    connect(m_liteApp->fileManager(),SIGNAL(recentProjectsChanged()),this,SLOT(loadRecentProjects()));
    connect(m_liteApp->fileManager(),SIGNAL(recentFilesChanged()),this,SLOT(loadRecentFiles()));
    connect(ui->optionsButton,SIGNAL(clicked()),m_liteApp->optionManager(),SLOT(exec()));
    connect(ui->docTreeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(openLiteDoument(QModelIndex)));

    m_docBrowserFactory = new DocumentBrowserFactory(m_liteApp,this);
    m_liteApp->editorManager()->addFactory(m_docBrowserFactory);

    loadRecentProjects();
    loadRecentFiles();
    loadDocFiles();
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

static void resizeTreeView(QTreeView *treeView)
{
    treeView->resizeColumnToContents(0);
}

void WelcomeBrowser::loadDocFiles()
{
    m_docModel->clear();
    QDir dir(m_liteApp->resourcePath()+"/doc");
    QStringList filter;
    filter << "*.html" << "*.txt" << "*.lgpl";
    QFileInfoList infoList = dir.entryInfoList(filter,QDir::Files|QDir::NoSymLinks);
    foreach (QFileInfo info, infoList) {
        m_docModel->appendRow(QList<QStandardItem*>()
                              << new QStandardItem(info.baseName())
                              << new QStandardItem(QDir::toNativeSeparators(info.filePath()))
                              );
    }
    resizeTreeView(ui->docTreeView);
}

void WelcomeBrowser::loadRecentProjects()
{
    m_recentProjectsModel->clear();
    QStringList recentProjects = m_liteApp->fileManager()->recentProjects();
    foreach (QString file, recentProjects) {
        QFileInfo info(file);
        m_recentProjectsModel->appendRow(QList<QStandardItem*>()
                                         << new QStandardItem(info.fileName())
                                         << new QStandardItem(QDir::toNativeSeparators(info.filePath()))
                                         );
    }
    resizeTreeView(ui->recentProjectsTreeView);
}

void WelcomeBrowser::loadRecentFiles()
{
    m_recentFilesModel->clear();
    QStringList files = m_liteApp->fileManager()->recentFiles();
    foreach (QString file, files) {
        QFileInfo info(file);
        m_recentFilesModel->appendRow(QList<QStandardItem*>()
                                      << new QStandardItem(info.fileName())
                                      << new QStandardItem(QDir::toNativeSeparators(info.filePath()))
                                      );
    }
    resizeTreeView(ui->recentFilesTreeView);
}


void WelcomeBrowser::openRecentProject(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    QModelIndex i = m_recentProjectsModel->index(index.row(),1);
    if (!i.isValid()) {
        return;
    }
    QString fileName = i.data().toString();
    m_liteApp->fileManager()->openProject(fileName);
}


void WelcomeBrowser::openRecentFile(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    QModelIndex i = m_recentFilesModel->index(index.row(),1);
    if (!i.isValid()) {
        return;
    }
    QString fileName = i.data().toString();
    m_liteApp->fileManager()->openEditor(fileName);
}

void WelcomeBrowser::openLiteDoument(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    QModelIndex i = m_docModel->index(index.row(),1);
    if (!i.isValid()) {
        return;
    }
    QString fileName = i.data().toString();
    if (fileName.isEmpty()) {
        return;
    }
    LiteApi::IEditor *editor = m_liteApp->editorManager()->openEditor(fileName,"liteide/x-browser");
    if (editor) {
        m_liteApp->editorManager()->setCurrentEditor(editor);
        LiteApi::IDocumentBrowser *browser = LiteApi::findExtensionObject<LiteApi::IDocumentBrowser*>(editor,"LiteApi.IDocumentBrowser");
        if (browser) {
            browser->browser()->setOpenLinks(false);
            connect(browser->browser(),SIGNAL(anchorClicked(QUrl)),this,SLOT(openDocumentUrl(QUrl)));
        }
    }
}

void WelcomeBrowser::openDocumentUrl(const QUrl &url)
{
    if (!url.scheme().isEmpty() && url.scheme() != "file") {
        QDesktopServices::openUrl(url);
    } else {
        QTextBrowser *browser = (QTextBrowser*)sender();
        if (browser) {
            browser->setSource(url);
        }
    }
}
