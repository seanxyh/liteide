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
    m_process = new ProcessEx(this);

    m_goroot = m_liteApp->settings()->value("golangdoc/goroot").toString();

    m_widget = new QWidget;
    m_recentModel = new QStringListModel(this);

    m_recentPackages = m_liteApp->settings()->value("golangdoc/RecentPackages").toStringList();
    m_recentModel->setStringList(m_recentPackages);

    m_recentListView = new QListView;
    m_recentListView->setEditTriggers(0);
    m_recentListView->setModel(m_recentModel);

    m_findComboBox = new QComboBox;
    m_findComboBox->setEditable(true);
    m_findComboBox->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    m_findComboBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);

    m_findResultLabel = new QLabel;

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(4);

    QHBoxLayout *findLayout = new QHBoxLayout;
    findLayout->setMargin(0);
    findLayout->addWidget(new QLabel(tr("Find:")));
    findLayout->addWidget(m_findComboBox);

    QToolButton *clearButton = new QToolButton;
    clearButton->setText(tr("Clear"));
    connect(clearButton,SIGNAL(clicked()),this,SLOT(clearRecentPackages()));

    QHBoxLayout *recentHeadLayout = new QHBoxLayout;
    recentHeadLayout->addWidget(new QLabel(tr("Recent:")));
    recentHeadLayout->addStretch(1);
    recentHeadLayout->addWidget(clearButton);

    mainLayout->addLayout(findLayout);
    mainLayout->addLayout(recentHeadLayout);
    mainLayout->addWidget(m_recentListView);
    mainLayout->addWidget(m_findResultLabel);
    m_widget->setLayout(mainLayout);

    BrowserEditorManager *browserManager = LiteApi::findExtensionObject<BrowserEditorManager*>(app,"LiteApi.BrowserEditorManager");
    if (browserManager) {
        m_docBrowser = new DocumentBrowser(app);
        connect(m_docBrowser->browser(),SIGNAL(anchorClicked(QUrl)),this,SLOT(anchorClicked(QUrl)));
        m_browserAct = browserManager->addBrowser(m_docBrowser);
        QMenu *menu = m_liteApp->actionManager()->loadMenu("view");
        if (menu) {
            menu->addAction(m_browserAct);
        }
    }
    m_liteApp->dockManager()->addDock(m_widget,tr("GolangDoc"),Qt::LeftDockWidgetArea);

    QString path = m_liteApp->resourcePath()+"/golangdoc/package.html";
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        m_templateData = file.readAll();
        file.close();
    }

    connect(m_findComboBox,SIGNAL(activated(QString)),this,SLOT(findPackage(QString)));
    connect(m_process,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(findOutput(QByteArray,bool)));
    connect(m_process,SIGNAL(extFinish(bool,int,QString)),this,SLOT(findFinish(bool,int,QString)));
    connect(m_recentListView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClockedLog(QModelIndex)));
}

GolangDoc::~GolangDoc()
{
    m_liteApp->settings()->setValue("golangdoc/goroot",m_goroot);
    m_liteApp->settings()->setValue("golangdoc/RecentPackages",m_recentPackages);
    if (m_docBrowser) {
        delete m_docBrowser;
    }
    delete m_widget;
}

void GolangDoc::findPackage(QString package)
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
    m_findData.clear();
    QStringList args;
    args << "-html=true" << package;
    m_process->start(cmd,args);
}

void GolangDoc::findOutput(QByteArray data,bool bStderr)
{
    if (bStderr) {
        return;
    }
    m_findData.append(data);
}

void GolangDoc::findFinish(bool error,int code,QString /*msg*/)
{
    if (!error && code == 0 && m_docBrowser != 0) {
        QString data = m_templateData;
        if (!data.isEmpty()) {
            data.replace("{title}",m_findText);
            data.replace("{data}",m_findData);
            m_docBrowser->browser()->setHtml(data);
        } else {
            m_docBrowser->browser()->setHtml(m_findData);
        }
        m_findResultLabel->setText(QString("GOROOT=%1\nFind package %2").arg(m_goroot).arg(m_findText));
        BrowserEditorManager *browserManager = LiteApi::findExtensionObject<BrowserEditorManager*>(m_liteApp,"LiteApi.BrowserEditorManager");
        if (browserManager) {
            browserManager->setActive(m_docBrowser);
        }
        m_recentPackages.append(m_findText);
        m_recentPackages.removeDuplicates();
        m_recentModel->setStringList(m_recentPackages);
    } else {
        int index = m_findComboBox->findText(m_findText);
        if (index != -1) {
            m_findComboBox->removeItem(index);
        }
        m_findResultLabel->setText(QString("GOROOT=%1\nNot find package %2").arg(m_goroot).arg(m_findText));
    }
}

void GolangDoc::anchorClicked(QUrl url)
{
    if (!url.encodedHost().isEmpty()) {
        return;
    }
    QString path = url.encodedPath();
    if (!path.isEmpty() && !m_goroot.isEmpty()) {
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
        }
    }
}

void GolangDoc::clearRecentPackages()
{
    m_recentPackages.clear();
    m_recentModel->setStringList(m_recentPackages);
}

void GolangDoc::doubleClockedLog(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    QString text = m_recentModel->data(index,Qt::DisplayRole).toString();
    if (!text.isEmpty()) {
        findPackage(text);
    }
}
