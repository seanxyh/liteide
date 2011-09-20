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

#include <QStandardItemModel>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QFileInfo>
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

//GdbMiValue
/*
    QByteArray result;
    switch (m_type) {
        case Invalid:
            if (multiline)
                result += ind(indent) + "Invalid\n";
            else
                result += "Invalid";
            break;
        case Const:
            if (!m_name.isEmpty())
                result += m_name + '=';
            result += '"' + escapeCString(m_data) + '"';
            break;
        case Tuple:
            if (!m_name.isEmpty())
                result += m_name + '=';
            if (multiline) {
                result += "{\n";
                dumpChildren(&result, multiline, indent + 1);
                result += '\n' + ind(indent) + '}';
            } else {
                result += '{';
                dumpChildren(&result, multiline, indent + 1);
                result += '}';
            }
            break;
        case List:
            if (!m_name.isEmpty())
                result += m_name + '=';
            if (multiline) {
                result += "[\n";
                dumpChildren(&result, multiline, indent + 1);
                result += '\n' + ind(indent) + ']';
            } else {
                result += '[';
                dumpChildren(&result, multiline, indent + 1);
                result += ']';
            }
            break;
    }
    return result;
*/

static void GdbMiValueToItem(QStandardItem *item, const GdbMiValue &value)
{
    switch (value.type()) {
    case GdbMiValue::Invalid:
        item->appendRow(new QStandardItem("Invalid"));
        break;
    case GdbMiValue::Const:
        if (value.name().isEmpty()) {
            item->appendRow(new QStandardItem(QString(value.data())));
        } else {
            item->appendRow(new QStandardItem(QString(value.name()+"="+value.data())));
        }
        break;
    case GdbMiValue::List: {
            QStandardItem *in = new QStandardItem(QString(value.name()));
            item->appendRow(in);
            for (int i = 0; i < value.childCount(); i++) {
                QStandardItem *iv = new QStandardItem(QString("[%1]").arg(i));
                in->appendRow(iv);
                GdbMiValueToItem(iv,value.childAt(i));
            }
            break;
        }
    case GdbMiValue::Tuple: {
            QStandardItem *iv = item;
            if (!value.name().isEmpty()) {
                iv = new QStandardItem(QString(value.name()));
                item->appendRow(iv);
            }
            foreach (const GdbMiValue &v, value.children()) {
                GdbMiValueToItem(iv,v);
            }
            break;
       }
    }
}

