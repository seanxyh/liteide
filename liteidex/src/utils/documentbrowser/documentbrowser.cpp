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
// Module: documentbrowser.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-7
// $Id: documentbrowser.cpp,v 1.0 2011-7-26 visualfc Exp $

#include "documentbrowser.h"
#include "extension/extension.h"

#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QComboBox>
#include <QToolButton>
#include <QCheckBox>
#include <QAction>
#include <QRegExp>
#include <QFile>
#include <QFileInfo>
#include <QDir>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

DocumentBrowser::DocumentBrowser(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IDocumentBrowser(parent),
    m_liteApp(app)
{
    m_extension = new Extension;
    m_widget = new QWidget;

    m_textBrowser = new QTextBrowser;

    m_statusBar = new QStatusBar;

    m_findComboBox = new QComboBox;
    m_findComboBox->setEditable(true);

    m_findNextAct = new QAction(tr("Next"),this);
    m_findNextAct->setShortcut(QKeySequence::FindNext);
    m_findNextAct->setToolTip(tr("FindNext\t")+QKeySequence(QKeySequence::FindNext).toString());
    m_findPrevAct = new QAction(tr("Prev"),this);
    m_findPrevAct->setShortcut(QKeySequence::FindPrevious);
    m_findPrevAct->setToolTip(tr("FindPrev\t")+QKeySequence(QKeySequence::FindPrevious).toString());

    QToolButton *findNext = new QToolButton;
    findNext->setDefaultAction(m_findNextAct);
    QToolButton *findPrev = new QToolButton;
    findPrev->setDefaultAction(m_findPrevAct);
    m_matchCaseCheckBox = new QCheckBox(tr("MatchCase"));
    m_matchWordCheckBox = new QCheckBox(tr("MatchWord"));
    m_useRegexCheckBox = new QCheckBox(tr("Regex"));

    m_statusBar->addPermanentWidget(m_findComboBox);
    m_statusBar->addPermanentWidget(findNext);
    m_statusBar->addPermanentWidget(findPrev);
    m_statusBar->addPermanentWidget(m_matchCaseCheckBox);
    m_statusBar->addPermanentWidget(m_matchWordCheckBox);
    m_statusBar->addPermanentWidget(m_useRegexCheckBox);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(m_textBrowser);
    mainLayout->addWidget(m_statusBar);
    m_widget->setLayout(mainLayout);

    connect(m_textBrowser,SIGNAL(highlighted(QUrl)),this,SLOT(highlighted(QUrl)));
    connect(m_findComboBox,SIGNAL(activated(QString)),this,SLOT(activatedFindText(QString)));
    connect(m_findNextAct,SIGNAL(triggered()),this,SLOT(findNext()));
    connect(m_findPrevAct,SIGNAL(triggered()),this,SLOT(findPrev()));

    m_liteApp->settings()->beginGroup("documentbrowser");
    m_matchWordCheckBox->setChecked(m_liteApp->settings()->value("matchword",true).toBool());
    m_matchCaseCheckBox->setChecked(m_liteApp->settings()->value("matchcase",true).toBool());
    m_useRegexCheckBox->setChecked(m_liteApp->settings()->value("useregex",false).toBool());
    m_liteApp->settings()->endGroup();

    m_extension->addObject("LiteApi.IDocumentBrowser",this);
}

DocumentBrowser::~DocumentBrowser()
{
    m_liteApp->settings()->beginGroup("documentbrowser");
    m_liteApp->settings()->setValue("matchword",m_matchWordCheckBox->isChecked());
    m_liteApp->settings()->setValue("matchcase",m_matchCaseCheckBox->isChecked());
    m_liteApp->settings()->setValue("useregex",m_useRegexCheckBox->isChecked());
    m_liteApp->settings()->endGroup();

    if (m_widget) {
        delete m_widget;
    }
    if (m_extension) {
        delete m_extension;
    }
}

LiteApi::IExtension *DocumentBrowser::extension()
{
    return m_extension;
}

bool DocumentBrowser::open(const QString &fileName,const QString &mimeType)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    m_mimeType = mimeType;
    QFileInfo info(fileName);
    QString htmlType = m_liteApp->mimeTypeManager()->findFileMimeType(fileName);
    m_name = info.fileName();
    m_fileName = QDir::toNativeSeparators(fileName);
    if (htmlType == "text/html") {
        m_textBrowser->setSource(QUrl::fromLocalFile(fileName));
    } else {
        QByteArray data = file.readAll();
        m_textBrowser->setText(QString::fromUtf8(data,data.size()));
    }
    file.close();
    return true;
}

QWidget *DocumentBrowser::widget()
{
    return m_widget;
}

QString DocumentBrowser::name() const
{
    return m_name;
}

QString DocumentBrowser::fileName() const
{
    return m_fileName;
}

QString DocumentBrowser::mimeType() const
{
    return m_mimeType;
}

void DocumentBrowser::setName(const QString &t)
{
    m_name = t;
}

QTextBrowser *DocumentBrowser::browser()
{
    return m_textBrowser;
}

void DocumentBrowser::highlighted(QUrl url)
{
    m_statusBar->showMessage(url.toString());
}

void DocumentBrowser::activatedFindText(QString)
{
    findText(false);
}

void DocumentBrowser::findNext()
{
    findText(false);
}

void DocumentBrowser::findPrev()
{
    findText(true);
}

bool DocumentBrowser::findText(bool findBackward)
{
    QString text = m_findComboBox->currentText();
    if (text.isEmpty()) {
        return false;
    }
    QTextCursor cursor = m_textBrowser->textCursor();
    QTextDocument::FindFlags flags = 0;
    if (findBackward) {
        flags |= QTextDocument::FindBackward;
    }
    if (m_matchCaseCheckBox->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (m_matchWordCheckBox->isChecked()) {
        flags |= QTextDocument::FindWholeWords;
    }
    QTextCursor find;
    if (m_useRegexCheckBox->isChecked()) {
        find = m_textBrowser->document()->find(QRegExp(text),cursor,flags);
    } else {
        find = m_textBrowser->document()->find(text,cursor,flags);
    }
    if (!find.isNull()) {
        m_textBrowser->setTextCursor(find);
        m_statusBar->showMessage(QString("find %1").arg(find.selectedText()));
        return true;
    }
    m_statusBar->showMessage("end find");
    return false;
}
