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
#include <QDebug>
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
    pkgMap.clear();
    //pkg go/ast, type File struct, Unresolved []*Ident
    QRegExp reg("^pkg\\s([\\w\\/]+)(\\s\\(([\\w-]+)\\))?,\\s(\\w+)(\\s\\(\\*?([\\w\\-]+)\\))?\\s(\\w+)");
    QRegExp reg2("^(\\w+),\\s+(\\w+)");
    while (!stream->atEnd()) {
        QString line = stream->readLine().trimmed();
        // 1 pkgname
        // 2 ? (system)
        // 3 ? system
        // 4 const|func|method|var|type
        // 5 ? (method-type)
        // 6 ? method-type
        // 7 value
        int pos = reg.indexIn(line);
        if (pos != -1) {
            QString typeVar;
            int next = reg2.indexIn(line.right(line.length()-reg.cap().length()).trimmed());
            if (next != -1) {
                typeVar = reg2.cap(2);
            }

            //qDebug() << reg.cap() << reg.captureCount();
            QString pkg = reg.cap(1);            
            //QString sys = reg.cap(3);
            QString flag = reg.cap(4);
            QString method_type = reg.cap(6);
            QString value = reg.cap(7);

            QMap<QString,Package>::iterator it = pkgMap.find(pkg);
            if (it == pkgMap.end()) {
                it = pkgMap.insert(pkg,Package());
            }

            if (flag == "const") {
                it->appendConst(value);
                //constList.append(tag);
            } else if (flag == "func") {
                //funcList.append(tag);
                it->appendFunc(value);
            } else if (flag == "method") {
                //methodList.append(tag);
                it->appendTypeMethod(method_type,value);
            } else if (flag == "type") {
                //typeList.append(tag);
                if (!typeVar.isEmpty()) {
                    it->appendTypeVar(value,typeVar);
                } else {
                    it->insertType(value);
                }
            } else if (flag == "var") {
                //varList.append(tag);
                it->appendVar(value);
            }
        }
    }
    return true;
}

QStringList GolangApi::all(int flag) const
{
    QStringList finds;
    QMapIterator<QString,Package> i(pkgMap);
    while (i.hasNext()) {
        i.next();
        QString pkgName = i.key();
        Package pkg = i.value();
        if (flag & LiteApi::PkgApi) {
            finds.append(pkgName);
        }
        if (flag &LiteApi::FuncApi) {
            foreach(QString v, pkg.funcList()) {
                finds.append(pkgName+"."+v+"()");
            }
        }
        if (flag & LiteApi::ConstApi) {
            foreach(QString v, pkg.constList()) {
                finds.append(pkgName+"."+v+"*");
            }
        }
        if (flag &LiteApi::VarApi) {
            foreach(QString v, pkg.varList()) {
                finds.append(pkgName+"."+v+"@");
            }
        }
        if (flag & LiteApi::TypeApi) {
            QMapIterator<QString,Type> m(pkg.typeMap());
            while(m.hasNext()) {
                m.next();
                QString methodName = pkgName+"."+m.key();
                finds.append(methodName);
                if (flag &LiteApi::MethodApi) {
                    foreach(QString v, m.value().methodList()) {
                        finds.append(methodName+"."+v+"()");
                    }
                }
                if (flag &LiteApi::TypeVarApi) {
                    foreach(QString v, m.value().varList()) {
                        finds.append(methodName+"."+v+"$");
                    }
                }
            }
        }
    }
    return finds;
}
