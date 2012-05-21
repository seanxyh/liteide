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
#include <QTextStream>
#include <QTextDocument>
#include <QRegExp>

FileSearch::FileSearch(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app)
{
    m_widget = new QWidget;
}

QWidget *FileSearch::widget()
{
    return m_widget;
}


QList<FileSearchResult> findInFile(const QString &text, bool useRegExp, bool matchWord, bool matchCase, const QString &fileName)
{
    QList<FileSearchResult> results;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return results;
    }
    QRegExp reg;
    if (matchWord) {
        reg.setPattern(QString::fromLatin1("\\b%1\\b").arg(text));
    } else {
        reg.setPattern(text);
    }
    reg.setCaseSensitivity(matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
    if (useRegExp) {
        reg.setPatternSyntax(QRegExp::FixedString);
    }

    QTextStream stream(&file);
    QString line;
    int lineNr = 1;
    while (!stream.atEnd()) {
        line = stream.readLine();
        int pos = 0;
        while ((pos = reg.indexIn(line, pos)) != -1) {
            results << FileSearchResult(fileName, lineNr, line,
                                          pos, reg.matchedLength(),
                                          reg.capturedTexts());
            pos += reg.matchedLength();
        }
        lineNr++;
    }

    return results;
}
