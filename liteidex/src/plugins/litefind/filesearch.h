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
// Module: filesearch.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-5-21
// $Id: filesearch.h,v 1.0 2012-5-21 visualfc Exp $

#ifndef FILESEARCH_H
#define FILESEARCH_H

#include "liteapi/liteapi.h"
#include <QStringList>
#include <QThread>

class FileSearchResult
{
public:
    FileSearchResult() {}
    FileSearchResult(QString fileName, int lineNumber, QString matchingLine,
                     int matchStart, int matchLength,
                     QStringList regexpCapturedTexts)
            : fileName(fileName),
            lineNumber(lineNumber),
            matchingLine(matchingLine),
            matchStart(matchStart),
            matchLength(matchLength),
            regexpCapturedTexts(regexpCapturedTexts)
    {
    }
    QString fileName;
    int lineNumber;
    QString matchingLine;
    int matchStart;
    int matchLength;
    QStringList regexpCapturedTexts;
};

Q_DECLARE_METATYPE(FileSearchResult)

class FindThread : public QThread
{
Q_OBJECT
public:
    FindThread(QObject *parent = 0);
    virtual void run();
public slots:
    void stop();
protected:
    void findDir(const QRegExp &reg, const QString &path);
    void findFile(const QRegExp &reg, const QString &fileName);
signals:
    void findResult(const FileSearchResult &result);
public:
    bool useRegExp;
    bool matchWord;
    bool matchCase;
    bool findSub;
    QString findText;
    QString findPath;
    QStringList nameFilter;
    volatile bool finding;
};

class QTabWidget;
class QLineEdit;
class QComboBox;
class QCheckBox;
class FileSearch : QObject
{
    Q_OBJECT
public:
    FileSearch(LiteApi::IApplication *app, QObject *parent = 0);
    ~FileSearch();
    QWidget* widget();
    void setVisible(bool b);
public slots:
    void findInFiles();
    void findResult(const FileSearchResult &rsult);
    void findStarted();
protected:
    LiteApi::IApplication *m_liteApp;
    FindThread *m_thread;
    QTabWidget  *m_tab;
    QWidget     *m_findWidget;
    QLineEdit   *m_findEdit;    
    QLineEdit   *m_findPathEdit;
    QComboBox   *m_filterCombo;
    QCheckBox   *m_findSubCheckBox;
    QCheckBox   *m_matchWordCheckBox;
    QCheckBox   *m_matchCaseCheckBox;
    QCheckBox   *m_useRegexCheckBox;
};

//static QList<FileSearchResult> findInFile(const QString &text, bool useRegExp, bool matchWord, bool matchCase, const QString &fileName);

#endif // FILESEARCH_H
