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
#include "liteapi/litefindobj.h"
#include "litebuildapi/litebuildapi.h"
#include "processex/processex.h"
#include "fileutil/fileutil.h"
#include "documentbrowser/documentbrowser.h"

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
#include <QTextCodec>
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

GolangDoc::GolangDoc(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app)
{
    m_findProcess = new ProcessEx(this);
    m_godocProcess = new ProcessEx(this);

    m_widget = new QWidget;
    m_findResultModel = new QStringListModel(this);

    m_findResultListView = new QListView;
    m_findResultListView->setEditTriggers(0);
    m_findResultListView->setModel(m_findResultModel);

    m_findComboBox = new QComboBox;
    m_findComboBox->setEditable(true);
    m_findComboBox->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    m_findComboBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);

    m_findAct = new QAction(tr("Find"),this);
    m_listPkgAct = new QAction(tr("List \"src/pkg\""),this);
    m_listCmdAct = new QAction(tr("List \"src/cmd\""),this);

    m_findMenu = new QMenu(tr("Find"));
    m_findMenu->addAction(m_findAct);
    m_findMenu->addSeparator();
    m_findMenu->addAction(m_listPkgAct);
    m_findMenu->addAction(m_listCmdAct);

    QToolButton *findBtn = new QToolButton;
    findBtn->setPopupMode(QToolButton::MenuButtonPopup);
    findBtn->setDefaultAction(m_findAct);
    findBtn->setMenu(m_findMenu);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(4);

    QHBoxLayout *findLayout = new QHBoxLayout;
    findLayout->setMargin(0);
    findLayout->addWidget(m_findComboBox);
    findLayout->addWidget(findBtn);

    mainLayout->addLayout(findLayout);
    mainLayout->addWidget(m_findResultListView);
    m_widget->setLayout(mainLayout);

    m_liteApp->dockManager()->addDock(m_widget,tr("GolangDoc"),Qt::LeftDockWidgetArea);

    m_docBrowser = new DocumentBrowser(m_liteApp,this);
    m_docBrowser->setName(tr("GodocBrowser"));
    m_docBrowser->browser()->setOpenLinks(false);

    m_browserAct = m_liteApp->editorManager()->registerBrowser(m_docBrowser);
    QMenu *menu = m_liteApp->actionManager()->loadMenu("view");
    if (menu) {
        menu->addAction(m_browserAct);
    }

    QString path = m_liteApp->resourcePath()+"/golangdoc/godoc.html";
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        m_templateData = file.readAll();
        file.close();
    }

    connect(m_docBrowser->browser(),SIGNAL(anchorClicked(QUrl)),this,SLOT(openUrl(QUrl)));
    connect(m_findComboBox,SIGNAL(activated(QString)),this,SLOT(findPackage(QString)));
    connect(m_godocProcess,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(godocOutput(QByteArray,bool)));
    connect(m_godocProcess,SIGNAL(extFinish(bool,int,QString)),this,SLOT(godocFinish(bool,int,QString)));
    connect(m_findProcess,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(findOutput(QByteArray,bool)));
    connect(m_findProcess,SIGNAL(extFinish(bool,int,QString)),this,SLOT(findFinish(bool,int,QString)));
    connect(m_findResultListView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickListView(QModelIndex)));
    connect(m_findAct,SIGNAL(triggered()),this,SLOT(findPackage()));
    connect(m_listPkgAct,SIGNAL(triggered()),this,SLOT(listPkg()));
    connect(m_listCmdAct,SIGNAL(triggered()),this,SLOT(listCmd()));

    m_envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
    if (m_envManager) {
        connect(m_envManager,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(currentEnvChanged(LiteApi::IEnv*)));
        currentEnvChanged(m_envManager->currentEnv());
    }

    QString data = m_templateData;
    if (!data.isEmpty()) {
        data.replace("{goroot}",m_goroot);
        data.replace("{title}","");
        data.replace("{content}","");
        m_docBrowser->browser()->setHtml(data);
    }
}

GolangDoc::~GolangDoc()
{
    m_liteApp->settings()->setValue("golangdoc/goroot",m_goroot);
    if (m_docBrowser) {
        delete m_docBrowser;
    }
    delete m_findMenu;
    delete m_widget;
}

void GolangDoc::currentEnvChanged(LiteApi::IEnv*)
{
    QProcessEnvironment env = m_envManager->currentEnvironment();
    QString goroot = env.value("GOROOT");
    QString gobin = env.value("GOBIN");
    if (!goroot.isEmpty() && gobin.isEmpty()) {
        gobin = goroot+"/bin";
    }
    QString godoc = FileUtil::findExecute(gobin+"/godoc");
    if (godoc.isEmpty()) {
        godoc = FileUtil::lookPath("godoc",env,true);
    }
    QString find = FileUtil::findExecute(m_liteApp->applicationPath()+"/godocview");
    if (find.isEmpty()) {
        find = FileUtil::lookPath("godocview",env,true);
    }

    if (m_goroot != goroot) {
        m_htmlCache.clear();
    }
    m_goroot = goroot;
    m_godocCmd = godoc;
    m_findCmd = find;

    m_findProcess->setEnvironment(env.toStringList());
    m_godocProcess->setEnvironment(env.toStringList());
}