GdbDebugeer::GdbDebugeer(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IDebugger(parent),
    m_liteApp(app),
    m_envManager(0)
{
    m_process = new QProcess(this);
    m_asyncModel = new QStandardItemModel(this);
    m_asyncItem = new QStandardItem;
    m_asyncModel->appendRow(m_asyncItem);
    /*
    m_asyncModel->setHeaderData(0,Qt::Horizontal,"Reason");
    m_asyncModel->setHeaderData(1,Qt::Horizontal,"Address");
    m_asyncModel->setHeaderData(2,Qt::Horizontal,"Function");
    m_asyncModel->setHeaderData(3,Qt::Horizontal,"File");
    m_asyncModel->setHeaderData(4,Qt::Horizontal,"Line");
    m_asyncModel->setHeaderData(5,Qt::Horizontal,"Thread ID");
    m_asyncModel->setHeaderData(6,Qt::Horizontal,"Stoped Threads");
    */
    m_varsModel = new QStandardItemModel(0,3,this);

    m_framesModel = new QStandardItemModel(0,5,this);
    m_framesModel->setHeaderData(0,Qt::Horizontal,"Level");
    m_framesModel->setHeaderData(1,Qt::Horizontal,"Address");
    m_framesModel->setHeaderData(2,Qt::Horizontal,"Function");
    m_framesModel->setHeaderData(3,Qt::Horizontal,"File");
    m_framesModel->setHeaderData(4,Qt::Horizontal,"Line");

    m_libraryModel = new QStandardItemModel(0,2,this);
    m_libraryModel->setHeaderData(0,Qt::Horizontal,"Id");
    m_libraryModel->setHeaderData(1,Qt::Horizontal,"Thread Groups");

    m_gdbinit = false;    

    connect(app,SIGNAL(loaded()),this,SLOT(appLoaded()));
    connect(m_process,SIGNAL(started()),this,SIGNAL(debugStarted()));
    connect(m_process,SIGNAL(finished(int)),this,SIGNAL(debugStoped()));
    connect(m_process,SIGNAL(finished(int)),this,SLOT(finished(int)));
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
    if (type == LiteApi::ASYNC_MODEL) {
        return m_asyncModel;
    } else if (type == LiteApi::VARS_MODEL) {
        return m_varsModel;
    } else if (type == LiteApi::CALLSTACK_MODEL) {
        return m_framesModel;
    } else if (type == LiteApi::LIBRARY_MODEL) {
        return m_libraryModel;
    }
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
    if (!m_envManager) {
        return false;
    }

    QStringList args;
    args << "--interpreter=mi";

    QString goroot = m_envManager->currentEnvironment().value("GOROOT");
    if (!goroot.isEmpty()) {
        QString path = QFileInfo(QDir(goroot),"src/pkg/runtime/").path();
       // args << "--directory" << path;
        m_runtime = path.toUtf8();
    }

    args << "--args" << program;
    if (!arguments.isEmpty()) {
        args << arguments;
    }

    QString gdb = m_envManager->currentEnvironment().value("LITEIDE_GDB","gdb");

    m_cmd = FileUtil::lookPath(gdb,m_envManager->currentEnvironment(),true);
    if (m_cmd.isEmpty()) {
        return false;
    }

    m_gdbinit = false;
    m_busy = false;
    m_token = 10000000;
    m_handleState.clear();
    m_varWatchMap.clear();

    m_varsModel->removeRows(0,m_varsModel->rowCount());
    m_libraryModel->removeRows(0,m_libraryModel->rowCount());    

    m_process->start(m_cmd,args);

    QString log = QString("%1 %2").arg(m_cmd).arg(args.join(" "));
    emit debugLog(log.toUtf8());

    return true;
}

void GdbDebugeer::stop()
{
    command("-gdb-exit");
}

bool GdbDebugeer::isDebugging()
{
    return m_process->state() != QProcess::NotRunning;
}

void GdbDebugeer::abort()
{
    m_process->kill();
}

void GdbDebugeer::execContinue()
{
    command("-exec-continue");
}

void GdbDebugeer::stepOver()
{
    command("-exec-next");
}

void GdbDebugeer::stepInto()
{
    command("-exec-step");
}

void GdbDebugeer::stepOut()
{
    command("-exec-finish");
}

void GdbDebugeer::runJump(const QString &fileName, const QString &spec)
{
    if (fileName.isEmpty()) {
        command("-break-insert -t "+spec.toLatin1());
    } else {
        command("-break-insert -t "+fileName.toLatin1()+":"+spec.toLatin1());
    }
    command("-exec-continue");
}

void GdbDebugeer::createWatch(const QString &var, bool floating)
{
    m_varWatchMap.insert(var,"nil");
    GdbCmd cmd;
    QStringList args;
    args << "-var-create";
    args << "-";
    if (floating) {
        args << "@";
    } else {
        args << "*";
    }
    args << var;
    cmd.setCmd(args);
    cmd.insert("var",var);
    command(cmd);
}

void GdbDebugeer::removeWatch(const QString &var, bool children)
{
    QString name = m_varWatchMap.value(var);
    QStringList args;
    args << "-var-delete";
    if (children) {
        args << "-c";
    }
    args << name;
    GdbCmd cmd;
    cmd.setCmd(args);
    cmd.insert("var",var);
    cmd.insert("name",name);
    cmd.insert("children",children);
    command(cmd);
}

void GdbDebugeer::command(const GdbCmd &cmd)
{
    m_token++;
    QByteArray buf = cmd.makeCmd(m_token);
    emit debugLog(">>> "+buf);
#ifdef Q_OS_WIN
    buf.append("\r\n");
#else
    buf.append("\n");
#endif
    m_tokenCookieMap.insert(m_token,cmd.cookie());
    m_process->write(buf);
}

void  GdbDebugeer::command(const QByteArray &cmd)
{
    command(GdbCmd(cmd));
}

void GdbDebugeer::readStdError()
{
    qDebug() << "err" << m_process->readAllStandardError();
}

/*
27.4.2 gdb/mi Output Syntax

The output from gdb/mi consists of zero or more out-of-band records followed, optionally,
by a single result record. This result record is for the most recent command. The sequence
of output records is terminated by ��(gdb)��.
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
"stopped" | others (where others will be added depending on the needs��this
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
        if (m_tokenCookieMap.contains(token)) {
            response.cookie = m_tokenCookieMap.take(token);
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
    QByteArray reason = result.findChild("reason").data();
    m_handleState.setStopped(true);
    if (reason.startsWith("exited")) {
        m_handleState.setExited(true);
        m_handleState.appendMsg(reason);
        return;
    }
    QString thread_id = result.findChild("thread-id").data();
    QString stopped_threads = result.findChild("stopped-threads").data();
    GdbMiValue frame = result.findChild("frame");
    if (frame.isValid()) {
        QString addr = frame.findChild("addr").data();
        QString func = frame.findChild("func").data();
        QString file = frame.findChild("file").data();
        QString fullname = frame.findChild("fullname").data();
        QString line = frame.findChild("line").data();
        /*
        QList<QStandardItem*> items;
        items << new QStandardItem(QString(reason))
              << new QStandardItem(addr)
              << new QStandardItem(func)
              << new QStandardItem(file)
              << new QStandardItem(line)
              << new QStandardItem(thread_id)
              << new QStandardItem(stopped_threads)
                 ;

        m_asyncModel->removeRows(0,m_asyncModel->rowCount());
        m_asyncModel->appendRow(items);
        */
        if (QFile::exists(fullname)) {
            LiteApi::IEditor *editor = m_liteApp->fileManager()->openEditor(fullname,true);
            if (editor) {
                bool ok = false;
                int n = line.toInt(&ok);
                if (ok) {
                    LiteApi::ITextEditor *textEditor = LiteApi::findExtensionObject<LiteApi::ITextEditor*>(editor,"LiteApi.ITextEditor");
                    if (textEditor) {
                        textEditor->gotoLine(n,0);
                    }
                }
            }
        }
    }
}

