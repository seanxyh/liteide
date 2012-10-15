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
    m_pkgs.clear();

    //pkg syscall (darwin-386), const TIOCSBRK ideal-int

    //var
    //pkg archive/tar, var ErrFieldTooLong error

    //const
    //pkg archive/tar, const TypeBlock ideal-char
    //pkg crypto/sha512, const BlockSize ideal-int
    //pkg archive/zip, const Deflate uint16

    //type-const
    //pkg go/ast, type ObjKind int
    //pkg syscall, type Errno uintptr
    //pkg syscall (windows-amd64), const ECOMM Errno

    // func
    //pkg archive/tar, func NewReader(io.Reader) *Reader
    //pkg bytes, func FieldsFunc([]byte, func(rune) bool) [][]byte

    //type-var,method
    //pkg archive/tar, type Header struct
    //pkg archive/tar, type Header struct, AccessTime time.Time
    //pkg archive/tar, method (*Reader) Next() (*Header, error)
    //pkg archive/zip, method (*File) Open() (io.ReadCloser, error)
    //pkg bufio, method (ReadWriter) Available() int

    //type-interface
    //pkg container/heap, type Interface interface { Len, Less, Pop, Push, Swap }
    //pkg container/heap, type Interface interface, Len() int

    QRegExp reg("^pkg\\s([\\w\\/]+)(\\s\\(([\\w-]+)\\))?,\\s(\\w+)");
    QRegExp regm("\\(\\*?([\\w\\-]+)\\)\\s*(\\w+)");
    Package *lastPkg = 0;
    Type *lastType = 0;
    while (!stream->atEnd()) {
        QString line = stream->readLine();
        int pos = reg.indexIn(line);
        if (pos < 0) {
            continue;
        }
        // 1 pkgname
        // 2 ? (system)
        // 3 ? system
        // 4 const|func|method|var|type
        QString pkgName = reg.cap(1);
//        if (!reg.cap(3).isEmpty()) {
//            pkg = reg.cap(2)+"."+pkg;
//        }
        if (!lastPkg || lastPkg->name != pkgName) {
            lastPkg = m_pkgs.findPackage(pkgName);
            if (!lastPkg) {
                lastPkg = new Package(pkgName);
                m_pkgs.pkgList.append(lastPkg);
                lastType = 0;
            }
        }
        QString right = line.mid(reg.cap().length()).trimmed();
        QString flag = reg.cap(4);
        if (flag == "var") {
            ///pkg archive/tar, var ErrFieldTooLong error
            int pos = right.indexOf(" ");
            if (pos != -1) {
                lastPkg->valueList.append(new Value(VarApi,right.left(pos),right.mid(pos+1)));
            }
        } else if (flag == "const") {
            //pkg syscall (windows-amd64), const ECOMM Errno
            int pos = right.indexOf(" ");
            if (pos != -1) {
                lastPkg->valueList.append(new Value(ConstApi,right.left(pos),right.mid(pos+1)));
            }
        } else if (flag == "func") {
            //pkg bytes, func FieldsFunc([]byte, func(rune) bool) [][]byte
            int pos = right.indexOf("(");
            if (pos != -1) {
                lastPkg->valueList.append(new Value(FuncApi,right.left(pos),right.mid(pos)));
            }
        } else if (flag == "method") {
            //pkg archive/tar, method (*Reader) Next() (*Header, error)
            //pkg archive/zip, method (*File) Open() (io.ReadCloser, error)
            //pkg bufio, method (ReadWriter) Available() int
            int pos = regm.indexIn(right);
            if (pos != -1) {
                QString typeName = regm.cap(1);
                QString name = regm.cap(2);
                QString exp = right.mid(regm.cap().length()).trimmed();
                if (lastType == 0 || lastType->name != typeName || lastType->typ == StructApi) {
                    lastType = lastPkg->findType(typeName);
                    if (!lastType) {
                        lastType = new Type(StructApi,typeName,"struct");
                        lastPkg->typeList.append(lastType);
                    }
                }
                lastType->valueList.append(new Value(TypeMethodApi,name,exp));
            }
        } else if (flag == "type") {
            //pkg go/ast, type ObjKind int
            //pkg archive/tar, type Header struct
            //pkg archive/tar, type Header struct, AccessTime time.Time
            //pkg container/heap, type Interface interface { Len, Less, Pop, Push, Swap }
            //pkg container/heap, type Interface interface, Len() int
            int pos = right.indexOf(" ");
            if (pos != -1) {
                QString typeName = right.left(pos);
                QString exp = right.mid(pos+1);
                if (exp == "struct") {
                    lastType = lastPkg->findType(typeName);
                    if (!lastType) {
                        lastType = new Type(StructApi,typeName,exp);
                        lastPkg->typeList.append(lastType);
                    }
                } else if (exp.startsWith("struct,")) {
                    QString last = exp.mid(7).trimmed();
                    int pos2 = last.indexOf(" ");
                    if (pos2 != -1) {
                        if (lastType == 0 || lastType->name != typeName || lastType->typ == StructApi) {
                            lastType = lastPkg->findType(typeName);
                            if (!lastType) {
                                lastType = new Type(StructApi,typeName,"struct");
                                lastPkg->typeList.append(lastType);
                            }
                        }
                        lastType->valueList.append(new Value(TypeVarApi,last.left(pos2),last.mid(pos2+1)));
                    }
                } else if (exp.startsWith("interface {")) {
                    lastType = lastPkg->findType(typeName);
                    if (!lastType) {
                        lastType = new Type(InterfaceApi,typeName,exp);
                        lastPkg->typeList.append(lastType);
                    }
                } else if (exp.startsWith("interface,")) {
                    QString last = exp.mid(10).trimmed();
                    int pos2 = last.indexOf("(");
                    if (pos2 != -1) {
                        if (lastType == 0 || lastType->name != typeName || lastType->typ == StructApi) {
                            lastType = lastPkg->findType(typeName);
                            if (!lastType) {
                                lastType = new Type(InterfaceApi,typeName,"struct");
                                lastPkg->typeList.append(lastType);
                            }
                        }
                        lastType->valueList.append(new Value(TypeMethodApi,last.left(pos2),last.mid(pos2+1)));
                    }
                } else {
                    lastType = lastPkg->findType(typeName);
                    if (!lastType) {
                        lastType = new Type(TypeApi,typeName,exp);
                        lastPkg->typeList.append(lastType);
                    }
                }
            }
        }
    }

