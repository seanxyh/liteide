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
#include <QAbstractItemModel>

namespace LiteApi {

enum DEBUG_MODEL_TYPE{
    LOCALS_MODEL = 1,
    WATCHES_MODEL,
    CALLSTACK_MODEL,
    BREAKPOINTS_MODEL,
    THREADS_MODEL,
};

class IDebugger : public QObject
{
    Q_OBJECT
public:
    IDebugger(QObject *parent = 0): QObject(parent) {}
    virtual ~IDebugger() {}
public:
    virtual QString mimeType() const = 0;
    virtual QAbstractItemModel *debugModel(DEBUG_MODEL_TYPE type) = 0;
    virtual void setWorkingDirectory(const QString &dir) = 0;
    virtual void setEnvironment (const QStringList &environment) = 0;
    virtual bool start(const QString &program, const QStringList &arguments) = 0;
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

class IDebuggerManager : public IManager
{
    Q_OBJECT
public:
    IDebuggerManager(QObject *parent = 0) : IManager(parent) {}
    virtual void addDebugger(IDebugger *debug) = 0;
    virtual void removeDebugger(IDebugger *debug) = 0;
    virtual IDebugger *findDebugger(const QString &mimeType) = 0;
    virtual QList<IDebugger*> debuggerList() const = 0;
    virtual void setCurrentDebugger(IDebugger *debug) = 0;
    virtual IDebugger *currentDebugger() = 0;
signals:
    void currentDebuggerChanged(LiteApi::IDebugger*);
};

} //namespace LiteApi

#endif //__LITEDEBUGAPI_H__

