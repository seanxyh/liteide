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
// Module: litedebugapi.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: litedebugapi.h,v 1.0 2011-8-12 visualfc Exp $

#ifndef __LITEDEBUGAPI_H__
#define __LITEDEBUGAPI_H__

#include "liteapi/liteapi.h"

namespace LiteApi {

class IDebug : public QObject
{
    Q_OBJECT
public:
    IDebug(QObject *parent = 0): QObject(parent) {}
    virtual ~IDebug() {}
public:
    virtual QString mimeType() const = 0;
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual bool isDebugging() = 0;
    virtual bool abort() = 0;
    virtual bool stepOver() = 0;
    virtual bool stepInto() = 0;
    virtual bool stepOut() = 0;
signals:
    void debugStarted();
    void debugStoped();
};

class IDebugManager : public IManager
{
    Q_OBJECT
public:
    IDebugManager(QObject *parent = 0) : IManager(parent) {}
    virtual void addDebug(IDebug *debug) = 0;
    virtual void removeDebug(IDebug *debug) = 0;
    virtual IDebug *findDebug(const QString &mimeType) = 0;
    virtual QList<IDebug*> debugList() const = 0;
    virtual void setCurrentDebug(IDebug *debug) = 0;
    virtual IDebug *currentDebug() = 0;
signals:
    void currentDebugChanged(LiteApi::IDebug*);
};

} //namespace LiteApi

#endif //__LITEDEBUGAPI_H__