void GolangDoc::activeBrowser()
{
    m_liteApp->editorManager()->activeBrowser(m_docBrowser);
}

void GolangDoc::listPkg()
{
    if (m_findCmd.isEmpty()) {
        return;
    }
    QStringList args;
    args << "-mode=lite" << "-list=src/pkg";
    m_findData.clear();
    m_findProcess->start(m_findCmd,args);
}

void GolangDoc::listCmd()
{
    if (m_findCmd.isEmpty()) {
        return;
    }
    QStringList args;
    args << "-mode=lite" << "-list=src/cmd";
    m_findData.clear();
    m_findProcess->start(m_findCmd,args);
}


void GolangDoc::findPackage(QString pkgname)
{
    if (pkgname.isEmpty()) {
        pkgname = m_findComboBox->currentText();
    }
    if (pkgname.isEmpty()) {
        return;
    }
    if (m_findCmd.isEmpty()) {
        return;
    }
    QStringList args;
    args << "-mode=lite" << "-find" << pkgname;
    m_findData.clear();
    m_findProcess->start(m_findCmd,args);
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
        } else if (array.size() >= 1 && array.at(0) == "$list") {
            array.removeFirst();
            m_findResultModel->setStringList(array);
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

    QString *html = m_htmlCache[m_findText];
    if (html && !html->isEmpty()) {        
        m_docBrowser->browser()->setHtml(*html);
        activeBrowser();
        return;
    }
    if (m_godocCmd.isEmpty()) {
        return;
    }
    m_godocData.clear();
    QStringList args;
    args << "-html=true" << package.split(" ");
    if (m_godocProcess->isRuning()) {
        m_godocProcess->waitForFinished(200);
    }
    m_godocProcess->start(m_godocCmd,args);
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
        QString *data = new QString(m_templateData);
        data->replace("{goroot}",m_goroot);
        data->replace("{title}","Package "+m_findText);
        data->replace("{content}",m_godocData);
        m_htmlCache.insert(m_findText,data);
        m_docBrowser->browser()->setHtml(*data);
        activeBrowser();
    } else {
        int index = m_findComboBox->findText(m_findText);
        if (index != -1) {
            m_findComboBox->removeItem(index);
        }
    }
}

void GolangDoc::openUrl(QUrl url)
{
    if (!url.isRelative() &&url.scheme() != "file") {
        QDesktopServices::openUrl(url);
        return;
    }
    QString path = url.encodedPath();
    QFileInfo info(url.toLocalFile());        
    if (url.path() == "/src/pkg/" || url.path() == "/src/cmd/") {
        m_findText = url.path();
        QString *html = m_htmlCache[m_findText];
        if (html && !html->isEmpty()) {
            m_docBrowser->browser()->setHtml(*html);
            return;
        }
        if (m_findCmd.isEmpty()) {
            return;
        }
        QStringList args;
        args << "-mode=html"<< "-list="+url.path();
        m_godocData.clear();
        m_godocProcess->start(m_findCmd,args);
    } else if (info.suffix() == "html") {
        QString name = m_goroot+"/doc/"+info.fileName();
        m_findText = name;
        QString *html = m_htmlCache[m_findText];
        if (html && !html->isEmpty()) {
            m_docBrowser->browser()->setHtml(*html);
            return;
        }
        QFile file(name);
        if (file.open(QIODevice::ReadOnly)) {
            QString *data = new QString(m_templateData);
            QByteArray r = file.readAll();
            file.close();
            data->replace("{goroot}",m_goroot);
            data->replace("{title}",name);
            data->replace("{content}",QString::fromUtf8(r,r.size()));
            m_htmlCache.insert(name,data);
            m_docBrowser->browser()->setHtml(*data);
        }
        return;
    } else if (info.suffix() == "go") {
        QString fileName = QDir(m_goroot).path()+path;
        if (!QFile::exists(fileName)) {
            godocPackage(url.toString());
            return;
        }
        LiteApi::IEditor *editor = m_liteApp->fileManager()->openEditor(fileName);
        if (editor) {
            editor->setReadOnly(true);
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
    } else if (info.suffix() == "pdf") {
        QString name = m_goroot+"/doc/"+info.fileName();
        if (QFile::exists(name)) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(name));
            return;
        }
    }
    godocPackage(url.toString());
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
