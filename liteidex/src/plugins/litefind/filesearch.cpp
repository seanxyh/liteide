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
// Module: filesearch.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-5-21
// $Id: filesearch.cpp,v 1.0 2012-5-21 visualfc Exp $

#include "filesearch.h"
#include <QFile>
#include <QTableWidget>
#include <QTextStream>
#include <QTextDocument>
#include <QRegExp>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDir>
#include <QDebug>

FindThread::FindThread(QObject *parent) :
    QThread(parent),
    useRegExp(true),
    matchWord(true),
    matchCase(true),
    findSub(true)
{
    qRegisterMetaType<FileSearchResult>("FileSearchResult");
}

void FindThread::findDir(const QRegExp &reg, const QString &path)
{
    QDir dir(path);
    if (!dir.exists()) {
        return;
    }
    QDir::Filters filter = QDir::Files|QDir::NoDotAndDotDot|QDir::NoSymLinks;
    if (findSub) {
        filter |= QDir::Dirs;
    }

    foreach(QFileInfo info, dir.entryInfoList(nameFilter,filter)) {
        if (info.isDir()) {
            findDir(reg,info.filePath());
        } else if (info.isFile()){
            findFile(reg,info.filePath());
        }
        if (!finding) {
            break;
        }
    }
}

void FindThread::findFile(const QRegExp &reg, const QString &fileName)
{
    QList<FileSearchResult> results;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QTextStream stream(&file);
    stream.setCodec("utf-8");
    QString line;
    int lineNr = 1;
    while (!stream.atEnd()) {
        line = stream.readLine();
        int pos = 0;
        while ((pos = reg.indexIn(line, pos)) != -1) {
            emit findResult(FileSearchResult(fileName, lineNr, line,
                                          pos, reg.matchedLength(),
                                          reg.capturedTexts()));
            pos += reg.matchedLength();
        }
        lineNr++;
        if (!finding) {
            break;
        }
    }
}


void FindThread::stop()
{
    finding = false;
    if (!this->wait(200)) {
        this->terminate();
    }
}

void FindThread::run()
{
    finding = true;
    QRegExp reg;
    if (matchWord) {
        reg.setPattern(QString::fromLatin1("\\b%1\\b").arg(findText));
    } else {
        reg.setPattern(findText);
    }
    reg.setCaseSensitivity(matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
    if (!useRegExp) {
        reg.setPatternSyntax(QRegExp::FixedString);
    }
    findDir(reg,findPath);
    finding = false;
}


FileSearch::FileSearch(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app)
{
    m_thread = new FindThread;
    m_tab = new QTabWidget;
    m_liteApp->outputManager()->addOutuput(m_tab,tr("FileSearch"));

    m_findWidget = new QWidget;

    QGridLayout *topLayout = new QGridLayout;

    m_findEdit = new QLineEdit;

    QHBoxLayout *optLayout = new QHBoxLayout;
    m_matchWordCheckBox = new QCheckBox(tr("Match word"));
    m_matchCaseCheckBox = new QCheckBox(tr("Match case"));
    m_useRegexCheckBox = new QCheckBox(tr("Regular expression"));
    m_findSubCheckBox = new QCheckBox(tr("Look in subfolders"));
    optLayout->addWidget(m_matchWordCheckBox);
    optLayout->addWidget(m_matchCaseCheckBox);
    optLayout->addWidget(m_useRegexCheckBox);
    optLayout->addWidget(m_findSubCheckBox);
    optLayout->addStretch();

    topLayout->addWidget(new QLabel(tr("Find Text:")),0,0);
    topLayout->addWidget(m_findEdit,0,1,1,3);
    topLayout->addWidget(new QLabel(tr("Option")),1,0);
    topLayout->addLayout(optLayout,1,1,1,3);

    QHBoxLayout *dirLayout = new QHBoxLayout;
    m_findPathEdit = new QLineEdit;
    QPushButton *browserBtn = new QPushButton(tr("Browser"));
    dirLayout->addWidget(m_findPathEdit);
    dirLayout->addWidget(browserBtn);

    topLayout->addWidget(new QLabel("Directory:"),2,0);
    topLayout->addLayout(dirLayout,2,1,1,3);

    m_filterCombo = new QComboBox;
    m_filterCombo->addItem("*.go");
    m_filterCombo->addItem("*.h;*.c;*.hpp;*.cpp");

    topLayout->addWidget(new QLabel(tr("Filter")),3,0);
    topLayout->addWidget(m_filterCombo,3,1,1,3);

    QHBoxLayout *findLayout = new QHBoxLayout;
    QPushButton *findBtn = new QPushButton(tr("Find"));
    QPushButton *stopBtn = new QPushButton(tr("Cancel"));
    findLayout->addWidget(findBtn);
    findLayout->addWidget(stopBtn);
    findLayout->addStretch(0);
    topLayout->addLayout(findLayout,4,3);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(topLayout);
    layout->addStretch(0);

    m_findWidget->setLayout(layout);

    m_tab->addTab(m_findWidget,tr("FindFiles"));

    m_findPathEdit->setText(QDir::homePath());

    connect(findBtn,SIGNAL(clicked()),this,SLOT(findInFiles()));
    connect(m_thread,SIGNAL(findResult(FileSearchResult)),this,SLOT(findResult(FileSearchResult)));
    connect(stopBtn,SIGNAL(clicked()),m_thread,SLOT(stop()));
}

FileSearch::~FileSearch()
{
    if (m_thread) {
        m_thread->stop();
        delete m_thread;
    }
    if (m_tab) {
        delete m_tab;
    }
}

QWidget *FileSearch::widget()
{
    return m_tab;
}

void FileSearch::setVisible(bool b)
{
    if (b) {
        LiteApi::IProject *proj = m_liteApp->projectManager()->currentProject();
        if (proj) {
            m_findPathEdit->setText(proj->projectInfo().value("PROJECTPATH"));
        }
    }
    if (b) {
        m_liteApp->outputManager()->setCurrentOutput(m_tab);
    } else {
        m_liteApp->outputManager()->setCurrentOutput(0);
    }
}

void FileSearch::findInFiles()
{
    if (m_thread->isRunning()) {
        m_thread->stop();
    }
    if (m_findEdit->text().isEmpty() || m_findPathEdit->text().isEmpty()) {
        return;
    }
    m_thread->findPath = m_findPathEdit->text();
    m_thread->findText = m_findEdit->text();
    m_thread->useRegExp = m_useRegexCheckBox->isChecked();
    m_thread->matchCase = m_matchCaseCheckBox->isChecked();
    m_thread->matchWord = m_matchWordCheckBox->isChecked();
    m_thread->findSub = m_findSubCheckBox->isChecked();
    m_thread->nameFilter = m_filterCombo->currentText().split(";");
    m_thread->start();
}

void FileSearch::findStarted()
{

}

void FileSearch::findResult(const FileSearchResult &result)
{
   // qDebug() << result.fileName << result.lineNumber << result.matchStart << result.matchingLine;
}