void GdbDebugeer::handleLibrary(const GdbMiValue &result)
{
    QString id = result.findChild("id").data();
    QString thread_group = result.findChild("thread-group").data();
    m_libraryModel->appendRow(QList<QStandardItem*>()
                              << new QStandardItem(id)
                              << new QStandardItem(thread_group)
                              );
}

void GdbDebugeer::handleAsyncClass(const QByteArray &asyncClass, const GdbMiValue &result)
{
    m_asyncItem->removeRows(0,m_asyncItem->rowCount());
    m_asyncItem->setText(asyncClass);
    //QStandardItem *item = new QStandardItem(QString(asyncClass));
    GdbMiValueToItem(m_asyncItem,result);
    //m_asyncModel->clear();
    //m_asyncModel->appendRow(item);
    if (asyncClass == "stopped") {
        handleStopped(result);        
    } else if (asyncClass == "library-loaded") {
        handleLibrary(result);
    }
    //emit modelChanged(LiteApi::ASYNC_MODEL);
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
    if (response.cookie.type() != QVariant::Map) {
        return;
    }
    QMap<QString,QVariant> map = response.cookie.toMap();
    QString cmd = map.value("cmd").toString();
    if (cmd.isEmpty()) {
        return;
    }
    QStringList cmdList = map.value("cmdList").toStringList();
    if (cmdList.isEmpty()) {
        return;
    }
    if (cmdList.at(0) == "-stack-list-variables") {
        GdbMiValue vars = response.data.findChild("variables");
        if (vars.isList()) {
            foreach (const GdbMiValue &child, vars.m_children) {
                if (child.isValid()) {
                    QString var = child.findChild("name").data();
                    //QString value = child.findChild("value").data();
                    //QString type = child.findChild("type").data();
                    /*
                    if (child.findChild("arg").isValid()) {
                        m_argsItem->appendRow(new QStandardItem(QString("%1 = %2").arg(name).arg(value)));
                    } else {
                        m_localItem->appendRow(new QStandardItem(QString("%1 = %2").arg(name).arg(value)));
                    }
                    */
                    if (!m_varWatchMap.contains(var)) {
                        createWatch(var,true);
                    }
                }
            }
            emit modelChanged(LiteApi::VARS_MODEL);
        }
    } else if (cmdList.at(0) == "-stack-list-frames") {
        GdbMiValue stack = response.data.findChild("stack");
        if (stack.isList()) {
            m_framesModel->removeRows(0,m_framesModel->rowCount());
            for (int i = 0; i < stack.childCount(); i++) {
                GdbMiValue child = stack.childAt(i);
                if (child.isValid() && child.name() == "frame") {
                    QString level = child.findChild("level").data();
                    QString addr = child.findChild("addr").data();
                    QString func = child.findChild("func").data();
                    QString file = child.findChild("file").data();
                    QString line = child.findChild("line").data();
                    m_framesModel->appendRow(QList<QStandardItem*>()
                                             << new QStandardItem(level)
                                             << new QStandardItem(addr)
                                             << new QStandardItem(func)
                                             << new QStandardItem(file)
                                             << new QStandardItem(line)
                                             );
                }
            }
        }
    } else if (cmdList.at(0) == "-var-create") {
        //10000018^done,name="var4",numchild="0",value="4265530",type="int",thread-id="1",has_more="0"
        //10000019^done,name="var5",numchild="2",value="{...}",type="struct string",thread-id="1",has_more="0"
        //10000020^done,name="var6",numchild="3",value="0x40bc38",type="struct main.pt *",thread-id="1",has_more="0"
        QString name = response.data.findChild("name").data();
        QString numchild = response.data.findChild("numchild").data();
        QString value = response.data.findChild("value").data();
        QString type = response.data.findChild("type").data();
        QString var = map.value("var").toString();
        if (m_varWatchMap.contains(var)) {
            m_varWatchMap.insert(var,name);
            QStandardItem *item = new QStandardItem(var);
            item->setData(name);
            m_varsModel->appendRow(QList<QStandardItem*>()
                               << item
                               << new QStandardItem(value)
                               << new QStandardItem(type)
                               );
        }
    } else if (cmdList.at(0) == "-var-update") {
        m_handleState.setChangeList(true);
        //10000040^done,changelist=[{name="var2",in_scope="true",type_changed="false",has_more="0"}]
        GdbMiValue list = response.data.findChild("changelist");
        if (list.isList()) {
            for (int i = 0; i < list.childCount(); i++) {
                GdbMiValue child = list.childAt(i);
                if (child.isValid()) {
                    QString name = child.findChild("name").data();
                    QString in_scope = child.findChild("in_scope").data();
                    QString type_changed = child.findChild("type_changed").data();
                    QString var = m_varWatchMap.key(name);
                    if (in_scope == "false") {
                        removeWatch(var,false);
                    } else {
                        if (type_changed == "true") {
                            removeWatch(var,true);
                            QStringList args;
                            args << "-var-info-type";
                            args << name;
                            GdbCmd cmd;
                            cmd.setCmd(args);
                            cmd.insert("var",var);
                            cmd.insert("name",name);
                            command(cmd);
                        }
                        QStringList args;
                        args << "-var-evaluate-expression";
                        args << name;
                        GdbCmd cmd;
                        cmd.setCmd(args);
                        cmd.insert("var",var);
                        cmd.insert("name",name);
                        command(cmd);
                    }
                }
            }
        }
    } else if (cmdList.at(0) == "-var-delete") {
        //10000062^done,ndeleted="1"
        QString var = map.value("var").toString();
        QString name = map.value("name").toString();
        bool children = map.value("children").toBool();
        if (!children) {
            m_varWatchMap.remove(var);
        }
        for (int i = 0; i < m_varsModel->rowCount(); i++) {
            QStandardItem *item = m_varsModel->item(i,0);
            if (item->data() == name) {
                if (children) {
                    item->removeRows(0,item->rowCount());
                } else {
                    m_varsModel->removeRow(i);
                }
                break;
            }
        }
    } else if (cmdList.at(0) == "-var-evaluate-expression") {
        //10000035^done,value="100"
        QString value = response.data.findChild("value").data();
        QString var = map.value("var").toString();
        QString name = map.value("name").toString();
        for (int i = 0; i < m_varsModel->rowCount(); i++) {
            QModelIndex index = m_varsModel->index(i,0);
            if (index.data(Qt::UserRole+1) == name) {
                QModelIndex v = m_varsModel->index(i,1);
                m_varsModel->setData(v,value,Qt::DisplayRole);
                m_varsModel->setData(v,Qt::red,Qt::TextColorRole);
                break;
            }
        }
    } else if (cmdList.at(0) == "-var-info-type") {
        //10000060^done,type="struct string"
        QString type = response.data.findChild("type").data();
        QString var = map.value("var").toString();
        QString name = map.value("name").toString();
        for (int i = 0; i < m_varsModel->rowCount(); i++) {
            QModelIndex index = m_varsModel->index(i,0);
            if (index.data(Qt::UserRole+1) == name) {
                QModelIndex v = m_varsModel->index(i,2);
                m_varsModel->setData(v,type,Qt::DisplayRole);
                break;
            }
        }
    }
}


