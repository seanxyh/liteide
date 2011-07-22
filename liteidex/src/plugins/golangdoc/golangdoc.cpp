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
// Module: golangdoc.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-7
// $Id: golangdoc.cpp,v 1.0 2011-7-7 visualfc Exp $

#include "golangdoc.h"
#include "documentbrowser.h"
#include "liteapi/litefindobj.h"
#include "litebuildapi/litebuildapi.h"
#include "processex/processex.h"
#include "fileutil/fileutil.h"
#include "browsereditor/browsereditormanager.h"

#include <QListView>
#include <QStringListModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QFile>
#include <QDir>
#include <QTextBrowser>
#include <QUrl>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QGroupBox>
#include <QToolButton>
#include <QDebug>

GolangDoc::GolangDoc(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_docBrowser(0)
{
    m_findProcess = new ProcessEx(this);
    m_godocProcess = new ProcessEx(this);

    m_goroot = m_liteApp->settings()->value("golangdoc/goroot").toString();

    m_widget = new QWidget;
    m_findResultModel = new QStringListModel(this);

    m_findResultListView = new QListView;
    m_findResultListView->setEditTriggers(0);
    m_findResultListView->setModel(m_findResultModel);

    m_findComboBox = new QComboBox;
    m_findComboBox->setEditable(true);
    m_findComboBox->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    m_findComboBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(4);

    QHBoxLayout *findLayout = new QHBoxLayout;
    findLayout->setMargin(0);
    findLayout->addWidget(new QLabel(tr("Find:")));
    findLayout->addWidget(m_findComboBox);

    mainLayout->addLayout(findLayout);
    mainLayout->addWidget(m_findResultListView);
    m_widget->setLayout(mainLayout);

    BrowserEditorManager *browserManager = LiteApi::findExtensionObject<BrowserEditorManager*>(app,"LiteApi.BrowserEditorManager");
    if (browserManager) {
        m_docBrowser = new DocumentBrowser(app);
        m_docBrowser->browser()->setOpenLinks(false);
        m_docBrowser->browser()->setSearchPaths(QStringList() << m_liteApp->resourcePath());
        connect(m_docBrowser->browser(),SIGNAL(anchorClicked(QUrl)),this,SLOT(anchorClicked(QUrl)));
        m_browserAct = browserManager->addBrowser(m_docBrowser);
        QMenu *menu = m_liteApp->actionManager()->loadMenu("view");
        if (menu) {
            menu->addAction(m_browserAct);
        }
    }
    m_liteApp->dockManager()->addDock(m_widget,tr("GolangDoc"),Qt::LeftDockWidgetArea);

    QString path = m_liteApp->resourcePath()+"/golangdoc/godoc.html";
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        m_templateData = file.readAll();
        file.close();
    }
    QString data = m_templateData;
    if (!data.isEmpty()) {
        data.replace("{title}","");
        data.replace("{content}","");
        m_docBrowser->browser()->setHtml(data);
    }

    connect(m_findComboBox,SIGNAL(activated(QString)),this,SLOT(findPackage(QString)));
    connect(m_godocProcess,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(godocOutput(QByteArray,bool)));
    connect(m_godocProcess,SIGNAL(extFinish(bool,int,QString)),this,SLOT(godocFinish(bool,int,QString)));
    connect(m_findProcess,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(findOutput(QByteArray,bool)));
    connect(m_findProcess,SIGNAL(extFinish(bool,int,QString)),this,SLOT(findFinish(bool,int,QString)));
    connect(m_findResultListView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickListView(QModelIndex)));
}

GolangDoc::~GolangDoc()
{
    m_liteApp->settings()->setValue("golangdoc/goroot",m_goroot);
    if (m_docBrowser) {
        delete m_docBrowser;
    }
    delete m_widget;
}



void GolangDoc::findPackage(QString pkgname)
{
    LiteApi::IBuildManager *manager = LiteApi::findExtensionObject<LiteApi::IBuildManager*>(m_liteApp,"LiteApi.IBuildManager");
    if (manager) {
        LiteApi::IBuild *build = manager->currentBuild();
        if (build) {
            m_findProcess->setEnvironment(build->currentEnv().toStringList());
        } else {
            m_findProcess->setEnvironment(QProcess::systemEnvironment());
        }
    }
    QString cmd = FileUtil::findExecute(m_liteApp->applicationPath()+"/godocview");
    if (cmd.isEmpty()) {
        return;
    }
    QStringList args;
    args << "-mode=lite" << "-find" << pkgname;
    m_findData.clear();
    m_findProcess->start(cmd,args);
}

void GolangDoc::findOutput(QByteArray data,bool bStderr)
{
    if (bStderr) {
        return;
    }
    m_findData.append(data);
}

