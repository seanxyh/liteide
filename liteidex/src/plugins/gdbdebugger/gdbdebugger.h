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
// Module: gdbdebugger.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: gdbdebugger.h,v 1.0 2011-8-12 visualfc Exp $

#ifndef GDBDEBUGGER_H
#define GDBDEBUGGER_H

#include "litedebugapi/litedebugapi.h"
#include "liteenvapi/liteenvapi.h"
#include "qtc_gdbmi/gdbmi.h"

class QProcess;
class GdbDebugeer : public LiteApi::IDebugger
{
    Q_OBJECT
public:
    GdbDebugeer(LiteApi::IApplication *app, QObject *parent = 0);
    ~GdbDebugeer();
public:
    virtual QString mimeType() const;
    virtual QAbstractItemModel *debugModel(LiteApi::DEBUG_MODEL_TYPE type);
    virtual void setWorkingDirectory(const QString &dir);
    virtual void setEnvironment (const QStringList &environment);
    virtual bool start(const QString &program, const QStringList &arguments);
    virtual bool stop();
    virtual bool isDebugging();
    virtual bool abort();
    virtual bool stepOver();
    virtual bool stepInto();
    virtual bool stepOut();
public slots:
    void appLoaded();
    void readStdError();
    void readStdOutput();
protected:
    void handleResponse(const QByteArray &buff);
    void handleAsyncClass(const QByteArray &asyncClass, const GdbMiValue &result);
    void handleConsoleStream(const QByteArray &data);
    void handleTargetStream(const QByteArray &data);
    void handleLogStream(const QByteArray &data);
    void handleResultRecord(const GdbResponse &response);
    void writeCmd(const QString &cmd);
    void initGdb();
protected:
    LiteApi::IApplication   *m_liteApp;
    LiteApi::IEnvManager    *m_envManager;
    QProcess *m_process;
    QString m_cmd;
    QByteArray m_inbuffer;
    bool    m_busy;
    bool    m_gdbinit;
    int     m_index;
};

#endif // GDBDEBUGGER_H
