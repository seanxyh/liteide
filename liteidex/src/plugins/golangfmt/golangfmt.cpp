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
// Module: golangfmt.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: golangfmt.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "golangfmt.h"
#include "fileutil/fileutil.h"
#include "processex/processex.h"
#include "liteapi/litefindobj.h"

#include <QDebug>
#include <QProcess>
#include <QFileInfo>
#include <QPlainTextEdit>
#include <QTextCodec>
#include <QTextCursor>
#include <QTextBlock>
#include <QScrollBar>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

GolangFmt::GolangFmt(LiteApi::IApplication *app,QObject *parent) :
    QObject(parent),
    m_liteApp(app)
{
    m_process = new ProcessEx(this);
    connect(m_process,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(fmtOutput(QByteArray,bool)));
    connect(m_process,SIGNAL(extFinish(bool,int,QString)),this,SLOT(fmtFinish(bool,int,QString)));

    m_envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
    if (m_envManager) {
        connect(m_envManager,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(currentEnvChanged(LiteApi::IEnv*)));
        currentEnvChanged(m_envManager->currentEnv());
    }
}

void GolangFmt::currentEnvChanged(LiteApi::IEnv*)
{
    QProcessEnvironment env = m_envManager->currentEnvironment();
    QString goroot = env.value("GOROOT");
    QString gobin = env.value("GOBIN");
    if (!goroot.isEmpty() && gobin.isEmpty()) {
        gobin = goroot+"/bin";
    }
    QString gofmt = FileUtil::findExecute(gobin+"/gofmt");
    if (gofmt.isEmpty()) {
        gofmt = FileUtil::lookPath("gofmt",env,true);
    }
    m_process->setProcessEnvironment(env);
    m_gofmtCmd = gofmt;
}

void GolangFmt::gofmt()
{
    if (m_process->isRuning()) {
        return;
    }

    if (m_gofmtCmd.isEmpty()) {
        return;
    }

    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    QString fileName = editor->filePath();
    if (fileName.isEmpty()) {
        return;
    }
    if (editor->isModified() && !editor->isReadOnly()) {
        m_liteApp->editorManager()->saveEditor(editor);
    }
    QStringList args;
    args << fileName;
    m_data.clear();;
    m_process->setUserData(0,fileName);
    m_process->start(m_gofmtCmd,args);
}

void GolangFmt::fmtOutput(QByteArray data,bool stdErr)
{
    if (stdErr) {
        return;
    }
    m_data.append(data);
}

void GolangFmt::fmtFinish(bool error,int code,QString /*msg*/)
{
    if (!error && code == 0) {
        QString fileName = m_process->userData(0).toString();
        if (!fileName.isEmpty()) {
            LiteApi::IEditor *editor = m_liteApp->fileManager()->openEditor(fileName);
            if (editor) {
                QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
                QTextCodec *codec = QTextCodec::codecForName("utf-8");
                if (ed && codec) {
                    int vpos = -1;
                    QScrollBar *bar = ed->verticalScrollBar();
                    if (bar) {
                        vpos = bar->sliderPosition();
                    }
                    QTextCursor cur = ed->textCursor();
                    int pos = cur.position();
                    cur.beginEditBlock();
                    cur.select(QTextCursor::Document);
                    cur.removeSelectedText();
                    cur.insertText(codec->toUnicode(m_data));
                    cur.setPosition(pos);
                    cur.endEditBlock();
                    ed->setTextCursor(cur);
                    if (vpos != -1) {
                        bar->setSliderPosition(vpos);
                    }
                }
            }
        }
    }
    m_data.clear();
}

