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

class FileSearch : QObject
{
    Q_OBJECT
public:
    FileSearch(LiteApi::IApplication *app, QObject *parent = 0);
    QWidget* widget();
protected:
    LiteApi::IApplication *m_liteApp;
    QWidget *m_widget;
};

static QList<FileSearchResult> findInFile(const QString &text, bool useRegExp, bool matchWord, bool matchCase, const QString &fileName);

#endif // FILESEARCH_H
