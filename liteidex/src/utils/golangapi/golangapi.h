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
// Module: golangapi.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-4-24
// $Id: golangapi.h,v 1.0 2012-4-24 visualfc Exp $

#ifndef GOLANGAPI_H
#define GOLANGAPI_H

#include "golangdocapi/golangdocapi.h"

#include <QTextStream>
#include <QSharedData>

class PackageData : public QSharedData
{
public:
    PackageData();
    PackageData(const PackageData &other)
    {

    }
    ~PackageData()
    {
    }
    QStringList constList;
    QStringList typeList;
    QStringList varList;
    QStringList funcList;
};


class GolangApi : public LiteApi::IGolangApi
{
public:
    GolangApi(QObject *parent = 0);
    bool load(const QString &fileName);
    bool loadStream(QTextStream *stream);
    virtual QStringList all(LiteApi::FindApiFlag flag) const;
    virtual QStringList filter(const QString &str, LiteApi::FindApiFlag flag) const;
    virtual QStringList filter(const QRegExp &rx, LiteApi::FindApiFlag flag) const;
    virtual QStringList info(const QString &api) const;
protected:
    QStringList pkgList;
    QStringList constList;
    QStringList varList;
    QStringList funcList;
    QStringList methodList;
    QStringList typeList;
    QMultiMap<QString,QString> allMap;
};

#endif // GOLANGAPI_H
