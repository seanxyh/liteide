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

class TypeData : public QSharedData
{
public:
    TypeData(){}
    TypeData(const TypeData &other)
        : QSharedData(other),varList(other.varList),methodList(other.methodList) {}
    ~TypeData() {}
    QStringList varList;
    QStringList methodList;
};

class Type
{
public:
    Type() { d = new TypeData; }
    Type(const Type &other)
        : d(other.d) {}
    QStringList varList() const { return d->varList; }
    QStringList methodList() const { return d->methodList; }
    void appendMethod(const QString &value) { d->methodList.append(value); }
private:
     QSharedDataPointer<TypeData> d;
};

class PackageData : public QSharedData
{
public:
    PackageData() {}
    PackageData(const PackageData &other)
        : QSharedData(other),
          constList(other.constList),
          varList(other.varList),
          funcList(other.funcList),
          typeMap(other.typeMap)
    {}
    ~PackageData()  { }
    QStringList constList;
    QStringList varList;
    QStringList funcList;
    QMap<QString,Type>  typeMap;
};

class Package
{
public:
    Package() {d = new PackageData;}
    Package(const Package &other)
        : d(other.d) {}
    ~Package() {}
    QStringList constList() const {return d->constList; }
    QStringList varList() const { return d->varList; }
    QStringList funcList() const { return d->funcList; }
    QStringList typeList() const { return d->typeMap.keys(); }
    QMap<QString,Type> typeMap() const { return d->typeMap; }
    void appendConst(const QString &value) {
        if (!d->constList.contains(value))
            d->constList.append(value);
    }
    void appendVar(const QString &value) {
        if (!d->varList.contains(value))
            d->varList.append(value);
    }
    void appendFunc(const QString &value) {
        if (!d->funcList.contains(value))
            d->funcList.append(value);
    }
    void insertType(const QString &value) {
        if (!d->typeMap.contains(value)) {
            d->typeMap.insert(value,Type());
        }
    }
    void appendTypeMethod(const QString &value, const QString method) {
        QMap<QString,Type>::iterator it = d->typeMap.find(value);
        if (it == d->typeMap.end()) {
            it = d->typeMap.insert(value,Type());
        }
        it->appendMethod(method);
    }
    Type type(const QString &value) const {return d->typeMap.value(value); }
protected:
    QSharedDataPointer<PackageData> d;
};


class GolangApi : public LiteApi::IGolangApi
{
public:
    GolangApi(QObject *parent = 0);
    bool load(const QString &fileName);
    bool loadStream(QTextStream *stream);
    virtual QStringList all(int flag) const;
protected:
    QMap<QString,Package> pkgMap;
};

#endif // GOLANGAPI_H
