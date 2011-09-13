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
class GdbHandleState
{
public:
    GdbHandleState() : m_exited(false),m_stopped(false) {}
    void clear()
    {
        m_msg.clear();
        m_exited = false;
        m_stopped = false;
    }
    void setExited(bool b) {m_exited = b;}
    void setStopped(bool b) {m_stopped = b;}
    void appendMsg(const QByteArray &m) { m_msg.append(m); }
    bool exited() { return m_exited; }
    bool stopped() { return m_stopped; }
public:
    bool       m_exited;
    bool       m_stopped;
    QList<QByteArray> m_msg;
};

class QStandardItemModel;
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
    virtual void stop();
    virtual bool isDebugging();
    virtual void abort();
    virtual void stepOver();
    virtual void stepInto();
    virtual void stepOut();
    virtual void execContinue();
    virtual void runJump(const QString &fileName, const QString &spec);
    virtual void command(const QByteArray &cmd);
public slots:
    void appLoaded();
    void readStdError();
    void readStdOutput();
    void finished(int);
protected:
    void handleResponse(const QByteArray &buff);
    void handleStopped(const GdbMiValue &result);
    void handleLibrary(const GdbMiValue &result);
    void handleAsyncClass(const QByteArray &asyncClass, const GdbMiValue &result);
    void handleConsoleStream(const QByteArray &data);
    void handleTargetStream(const QByteArray &data);
    void handleLogStream(const QByteArray &data);
    void handleResultRecord(const GdbResponse &response);
protected:
    void initGdb();
    void updateLocals();
    void updateFrames();
    void updateBreaks();
protected:
    LiteApi::IApplication   *m_liteApp;
    LiteApi::IEnvManager    *m_envManager;
    QProcess *m_process;
    QStandardItemModel *m_asyncModel;
    QStandardItemModel *m_localsModel;
    QStandardItemModel *m_framesModel;
    QStandardItemModel *m_libraryModel;
    QString m_cmd;
    QByteArray m_runtime;
    QByteArray m_inbuffer;
    GdbHandleState m_handleState;
    QList<QByteArray> m_gdbCommand;
    bool    m_busy;
    bool    m_gdbinit;
    int     m_index;
};

#endif // GDBDEBUGGER_H
