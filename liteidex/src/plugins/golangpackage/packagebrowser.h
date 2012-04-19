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
// Module: packagebrowser.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-4-19
// $Id: packagebrowser.h,v 1.0 2012-4-19 visualfc Exp $

#ifndef PACKAGEBROWSER_H
#define PACKAGEBROWSER_H

#include "liteapi/liteapi.h"
#include "gotool.h"

class QTreeView;

struct Package {
    QString dir;
    QString importPath;
    QString Name;
    QString doc;
    QString target;
    bool    goroot;
    bool    standard;
};

class QStandardItemModel;
class PackageBrowser : public QObject
{
    Q_OBJECT
public:
    explicit PackageBrowser(LiteApi::IApplication *app, QObject *parent = 0);
signals:
    
public slots:
    void reload();
    void setupGopath();
    void editPackage();
    void finished(int,QProcess::ExitStatus);
    void customContextMenuRequested(QPoint);
protected:
    LiteApi::IApplication *m_liteApp;
    QWidget      *m_widget;
    QTreeView    *m_treeView;
    QStandardItemModel *m_model;
    GoTool       *m_goTool;
    QStringList   m_gopathList;
    bool         m_groupByPath;
    bool         m_hideStandard;
    QMenu       *m_contextMenu;
    QAction     *m_setupGopathAct;
    QAction     *m_editPackageAct;
};

#endif // PACKAGEBROWSER_H
