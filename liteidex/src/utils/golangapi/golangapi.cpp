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
// Module: golangapi.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-4-24
// $Id: golangapi.cpp,v 1.0 2012-4-24 visualfc Exp $

#include "golangapi.h"
#include <QFile>
#include <QSet>
#include <QRegExp>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

GolangApi::GolangApi(QObject *parent) :
    LiteApi::IGolangApi(parent)
{
}

bool GolangApi::load(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        return false;
    }
    QTextStream stream(&file);
    stream.setCodec("utf-8");
    return loadStream(&stream);
}

bool GolangApi::loadStream(QTextStream *stream)
{
    this->allMap.clear();
    this->pkgList.clear();
    this->typeList.clear();
    this->funcList.clear();
    this->methodList.clear();
    this->constList.clear();
    this->varList.clear();

    QRegExp reg("^pkg\\s([\\w\\/]+)(\\s\\(([\\w-]+)\\))?,\\s(\\w+)(\\s\\(\\*?([\\w\\-]+)\\))?\\s(\\w+)");
    QSet<QString> pkgSet;
    while (!stream->atEnd()) {
        QString line = stream->readLine();
        // 1 pkgname
        // 2 ? (system)
        // 3 ? system
        // 4 const|func|method|var|type
        // 5 ? (method-type)
        // 6 ? method-type
        // 7 name
        if (reg.indexIn(line.trimmed()) != -1) {
            //qDebug() << reg.cap() << reg.captureCount();
            QString pkg = reg.cap(1);
            //QString sys = reg.cap(3);
            QString flag = reg.cap(4);
            QString type = reg.cap(6);
            QString name = reg.cap(7);
            QString tag = pkg;
            if (!type.isEmpty()) {
                tag = pkg+"#"+type+"."+name;
            } else {
                tag = pkg+"#"+name;
            }
            if (flag == "const") {
                constList.append(tag);
            } else if (flag == "func") {
                funcList.append(tag);
            } else if (flag == "method") {
                methodList.append(tag);
            } else if (flag == "type") {
                typeList.append(tag);
            } else if (flag == "var") {
                varList.append(tag);
            }
            pkgSet.insert(pkg);
            allMap.insert(tag,line);
        }
    }
    pkgList = pkgSet.toList();
    pkgList.sort();
    constList.removeDuplicates();
    varList.removeDuplicates();
    typeList.removeDuplicates();
    methodList.removeDuplicates();
    funcList.removeDuplicates();
    return true;
}

QStringList GolangApi::all(LiteApi::FindApiFlag flag) const
{
    QStringList finds;
    if (flag & LiteApi::PkgApi) {
        finds.append(pkgList);
    }
    if (flag & LiteApi::TypeApi) {
        finds.append(typeList);
    }
    if (flag &LiteApi::FuncApi) {
        finds.append(funcList);
    }
    if (flag & LiteApi::MethodApi) {
        finds.append(methodList);
    }
    if (flag & LiteApi::ConstApi) {
        finds.append(constList);
    }
    if (flag &LiteApi::VarApi) {
        finds.append(varList);
    }
    return finds;
}

QStringList GolangApi::filter(const QString &str, LiteApi::FindApiFlag flag) const
{
    QStringList finds;
    if (flag & LiteApi::PkgApi) {
        finds.append(pkgList.filter(str));
    }
    if (flag & LiteApi::TypeApi) {
        finds.append(typeList.filter(str));
    }
    if (flag &LiteApi::FuncApi) {
        finds.append(funcList.filter(str));
    }
    if (flag & LiteApi::MethodApi) {
        finds.append(methodList.filter(str));
    }
    if (flag & LiteApi::ConstApi) {
        finds.append(constList.filter(str));
    }
    if (flag &LiteApi::VarApi) {
        finds.append(varList.filter(str));
    }
    return finds;
}

QStringList GolangApi::filter(const QRegExp &rx, LiteApi::FindApiFlag flag) const
{
    QStringList finds;
    if (flag & LiteApi::PkgApi) {
        finds.append(pkgList.filter(rx));
    }
    if (flag & LiteApi::TypeApi) {
        finds.append(typeList.filter(rx));
    }
    if (flag &LiteApi::FuncApi) {
        finds.append(funcList.filter(rx));
    }
    if (flag & LiteApi::MethodApi) {
        finds.append(methodList.filter(rx));
    }
    if (flag & LiteApi::ConstApi) {
        finds.append(constList.filter(rx));
    }
    if (flag &LiteApi::VarApi) {
        finds.append(varList.filter(rx));
    }
    return finds;
}

QStringList GolangApi::info(const QString &api) const
{
    return allMap.values(api);
}
