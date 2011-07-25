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
// Module: golangcode.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-5-19
// $Id: golangcode.cpp,v 1.0 2011-7-25 visualfc Exp $

#include "golangcode.h"
#include "fileutil/fileutil.h"
#include "liteapi/litefindobj.h"
#include <QProcess>
#include <QTextDocument>
#include <QAbstractItemView>
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

GolangCode::GolangCode(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_completer(0)
{
    m_process = new QProcess(this);
    connect(m_process,SIGNAL(started()),this,SLOT(started()));
    connect(m_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finished(int,QProcess::ExitStatus)));

    m_gocodeCmd = m_liteApp->settings()->value("golangcode/cmd").toString();
    m_bLoad = false;
    m_envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
    if (m_envManager) {
        connect(m_envManager,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(currentEnvChanged(LiteApi::IEnv*)));
        currentEnvChanged(m_envManager->currentEnv());
    }
}

GolangCode::~GolangCode()
{
    if (!m_gocodeCmd.isEmpty()) {
        m_liteApp->settings()->setValue("golangcode/cmd",m_gocodeCmd);
        m_process->start(m_gocodeCmd,QStringList() << "close");
        m_process->waitForFinished(200);
    }
    delete m_process;
}

void GolangCode::currentEnvChanged(LiteApi::IEnv*)
{
    QProcessEnvironment env = m_envManager->currentEnvironment();
    QString goroot = env.value("GOROOT");
    QString gobin = env.value("GOBIN");
    if (!goroot.isEmpty() && gobin.isEmpty()) {
        gobin = goroot+"/bin";
    }
    QString gocode = FileUtil::findExecute(gobin+"/gocode");
    if (gocode.isEmpty()) {
        gocode = FileUtil::lookPath("gocode",env,true);
    }
    if (!FileUtil::compareFile(m_gocodeCmd,gocode)) {
        if (!m_gocodeCmd.isEmpty()) {
            m_process->start(m_gocodeCmd,QStringList() << "close");
            m_process->waitForFinished(200);
        }
    }
    m_process->setProcessEnvironment(env);
    m_gocodeCmd = gocode;
    if (!m_gocodeCmd.isEmpty()) {
        m_process->start(m_gocodeCmd);
    }
}

void GolangCode::setCompleter(LiteApi::ICompleter *completer)
{
    if (m_completer) {
        disconnect(m_completer,0,this,0);
    }
    m_completer = completer;
    if (m_completer) {
        connect(m_completer,SIGNAL(prefixChanged(QTextCursor,QString)),this,SLOT(prefixChanged(QTextCursor,QString)));
    }
}

void GolangCode::prefixChanged(QTextCursor cur,QString pre)
{
    if (m_gocodeCmd.isEmpty()) {
        return;
    }

    if (pre.right(1) != ".") {
        return;
    }
    QString src = cur.document()->toPlainText();
    src = src.replace("\r\n","\n");
    m_writeData = src.left(cur.position()).toUtf8();
    QStringList args;
    args << "-in" << "" << "-f" << "csv" << "autocomplete" << QString::number(m_writeData.length());
    m_writeData = src.toUtf8();
    m_prefix = pre;

    m_process->start(m_gocodeCmd,args);
}

void GolangCode::started()
{
    if (m_writeData.isEmpty()) {
        return;
    }
    m_process->write(m_writeData);
    m_process->closeWriteChannel();
}

void GolangCode::finished(int,QProcess::ExitStatus)
{
    if (m_writeData.isEmpty()) {
        return;
    }
    m_writeData.clear();

    QString read = m_process->readAllStandardOutput();
    QStringList all = read.split('\n');
    //func,,Fprint,,func(w io.Writer, a ...interface{}) (n int, error os.Error)
    //type,,Formatter,,interface
    int n = 0;
    foreach (QString s, all) {
        QStringList word = s.split(",,");
        QString item,param;
        //classes, names, types
        if (word.count() == 3) {
            if (word.at(0) == "type" || word.at(0) == "func") {
                item = word.at(1);
            }
            if (word.at(0) == "func") {
                int pos = word.at(2).indexOf("(");
                if (pos != -1) {
                    param = word.at(2).right(word.at(2).length()-pos);
                }
            }
        }
        if (!item.isEmpty()) {
            if (m_completer->appendItem(m_prefix+item+param,true)) {
                n++;
            }
        }
    }
    if (n >= 1) {
        m_completer->show();
    }
}
