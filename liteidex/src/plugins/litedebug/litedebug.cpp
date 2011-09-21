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
// Module: litedebug.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: litedebug.cpp,v 1.0 2011-8-12 visualfc Exp $

#include "litedebug.h"
#include "debugmanager.h"
#include "debugwidget.h"
#include "liteapi/litefindobj.h"
#include "fileutil/fileutil.h"


#include <QLayout>
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QPushButton>
#include <QLabel>
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

LiteDebug::LiteDebug(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_manager(new DebugManager(this)),
    m_widget(new QWidget),
    m_debugger(0),
    m_liteBuild(0),
    m_envManager(0)
{
    m_manager->initWithApp(app);

    m_dbgWidget = new DebugWidget(app,this);
    m_toolBar = new QToolBar;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_toolBar);
    layout->addWidget(m_dbgWidget->widget());
    m_widget->setLayout(layout);

    m_startDebugAct = new QAction(tr("Start"),this);
    m_execContinueAct = new QAction(tr("Continue"),this);
    m_runToLineAct = new QAction(tr("RunToLine"),this);
    m_stopDebugAct = new QAction(tr("Stop"),this);
    m_abortDebugAct = new QAction(tr("Abort"),this);
    m_stepOverAct = new QAction(tr("StepOver"),this);
    m_stepOverAct->setShortcut(QKeySequence(Qt::Key_F10));
    m_stepIntoAct = new QAction(tr("StepInto"),this);
    m_stepIntoAct->setShortcut(QKeySequence(Qt::Key_F11));
    m_stepOutAct = new QAction(tr("StepOut"),this);
    m_stepOutAct->setShortcut(QKeySequence(Qt::SHIFT+Qt::Key_F11));

    m_hideAct = new QAction(tr("Hide"),this);
    m_infoLabel = new QLabel;
    m_infoLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    m_toolBar->addAction(m_startDebugAct);
    m_toolBar->addAction(m_execContinueAct);
    m_toolBar->addAction(m_stopDebugAct);
    m_toolBar->addAction(m_abortDebugAct);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_runToLineAct);
    m_toolBar->addAction(m_stepOverAct);
    m_toolBar->addAction(m_stepIntoAct);
    m_toolBar->addAction(m_stepOutAct);

    m_toolBar->addWidget(m_infoLabel);
    m_toolBar->addAction(m_hideAct);

    QMenu *menu = m_liteApp->actionManager()->insertMenu("Debug",tr("&Debug"),"help");
    if (menu) {
        menu->addAction(m_startDebugAct);
        menu->addAction(m_execContinueAct);
        menu->addAction(m_stopDebugAct);
        menu->addAction(m_abortDebugAct);
        menu->addSeparator();
        menu->addAction(m_stepOverAct);
        menu->addAction(m_stepIntoAct);
        menu->addAction(m_stepOutAct);
    }

    connect(m_manager,SIGNAL(currentDebuggerChanged(LiteApi::IDebugger*)),this,SLOT(setDebugger(LiteApi::IDebugger*)));
    connect(m_liteApp,SIGNAL(loaded()),this,SLOT(appLoaded()));

    connect(m_startDebugAct,SIGNAL(triggered()),this,SLOT(startDebug()));
    connect(m_execContinueAct,SIGNAL(triggered()),this,SLOT(execContinue()));
    connect(m_runToLineAct,SIGNAL(triggered()),this,SLOT(runToLine()));
    connect(m_stopDebugAct,SIGNAL(triggered()),this,SLOT(stopDebug()));
    connect(m_abortDebugAct,SIGNAL(triggered()),this,SLOT(abortDebug()));
    connect(m_stepOverAct,SIGNAL(triggered()),this,SLOT(stepOver()));
    connect(m_stepIntoAct,SIGNAL(triggered()),this,SLOT(stepInto()));
    connect(m_stepOutAct,SIGNAL(triggered()),this,SLOT(stepOut()));
    connect(m_hideAct,SIGNAL(triggered()),this,SLOT(hideDebug()));

    m_liteApp->extension()->addObject("LiteApi.IDebugManager",m_manager);    
}

void LiteDebug::appLoaded()
{
    m_liteBuild = LiteApi::findExtensionObject<LiteApi::ILiteBuild*>(m_liteApp,"LiteApi.ILiteBuild");
    m_envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");

    LiteApi::IEditorMarkTypeManager *markTypeManager = LiteApi::findExtensionObject<LiteApi::IEditorMarkTypeManager*>(m_liteApp,"LiteApi.IEditorMarkTypeManager");
    if (markTypeManager) {
        markTypeManager->registerMark(BrackPointMark,QIcon(":/images/brackpoint.png"));
        markTypeManager->registerMark(CurrentLineMark,QIcon(":/images/currentline.png"));
    }
}

QWidget *LiteDebug::widget()
{
    return m_widget;
}

void LiteDebug::showDebug()
{
    m_widget->show();
    emit debugVisible(true);
}

