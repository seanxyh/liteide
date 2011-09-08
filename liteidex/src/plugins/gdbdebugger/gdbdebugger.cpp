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
// Module: gdbdebuger.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: gdbdebuger.cpp,v 1.0 2011-8-12 visualfc Exp $

#include "gdbdebugger.h"
#include "fileutil/fileutil.h"
#include "liteapi/litefindobj.h"

#include <QProcess>
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

GdbDebugeer::GdbDebugeer(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IDebugger(parent),
    m_liteApp(app),
    m_envManager(0)
{
    m_process = new QProcess(this);
    m_gdbinit = false;
    connect(app,SIGNAL(loaded()),this,SLOT(appLoaded()));
    connect(m_process,SIGNAL(started()),this,SIGNAL(debugStarted()));
    connect(m_process,SIGNAL(finished(int)),this,SIGNAL(debugStoped()));
    connect(m_process,SIGNAL(readyReadStandardError()),this,SLOT(readStdError()));
    connect(m_process,SIGNAL(readyReadStandardOutput()),this,SLOT(readStdOutput()));
}

GdbDebugeer::~GdbDebugeer()
{
    if (m_process) {
         delete m_process;
    }
}

void GdbDebugeer::appLoaded()
{
    m_envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
}

QString GdbDebugeer::mimeType() const
{
    return QLatin1String("debuger/gdb");
}

QAbstractItemModel *GdbDebugeer::debugModel(LiteApi::DEBUG_MODEL_TYPE type)
{
    return 0;
}

void GdbDebugeer::setWorkingDirectory(const QString &dir)
{
    m_process->setWorkingDirectory(dir);
}

void GdbDebugeer::setEnvironment (const QStringList &environment)
{
    m_process->setEnvironment(environment);
}

bool GdbDebugeer::start(const QString &program, const QStringList &arguments)
{
    qDebug() << program << arguments;

    if (!m_envManager) {
        return false;
    }

    FileUtil::findExecute(program);

    QStringList args;
    args << "--interpreter=mi" << "--args" << program;
    if (!arguments.isEmpty()) {
        args << arguments;
    }

    m_cmd = FileUtil::lookPath("gdb",m_envManager->currentEnvironment(),true);
    if (m_cmd.isEmpty()) {
        return false;
    }

    m_gdbinit = false;
    m_busy = false;
    m_index = 0;
    m_process->start(m_cmd,args);

    return true;
}

void GdbDebugeer::stop()
{
    writeCmd("-gdb-exit");
}

bool GdbDebugeer::isDebugging()
{
    return m_process->state() != QProcess::NotRunning;
}

void GdbDebugeer::abort()
{
    m_process->kill();
}

void GdbDebugeer::stepOver()
{
    writeCmd("-exec-next");
}

void GdbDebugeer::stepInto()
{
    writeCmd("-exec-step");
}

void GdbDebugeer::stepOut()
{
    writeCmd("-exec-return");
}

void GdbDebugeer::writeCmd(const QString &cmd)
{
    QString num = QString::number(m_index++);
    QString c = QString("%1%2\r\n").arg(num,8,'0').arg(cmd);
    m_process->write(c.toLatin1());
}

void GdbDebugeer::readStdError()
{
    qDebug() << "err" << m_process->readAllStandardError();
}

/*
27.4.2 gdb/mi Output Syntax

The output from gdb/mi consists of zero or more out-of-band records followed, optionally,
by a single result record. This result record is for the most recent command. The sequence
of output records is terminated by ¡®(gdb)¡¯.
If an input command was prefixed with a token then the corresponding output for that
command will also be prefixed by that same token.

If an input command was prefixed with a token then the corresponding output for that
command will also be prefixed by that same token.

output -> ( out-of-band-record )* [ result-record ] "(gdb)" nl
result-record ->
[ token ] "^" result-class ( "," result )* nl
out-of-band-record ->
async-record | stream-record
async-record ->
exec-async-output | status-async-output | notify-async-output
exec-async-output ->
[ token ] "*" async-output
status-async-output ->
[ token ] "+" async-output
notify-async-output ->
[ token ] "=" async-output
async-output ->
async-class ( "," result )* nl
result-class ->
"done" | "running" | "connected" | "error" | "exit"
async-class ->
"stopped" | others (where others will be added depending on the needs¡ªthis
is still in development).
result -> variable "=" value
variable ->
string
value -> const | tuple | list
const -> c-string
tuple -> "{}" | "{" result ( "," result )* "}"
list -> "[]" | "[" value ( "," value )* "]" | "[" result ( "," result )* "]"
stream-record ->
console-stream-output | target-stream-output | log-stream-output
console-stream-output ->
"~" c-string
target-stream-output ->
"@" c-string
log-stream-output ->
"&" c-string
nl -> CR | CR-LF
*/

static bool isNameChar(char c)
{
    // could be 'stopped' or 'shlibs-added'
    return (c >= 'a' && c <= 'z') || c == '-';
}

void GdbDebugeer::handleResponse(const QByteArray &buff)
{
    if (buff.isEmpty() || buff == "(gdb) ")
        return;

    const char *from = buff.constData();
    const char *to = from + buff.size();
    const char *inner;

    int token = -1;
    // Token is a sequence of numbers.
    for (inner = from; inner != to; ++inner)
        if (*inner < '0' || *inner > '9')
            break;
    if (from != inner) {
        token = QByteArray(from, inner - from).toInt();
        from = inner;
    }
    // Next char decides kind of response.
    const char c = *from++;
    switch (c) {
    case '*':
    case '+':
    case '=':
    {
        QByteArray asyncClass;
        for (; from != to; ++from) {
            const char c = *from;
            if (!isNameChar(c))
                break;
            asyncClass += *from;
        }
        GdbMiValue result;
        while (from != to) {
            GdbMiValue data;
            if (*from != ',') {
                // happens on archer where we get
                // 23^running <NL> *running,thread-id="all" <NL> (gdb)
                result.m_type = GdbMiValue::Tuple;
                break;
            }
            ++from; // skip ','
            data.parseResultOrValue(from, to);
            if (data.isValid()) {
                //qDebug() << "parsed result:" << data.toString();
                result.m_children += data;
                result.m_type = GdbMiValue::Tuple;
            }
        }
        handleAsyncClass(asyncClass,result);
        break;
    }
    case '~':
        handleConsoleStream(GdbMiValue::parseCString(from, to));
        break;
    case '@':
        handleTargetStream(GdbMiValue::parseCString(from, to));
        break;
    case '&':
        handleLogStream(GdbMiValue::parseCString(from, to));
        break;
    case '^': {
        GdbResponse response;

        response.token = token;

        for (inner = from; inner != to; ++inner)
            if (*inner < 'a' || *inner > 'z')
                break;

        QByteArray resultClass = QByteArray::fromRawData(from, inner - from);
        if (resultClass == "done") {
            response.resultClass = GdbResultDone;
        } else if (resultClass == "running") {
            response.resultClass = GdbResultRunning;
        } else if (resultClass == "connected") {
            response.resultClass = GdbResultConnected;
        } else if (resultClass == "error") {
            response.resultClass = GdbResultError;
        } else if (resultClass == "exit") {
            response.resultClass = GdbResultExit;
        } else {
            response.resultClass = GdbResultUnknown;
        }

        from = inner;
        if (from != to) {
            if (*from == ',') {
                ++from;
                response.data.parseTuple_helper(from, to);
                response.data.m_type = GdbMiValue::Tuple;
                response.data.m_name = "data";
            } else {
                // Archer has this.
                response.data.m_type = GdbMiValue::Tuple;
                response.data.m_name = "data";
            }
        }
        handleResultRecord(response);
        break;
    }
    default: {
        qDebug() << "UNKNOWN RESPONSE TYPE" << c;
        break;
    }
    }
}

void GdbDebugeer::handleStopped(const GdbMiValue &result)
{
    const QByteArray reason = result.findChild("reason").data();
    bool exit = false;
    if (reason == "exited-normally") {
        exit = true;
    } else if (reason == "exited") {
        exit = true;
    } else if (reason == "exited-signalled") {
        exit = true;
    } else if (reason == "breakpoint-hit") {
        QByteArray ar;
        result.dumpChildren(&ar,true,4);
        qDebug() << ar;
        qDebug() << result.findChild("frame").findChild("fullname").data();
        qDebug() << result.findChild("frame").findChild("line").data();
    }

    if (exit) {
        stop();
        return;
    }
}

void GdbDebugeer::handleAsyncClass(const QByteArray &asyncClass, const GdbMiValue &result)
{
    if (asyncClass == "stopped") {
        handleStopped(result);
    }
}

void GdbDebugeer::handleConsoleStream(const QByteArray &data)
{

}

void GdbDebugeer::handleTargetStream(const QByteArray &data)
{

}

void GdbDebugeer::handleLogStream(const QByteArray &data)
{

}

void GdbDebugeer::handleResultRecord(const GdbResponse &response)
{

}


void GdbDebugeer::initGdb()
{
#ifdef Q_OS_WIN
    writeCmd("set new-console on");
#endif
    writeCmd("set unwindonsignal on");
    writeCmd("set overload-resolution off");
    writeCmd("handle SIGSEGV nopass stop print");
    writeCmd("set breakpoint pending on");
    writeCmd("set width 0");
    writeCmd("set height 0");
    writeCmd("set auto-solib-add on");
    writeCmd("break main.main");
    writeCmd("-exec-run");
}

void GdbDebugeer::readStdOutput()
{
    int newstart = 0;
    int scan = m_inbuffer.size();
    m_inbuffer.append(m_process->readAllStandardOutput());

    // This can trigger when a dialog starts a nested event loop.
    if (m_busy)
        return;

    while (newstart < m_inbuffer.size()) {
        int start = newstart;
        int end = m_inbuffer.indexOf('\n', scan);
        if (end < 0) {
            m_inbuffer.remove(0, start);
            return;
        }
        newstart = end + 1;
        scan = newstart;
        if (end == start)
            continue;
#ifdef Q_OS_WIN
        if (m_inbuffer.at(end - 1) == '\r') {
            --end;
            if (end == start)
                continue;
        }
#endif
        m_busy = true;
        QByteArray data = QByteArray::fromRawData(m_inbuffer.constData() + start, end - start);
        handleResponse(data);
        m_busy = false;
    }
    m_inbuffer.clear();

    if (!m_gdbinit) {
        m_gdbinit = true;
        initGdb();
    }
    //wait for cmd;
}
