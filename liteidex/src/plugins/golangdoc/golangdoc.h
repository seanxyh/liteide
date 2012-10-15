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

#include "liteapi/liteapi.h"
#include "litebuildapi/litebuildapi.h"
#include "liteenvapi/liteenvapi.h"
#include "liteeditorapi/liteeditorapi.h"
#include "golangdocapi/golangdocapi.h"
#include "qtc_editutil/filterlineedit.h"

#include <QUrl>
#include <QModelIndex>

class QListView;
class QLineEdit;
class QStringListModel;
class QComboBox;
class QPushButton;
class ProcessEx;
class DocumentBrowser;
class QSortFilterProxyModel;
class GolangApi;

class GolangDoc : public LiteApi::IGolangDoc
{
    Q_OBJECT
public:
    explicit GolangDoc(LiteApi::IApplication *app, QObject *parent = 0);
    ~GolangDoc();    
public slots:
    virtual void openUrl(const QUrl &url);
    virtual void activeBrowser();
public slots:
    void editorFindDoc();
    void editorCreated(LiteApi::IEditor *editor);
    void loadApi();
    void currentEnvChanged(LiteApi::IEnv*);
    void listCmd();
    void listPkg();
    void findPackage(QString name = QString());
    void findOutput(QByteArray,bool);
    void findFinish(bool,int,QString);
    void godocFindPackage(QString name);
    void godocOutput(QByteArray,bool);
    void godocFinish(bool,int,QString);
    void doubleClickListView(QModelIndex);
    void findTag(const QString &tag);
    void highlighted(const QUrl &url);
protected:
    QUrl parserUrl(const QUrl &url);
    void openUrlList(const QUrl &url);
    void openUrlFind(const QUrl &url);
    void openUrlPdoc(const QUrl &url);
    void openUrlFile(const QUrl &url);
    void updateTextDoc(const QUrl &url, const QByteArray &ba, const QString &header);
    void updateHtmlDoc(const QUrl &url, const QByteArray &ba, const QString &header = QString(), bool toNav = true);
protected:
    LiteApi::IApplication   *m_liteApp;
    QUrl    m_openUrl;
    QUrl    m_lastUrl;
    QString m_lastPath;
    QWidget *m_widget;
    DocumentBrowser *m_docBrowser;
    QComboBox *m_godocFindComboBox;
    QStringListModel *m_findResultModel;
    QSortFilterProxyModel *m_findFilterModel;
    QListView *m_findResultListView;
    Utils::FilterLineEdit *m_findEdit;
    ProcessEx  *m_findProcess;
    ProcessEx  *m_godocProcess;
    QAction *m_browserAct;
    QString m_goroot;
    QByteArray  m_godocData;
    QByteArray  m_findData;
    QString  m_templateData;
    LiteApi::IEnvManager *m_envManager;
    GolangApi  *m_golangApi;
    QString m_godocCmd;
    QString m_findCmd;
    QStringList m_targetList;
    QMap<QString,QString> m_pathFileMap;
    QAction *m_toolAct;
    QAction *m_findDocAct;
};

#endif // GOLANGDOC_H