//    QRegExp reg("^pkg\\s([\\w\\/]+)(\\s\\(([\\w-]+)\\))?,\\s(\\w+)(\\s\\(\\*?([\\w\\-]+)\\))?\\s(\\w+)");
//    QRegExp reg2("^(\\w+),\\s+(\\w+)");
//    while (!stream->atEnd()) {
//        QString line = stream->readLine().trimmed();
//        // 1 pkgname
//        // 2 ? (system)
//        // 3 ? system
//        // 4 const|func|method|var|type
//        // 5 ? (method-type)
//        // 6 ? method-type
//        // 7 value
//        int pos = reg.indexIn(line);
//        if (pos != -1) {
//            QString typeVar;
//            int next = reg2.indexIn(line.right(line.length()-reg.cap().length()).trimmed());
//            if (next != -1) {
//                typeVar = reg2.cap(2);
//            }

//            //qDebug() << reg.cap() << reg.captureCount();
//            QString pkg = reg.cap(1);
//            //QString sys = reg.cap(3);
//            QString flag = reg.cap(4);
//            QString method_type = reg.cap(6);
//            QString value = reg.cap(7);

//            QMap<QString,Package>::iterator it = pkgMap.find(pkg);
//            if (it == pkgMap.end()) {
//                it = pkgMap.insert(pkg,Package());
//            }

//            if (flag == "const") {
//                it->appendConst(value);
//                //constList.append(tag);
//            } else if (flag == "func") {
//                //funcList.append(tag);
//                it->appendFunc(value);
//            } else if (flag == "method") {
//                //methodList.append(tag);
//                it->appendTypeMethod(method_type,value);
//            } else if (flag == "type") {
//                //typeList.append(tag);
//                if (!typeVar.isEmpty()) {
//                    it->appendTypeVar(value,typeVar);
//                } else {
//                    it->insertType(value);
//                }
//            } else if (flag == "var") {
//                //varList.append(tag);
//                it->appendVar(value);
//            }
//        }
//    }
    return true;
}

QStringList GolangApi::all(int flag) const
{
    QStringList finds;
    foreach(Package *pkg, m_pkgs.pkgList) {
        finds.append(pkg->name);
        foreach(Value *value, pkg->valueList) {
            if (flag & value->typ) {
                finds.append(pkg->name+"."+value->name);
            }
        }
        foreach(Type *type, pkg->typeList) {
            if (flag & type->typ) {
                QString name = pkg->name+"."+type->name;
                finds.append(name);
                name += ".";
                foreach (Value *value, type->valueList) {
                    if (flag & value->typ) {
                        finds.append(name+value->name);
                    }
                }
            }
        }

//        if (flag &LiteApi::FuncApi) {
//            foreach(QString v, pkg.funcList()) {
//                finds.append(pkgName+"."+v+"()");
//            }
//        }
//        if (flag & LiteApi::ConstApi) {
//            foreach(QString v, pkg.constList()) {
//                finds.append(pkgName+"."+v+"*");
//            }
//        }
//        if (flag &LiteApi::VarApi) {
//            foreach(QString v, pkg.varList()) {
//                finds.append(pkgName+"."+v+"@");
//            }
//        }
//        if (flag & LiteApi::TypeApi) {
//            QMapIterator<QString,Type> m(pkg.typeMap());
//            while(m.hasNext()) {
//                m.next();
//                QString methodName = pkgName+"."+m.key();
//                finds.append(methodName);
//                if (flag &LiteApi::MethodApi) {
//                    foreach(QString v, m.value().methodList()) {
//                        finds.append(methodName+"."+v+"()");
//                    }
//                }
//                if (flag &LiteApi::TypeVarApi) {
//                    foreach(QString v, m.value().fieldList()) {
//                        finds.append(methodName+"."+v+"$");
//                    }
//                }
//            }
//        }
    }
    return finds;
}
