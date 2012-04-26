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
// Module: packagebrowser.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-4-19
// $Id: packagebrowser.cpp,v 1.0 2012-4-19 visualfc Exp $

#include "packagebrowser.h"
#include "setupgopathdialog.h"
#include "liteenvapi/liteenvapi.h"
#include "golangdocapi/golangdocapi.h"
#include "qjson/include/QJson/Parser"
#include "packageproject.h"
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDir>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QUrl>
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

PackageBrowser::PackageBrowser(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app)
{
    m_goTool = new GoTool(m_liteApp,this);
    m_widget = new QWidget;
    m_groupByPath = true;
    m_hideStandard = false;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    m_treeView = new SymbolTreeView;
    m_model = new QStandardItemModel(this);
    m_treeView->setHeaderHidden(true);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeView->setModel(m_model);
    m_treeView->setEditTriggers(QTreeView::NoEditTriggers);
    layout->addWidget(m_treeView);
    m_widget->setLayout(layout);

    m_contextMenu = new QMenu(m_widget);
    m_reloadAct = new QAction(tr("Reload"),this);
    m_setupGopathAct = new QAction(tr("Setup GOPATH"),this);
    m_godocAct = new QAction(tr("View Godoc"),this);
    m_editPackageAct = new QAction(tr("Open Package Poject"),this);
    m_contextMenu->addAction(m_reloadAct);
    m_contextMenu->addAction(m_setupGopathAct);
    m_contextMenu->addAction(m_godocAct);
    m_contextMenu->addAction(m_editPackageAct);


    m_liteApp->dockManager()->addDock(m_widget,tr("Package Browser"));
    connect(m_goTool,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finished(int,QProcess::ExitStatus)));
    connect(m_treeView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenuRequested(QPoint)));
    connect(m_reloadAct,SIGNAL(triggered()),this,SLOT(reload()));
    connect(m_setupGopathAct,SIGNAL(triggered()),this,SLOT(setupGopath()));
    connect(m_godocAct,SIGNAL(triggered()),this,SLOT(viewGodoc()));
    connect(m_editPackageAct,SIGNAL(triggered()),this,SLOT(editPackage()));

    QAction *act = new QAction(QIcon(":/images/gopath.png"),tr("GOPATH Setup"),this);
    connect(act,SIGNAL(triggered()),this,SLOT(setupGopath()));

    QToolBar *toolBar = m_liteApp->actionManager()->loadToolBar("toolbar/nav");
    if (toolBar) {
        toolBar->addAction(act);
    }

    LiteApi::IEnvManager *env = LiteApi::getEnvManager(m_liteApp);
    if (env) {
        connect(env,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(reload()));
    }

    m_model->appendRow(new QStandardItem(tr("Loading go package ...")));

    reload();
}

void PackageBrowser::reload()
{
    m_goTool->reloadEnv();
    m_gopathList = m_goTool->gopath();
    m_goTool->start(QStringList() << "list" << "-json" << "...");
}

void PackageBrowser::setupGopath()
{
    SetupGopathDialog *dlg = new SetupGopathDialog(m_liteApp->mainWindow());
    dlg->setSysPathList(m_goTool->sysGopath());
    dlg->setLitePathList(m_goTool->liteGopath());
    if (dlg->exec() == QDialog::Accepted) {
        m_goTool->setLiteGopath(dlg->litePathList());
        reload();
    }
}

void PackageBrowser::viewGodoc()
{
    QModelIndex index = m_treeView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    QVariant json = m_model->data(index,Qt::UserRole+1);
    if (json.isNull()) {
        return;
    }
    QString pkg = json.toMap().value("ImportPath").toString();
    if (!pkg.isEmpty()) {
        LiteApi::IGolangDoc *doc = LiteApi::getGolangDoc(m_liteApp);
        if (doc) {
            doc->openUrl(QUrl(QString("pdoc:%1").arg(pkg)));
            doc->activeBrowser();
        }
    }
}

void PackageBrowser::editPackage()
{
    QModelIndex index = m_treeView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    QVariant json = m_model->data(index,Qt::UserRole+1);
    if (json.isNull()) {
        return;
    }
    QDir dir(json.toMap().value("Dir").toString());
    if (dir.exists()) {
        PackageProject *proj = new PackageProject(m_liteApp);
        proj->setJson(json.toMap());
        m_liteApp->projectManager()->setCurrentProject(proj);;
        m_liteApp->fileManager()->addRecentFile(dir.path(),"gopkg");
    }
}

void PackageBrowser::customContextMenuRequested(QPoint pos)
{
    QMenu *contextMenu = m_contextMenu;
/*
    if (node->isDir()) {
        contextMenu = m_folderMenu;
    } else {
        contextMenu = m_fileMenu;
    }
*/
    if (contextMenu && contextMenu->actions().count() > 0) {
        contextMenu->popup(m_treeView->mapToGlobal(pos));
    }
}

void PackageBrowser::finished(int code,QProcess::ExitStatus)
{
    QByteArray jsonData;

    //save state
    SymbolTreeState state;
    m_treeView->saveState(&state);

    m_model->clear();

    QMap<QString,QStandardItem*> cmdMap;
    QMap<QString,QStandardItem*> pkgMap;
    foreach (QString path, m_gopathList) {
        QStandardItem *item = new QStandardItem(QDir::toNativeSeparators(path));
        QStandardItem *cmd = new QStandardItem("cmd");
        item->appendRow(cmd);
        QStandardItem *pkg = new QStandardItem("pkg");
        item->appendRow(pkg);
        m_model->appendRow(item);
        cmdMap.insert(QDir(path).path(),cmd);
        pkgMap.insert(QDir(path).path(),pkg);
        m_treeView->expand(m_model->indexFromItem(item));
    }

    foreach(QByteArray line, m_goTool->stdOutputData().split('\n')) {
        jsonData.append(line);
        if (line.trimmed() == "}") {
            QJson::Parser parser;
            bool ok = false;
            QVariant json = parser.parse(jsonData, &ok).toMap();
            QVariantMap jsonMap = json.toMap();
            QString root = QDir(jsonMap.value("Root").toString()).path();
            QStandardItem *parent = 0;
            if (jsonMap.value("Name").toString() == "main") {
                parent = cmdMap.value(root);
            } else {
                parent = pkgMap.value(root);
            }
            if (parent) {
                QStandardItem *item = new QStandardItem(jsonMap.value("ImportPath").toString());
                item->setData(json,Qt::UserRole+1);
                item->setToolTip(jsonMap.value("Doc").toString());
                parent->appendRow(item);
            }
            jsonData.clear();
        }
    }

    //load state
    m_treeView->loadState(m_model,&state);
}
