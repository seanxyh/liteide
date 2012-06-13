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
// Module: golangdocapi.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-11
// $Id: golangdocapi.h,v 1.0 2011-8-1 visualfc Exp $

#ifndef __GOLANGDOCAPI_H__
#define __GOLANGDOCAPI_H__

#include "liteapi/liteapi.h"
#include "liteapi/litefindobj.h"

/*
openUrl(const QUrl &url);
url scheme
file : open html or plain file
list : url path only [pkg|cmd]
find : find pkg name
pdoc : show pkg doc
*/

namespace LiteApi {

enum FindApiFlag {
    PkgApi = 0x0001,
    ConstApi = 0x0002,
    VarApi = 0x0004,
    TypeApi = 0x008,
    FuncApi = 0x0010,
    MethodApi = 0x020,
    AllGolangApi = PkgApi | ConstApi | VarApi | TypeApi | FuncApi | MethodApi
};

class IGolangApi : public QObject
{
    Q_OBJECT
public:
    IGolangApi(QObject *parent) : QObject(parent) {}
public:
    virtual QStringList all(int flag = AllGolangApi) const = 0;
};

class IGolangDoc : public IObject
{
    Q_OBJECT
public:
    IGolangDoc(QObject *parent) : IObject(parent) {}
public slots:
    virtual void openUrl(const QUrl &url) = 0;
    virtual void activeBrowser() = 0;
};

inline IGolangDoc *getGolangDoc(LiteApi::IApplication *app)
{
    return LiteApi::findExtensionObject<IGolangDoc*>(app,"LiteApi.IGolangDoc");
}

}

#endif //__GOLANGDOCAPI_H__

