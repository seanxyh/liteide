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
// Module: liteapi.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: liteapi.h,v 1.0 2011-4-22 visualfc Exp $

#ifndef __LITEEDITORAPI_H__
#define __LITEEDITORAPI_H__

#include "liteapi.h"
#include <QTextCursor>
#include <QCompleter>

namespace LiteApi {

class IWordApi
{
public:
    virtual ~IWordApi() {}
    virtual QString mimeType() const = 0;
    virtual QStringList apiFiles() const = 0;
    virtual bool loadApi() = 0;
    virtual QStringList wordList() const = 0;
};

class IWordApiManager : public IManager
{
    Q_OBJECT
public:
    IWordApiManager(QObject *parent = 0) : IManager(parent) {}
    virtual void addWordApi(IWordApi *wordApi) = 0;
    virtual void removeWordApi(IWordApi *wordApi) = 0;
    virtual IWordApi *findWordApi(const QString &mimeType) = 0;
    virtual QList<IWordApi*> wordApiList() const = 0;
};

class ICompleter : public QObject
{
    Q_OBJECT
public:
    ICompleter(QObject *parent): QObject(parent) {}
    virtual QCompleter *completer() const = 0;
    virtual bool appendItem(QString item,bool temp) = 0;
    virtual void appendItems(QStringList items, bool temp) = 0;
    virtual void clearTemp() = 0;
    virtual void clear() = 0;
    virtual void show() = 0;
signals:
    void prefixChanged(QTextCursor,QString);
    void wordCompleted(const QString &func, const QStringList &args);
};

} //namespace LiteApi


#endif //__LITEEDITORAPI_H__