void GolangDoc::findFinish(bool error,int code,QString msg)
{
    if (!error && code == 0) {
        QStringList array = QString(m_findData.trimmed()).split(',');
        if (array.size() >= 2 && array.at(0) == "$find") {
            array.removeFirst();
            QString best = array.at(0);
            if (best.isEmpty()) {
                array.removeFirst();
            } else {
                godocPackage(best);
            }
            if (array.isEmpty()) {
                m_findResultModel->setStringList(QStringList() << "<not find>");
            } else {
                m_findResultModel->setStringList(array);
            }
        }
    } else {
        m_findResultModel->setStringList(QStringList() << "<error>");
    }
}

void GolangDoc::godocPackage(QString package)
{
    m_findText = package;
    if (package.isEmpty()) {
        return;
    }
    QString cmd;
    LiteApi::IBuildManager *manager = LiteApi::findExtensionObject<LiteApi::IBuildManager*>(m_liteApp,"LiteApi.IBuildManager");
    if (manager) {
        LiteApi::IBuild *build = manager->currentBuild();
        if (build) {
            m_goroot = build->currentEnv().value("GOROOT");
            QString gobin = build->currentEnv().value("GOBIN");
            if (gobin.isEmpty()) {
                QString goroot = build->currentEnv().value("GOROOT");
                gobin = goroot+"/bin";
            }
            cmd = FileUtil::findExecute(gobin+"/godoc");
            if (cmd.isEmpty()) {
                cmd = FileUtil::lookPath("godoc",build->currentEnv(),true);
            }
        }
    }
    if (cmd.isEmpty()) {
        cmd = FileUtil::lookPath("godoc",QProcessEnvironment::systemEnvironment(),true);
    }
    if (m_goroot.isEmpty()) {
        m_goroot = QProcessEnvironment::systemEnvironment().value("GOROOT");
    }

    if (cmd.isEmpty()) {
        return;
    }
    m_godocData.clear();
    QStringList args;
    args << "-html=true" << package.split(" ");
    m_godocProcess->start(cmd,args);
}

void GolangDoc::godocOutput(QByteArray data,bool bStderr)
{
    if (bStderr) {
        return;
    }
    m_godocData.append(data);
}

void GolangDoc::godocFinish(bool error,int code,QString /*msg*/)
{
    if (!error && code == 0 && m_docBrowser != 0) {
        QString data = m_templateData;
        if (!data.isEmpty()) {
            data.replace("{title}","Package "+m_findText);
            data.replace("{content}",m_godocData);
            m_docBrowser->browser()->setHtml(data);
        } else {
            m_docBrowser->browser()->setHtml(m_godocData);
        }
        BrowserEditorManager *browserManager = LiteApi::findExtensionObject<BrowserEditorManager*>(m_liteApp,"LiteApi.BrowserEditorManager");
        if (browserManager) {
            browserManager->setActive(m_docBrowser);
        }
    } else {
        int index = m_findComboBox->findText(m_findText);
        if (index != -1) {
            m_findComboBox->removeItem(index);
        }
    }
}

void GolangDoc::anchorClicked(QUrl url)
{
    QString path = url.encodedPath();
    QFileInfo info(url.toLocalFile());
    if (url.path() == "/src/pkg/" || url.path() == "/src/cmd/") {
        QString cmd = FileUtil::findExecute(m_liteApp->applicationPath()+"/godocview");
        if (cmd.isEmpty()) {
            return;
        }
        QStringList args;
        args << "-subdir="+path<< "-mode=html" << "-find=*";
        m_godocData.clear();
        m_godocProcess->start(cmd,args);
    } else if (info.suffix() == "html") {
        QString name = m_goroot+"/doc/"+info.fileName();
        QFile file(name);
        if (file.open(QIODevice::ReadOnly)) {
            QString data = m_templateData;
            QByteArray r = file.readAll();
            data.replace("{title}",info.fileName());
            data.replace("{content}",r);
            m_docBrowser->browser()->setHtml(data);
        }
        return;
    } else if (info.suffix() == "go") {
        QString fileName = QDir(m_goroot).path()+path;
        LiteApi::IEditor *editor = m_liteApp->editorManager()->loadEditor(fileName);
        if (editor) {
            editor->setReadOnly(true);
            m_liteApp->editorManager()->setCurrentEditor(editor);
            QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
            if (ed && url.hasQueryItem("s")) {
                QStringList pos = url.queryItemValue("s").split(":");
                if (pos.length() == 2) {
                    bool ok = false;
                    int begin = pos.at(0).toInt(&ok);
                    if (ok) {
                        QTextCursor cur = ed->textCursor();
                        cur.setPosition(begin);
                        ed->setTextCursor(cur);
                        ed->centerCursor();
                    }
                }
            }
            return;
        }
    }
    if (url.scheme().isEmpty()) {
        godocPackage(url.toString());
    }
}

void GolangDoc::doubleClickListView(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    QString text = m_findResultModel->data(index,Qt::DisplayRole).toString();
    if (!text.isEmpty()) {
        godocPackage(text);
    }
}