void GdbDebugeer::initGdb()
{
#ifdef Q_OS_WIN
    command("set new-console on");
#endif
    command("set unwindonsignal on");
    command("set overload-resolution off");
    command("handle SIGSEGV nopass stop print");
    command("set breakpoint pending on");
    command("set width 0");
    command("set height 0");
    command("set auto-solib-add on");
    if (!m_runtime.isEmpty()) {
        command("-environment-directory "+m_runtime);
        command("set substitute-path /go/src/pkg/runtime "+m_runtime);
    }
    command("break main.main");
    command("-exec-run");
}

void GdbDebugeer::updateWatch()
{
    command("-var-update *");
}

void GdbDebugeer::updateLocals()
{
    command("-stack-list-variables 0");
}

void GdbDebugeer::updateFrames()
{
    command("-stack-list-frames");
}

void GdbDebugeer::updateBreaks()
{
    command("-break-info");
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
    emit debugLog(m_inbuffer);
    m_inbuffer.clear();

    if (!m_gdbinit) {
        m_gdbinit = true;
        initGdb();
    }

    if (m_handleState.exited()) {
        stop();
    } else if (m_handleState.stopped()) {
        updateWatch();
        updateLocals();
        updateFrames();
    }

    m_handleState.clear();
}

void GdbDebugeer::finished(int)
{
    m_framesModel->removeRows(0,m_framesModel->rowCount());
   // m_asyncModel->removeRows(0,m_asyncModel->rowCount());
}