void LiteDebug::hideDebug()
{
    m_widget->hide();
    emit debugVisible(false);
}

void LiteDebug::setDebugger(LiteApi::IDebugger *debug)
{
    m_debugger = debug;
    if (m_debugger) {
        connect(m_debugger,SIGNAL(debugStarted()),this,SLOT(showDebug()));
        connect(m_debugger,SIGNAL(debugStoped()),this,SLOT(debugStoped()));
        connect(m_debugger,SIGNAL(debugLog(QByteArray)),m_dbgWidget,SLOT(debugLog(QByteArray)));
        connect(m_debugger,SIGNAL(setCurrentLine(QString,int)),this,SLOT(setCurrentLine(QString,int)));
    }
    m_dbgWidget->setDebug(m_debugger);
}

void LiteDebug::startDebug()
{
    if (!m_debugger) {
        return;
    }
    if (m_debugger->isDebugging()) {
        return;
    }
    if (!m_liteBuild || !m_liteBuild->buildManager()->currentBuild()) {
        return;
    }
    if (!m_envManager) {
        return;
    }
    m_dbgWidget->clearLog();

    QString targetFilepath = m_liteBuild->targetFilePath();
    if (targetFilepath.isEmpty() || !QFile::exists(targetFilepath)) {
        return;
    }

    QString workDir = m_liteBuild->buildEnvMap().value("${WORKDIR}");
    QString target = m_liteBuild->buildEnvMap().value("${TARGETNAME}");
    QString args = m_liteBuild->buildEnvMap().value("${TARGETARGS}");
    int index = targetFilepath.lastIndexOf(target);
    if (index != -1) {
        target = targetFilepath.right(targetFilepath.length()-index);
    }
    m_debugger->setEnvironment(m_envManager->currentEnvironment().toStringList());
    m_debugger->setWorkingDirectory(workDir);
    m_debugger->start(target,args.split(" "));
}

void LiteDebug::execContinue()
{
    if (!m_debugger || !m_debugger->isDebugging()) {
        return;
    }
    m_debugger->execContinue();
}

void LiteDebug::runToLine()
{
    if (!m_debugger || !m_debugger->isDebugging()) {
        return;
    }
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    LiteApi::ITextEditor *textEditor = LiteApi::findExtensionObject<LiteApi::ITextEditor*>(editor,"LiteApi.ITextEditor");
    if (!textEditor) {
        return;
    }
    QString filePath = textEditor->fileName();
    if (filePath.isEmpty()) {
        return;
    }
    QString fileName = QFileInfo(filePath).fileName();
    int line = textEditor->line()+1;
    m_debugger->runToLine(fileName,line);
}

void LiteDebug::stopDebug()
{
    if (!m_debugger || !m_debugger->isDebugging()) {
        return;
    }
    m_debugger->stop();
}

void LiteDebug::abortDebug()
{
    if (!m_debugger || !m_debugger->isDebugging()) {
        return;
    }
    m_debugger->abort();
}

void LiteDebug::stepOver()
{
    if (!m_debugger || !m_debugger->isDebugging()) {
        return;
    }
    m_debugger->stepOver();
}

void LiteDebug::stepInto()
{
    if (!m_debugger || !m_debugger->isDebugging()) {
        return;
    }
    m_debugger->stepInto();
}

void LiteDebug::stepOut()
{
    if (!m_debugger || !m_debugger->isDebugging()) {
        return;
    }
    m_debugger->stepOut();
}

void LiteDebug::clearLastLine()
{
    if (!m_last.fileName.isEmpty()) {
        LiteApi::IEditor *lastEditor = m_liteApp->editorManager()->findEditor(m_last.fileName,true);
        if (lastEditor) {
            LiteApi::IEditorMark *lastMark = LiteApi::findExtensionObject<LiteApi::IEditorMark*>(lastEditor,"LiteApi.IEditorMark");
            if (lastMark) {
                lastMark->removeMark(m_last.line,LiteApi::CurrentLineMark);
            }
        }
    }
    m_last.fileName.clear();
}

void LiteDebug::debugStoped()
{
    clearLastLine();
}

void LiteDebug::setCurrentLine(const QString &fileName, int line)
{
    bool center = true;
    if (m_last.fileName == fileName) {
        center = false;
    }
    clearLastLine();
    if (QFile::exists(fileName)) {
        LiteApi::IEditor *editor = m_liteApp->fileManager()->openEditor(fileName,true);
        if (editor) {
            m_last.fileName = fileName;
            m_last.line = line;
            LiteApi::ITextEditor *textEditor = LiteApi::findExtensionObject<LiteApi::ITextEditor*>(editor,"LiteApi.ITextEditor");
            if (textEditor) {
                textEditor->gotoLine(line,0,center);
            }
            LiteApi::IEditorMark *editMark = LiteApi::findExtensionObject<LiteApi::IEditorMark*>(editor,"LiteApi.IEditorMark");
            if (editMark) {
                editMark->addMark(line,LiteApi::CurrentLineMark);
            }
        }
    }
}

