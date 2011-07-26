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
// Module: golangdoc.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-7
// $Id: golangdoc.h,v 1.0 2011-7-7 visualfc Exp $

#ifndef GOLANGDOC_H
#define GOLANGDOC_H

#include "liteapi.h"
#include "litebuildapi/litebuildapi.h"
#include "liteenvapi/liteenvapi.h"
#include <QUrl>
#include <QModelIndex>
#include <QCache>

class QLabel;
class QListView;
class QStringListModel;
class QComboBox;
class QPushButton;
class ProcessEx;
class DocumentBrowser;
class GolangDoc : public QObject
{
    Q_OBJECT
public:
    explicit GolangDoc(LiteApi::IApplication *app, QObject *parent = 0);
    ~GolangDoc();
signals:

public slots:
    void currentEnvChanged(LiteApi::IEnv*);
    void listCmd();
    void listPkg();
    void findPackage(QString name = QString());
    void findOutput(QByteArray,bool);
    void findFinish(bool,int,QString);
    void godocPackage(QString);
    void godocOutput(QByteArray,bool);
    void godocFinish(bool,int,QString);
    void anchorClicked(QUrl);
    void doubleClickListView(QModelIndex);
protected:
    void activeBrowser();
protected:
    LiteApi::IApplication   *m_liteApp;
    QWidget *m_widget;
    QStringListModel *m_findResultModel;
    QListView *m_findResultListView;
    QComboBox *m_findComboBox;
    ProcessEx  *m_findProcess;
    ProcessEx  *m_godocProcess;
    DocumentBrowser *m_docBrowser;
    QAction *m_browserAct;
    QString m_findText;
    QString m_goroot;
    QByteArray  m_godocData;
    QByteArray  m_findData;
    QString  m_templateData;
    QAction *m_listPkgAct;
    QAction *m_listCmdAct;
    QAction *m_findAct;
    QMenu *m_findMenu;
    LiteApi::IEnvManager *m_envManager;
    QCache<QString,QString> m_htmlCache;
    QString m_godocCmd;
    QString m_findCmd;
};

#endif // GOLANGDOC_H
