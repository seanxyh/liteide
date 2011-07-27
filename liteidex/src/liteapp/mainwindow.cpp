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
// Module: mainwindow.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: mainwindow.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "mainwindow.h"
#include "aboutdialog.h"

#include <QAction>
#include <QActionGroup>
#include <QIcon>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QDockWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QUrl>
#include <QTextBlock>
#include <QRegExp>
#include <QFileInfo>
#include <QSplitter>
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

MainWindow::MainWindow(IApplication *app,QWidget *parent)
    : QMainWindow(parent),
      m_liteApp(app)
{
   // this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowIcon(QIcon(":images/liteide128.png"));
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setAcceptDrops(true);
}

void MainWindow::init()
{
    m_mainSplitter = new QSplitter(Qt::Vertical,this);
    m_mainSplitter->addWidget(m_liteApp->editorManager()->widget());
    //m_liteApp->outputManager()->widget()->hide();
    m_mainSplitter->addWidget(m_liteApp->outputManager()->widget());
    m_mainSplitter->setStretchFactor(0,50);
    addToolBar(Qt::BottomToolBarArea,m_liteApp->outputManager()->toolBar());
    setCentralWidget(m_mainSplitter);

    //resize(640,480);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createActions()
{
    m_aboutAct = new QAction(tr("About"),this);
    connect(m_aboutAct,SIGNAL(triggered()),this,SLOT(about()));
}

void MainWindow::createMenus()
{
    IActionManager *m = m_liteApp->actionManager();
    m->addMenu("file",tr("File"));
    m->addMenu("edit",tr("Edit"));
    m->addMenu("view", tr("View"));
    m->addMenu("tools", tr("Tools"));
    m->addMenu("help",tr("Help"))->addAction(m_aboutAct);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_liteApp->saveSession("default");
    m_liteApp->projectManager()->closeProject();
    if (m_liteApp->editorManager()->closeAllEditors(false)) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;
    foreach (QUrl url, urls) {
        QString fileName = url.toLocalFile();
        if (!fileName.isEmpty()) {
            m_liteApp->fileManager()->openFile(fileName);
        }
    }
}

void MainWindow::currentEditorChanged(IEditor *editor)
{
    QString title = "LiteIDE";
    if (editor && !editor->displayName().isEmpty()) {
        title += " - " + editor->displayName();
        if (editor->isModified()) {
            title += " * ";
        }
        connect(editor,SIGNAL(modificationChanged(bool)),this,SLOT(editorModifyChanged(bool)));
    }
    this->setWindowTitle(title);
}

void MainWindow::editorModifyChanged(bool b)
{
    IEditor *editor = (IEditor*)sender();
    QString title = "LiteIDE";
    if (editor && !editor->displayName().isEmpty()) {
        title += " - " + editor->displayName();
        if (b == true) {
            title += " * ";
        }
        this->setWindowTitle(title);
    }
}

void MainWindow::about()
{
    AboutDialog *dlg = new AboutDialog(m_liteApp->mainWindow());
    dlg->exec();
}

