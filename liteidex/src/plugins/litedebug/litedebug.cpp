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
#include <QAction>
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
    m_widget(new DebugWidget(app,this)),
    m_debugger(0),
    m_liteBuild(0),
    m_envManager(0)
{
    m_manager->initWithApp(app);

    connect(m_manager,SIGNAL(currentDebuggerChanged(LiteApi::IDebugger*)),this,SLOT(setDebugger(LiteApi::IDebugger*)));
    connect(m_liteApp,SIGNAL(loaded()),this,SLOT(appLoaded()));
    m_liteApp->extension()->addObject("LiteApi.IDebugManager",m_manager);
}

void LiteDebug::appLoaded()
{
    m_liteBuild = LiteApi::findExtensionObject<LiteApi::ILiteBuild*>(m_liteApp,"LiteApi.ILiteBuild");
    m_envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
}

QWidget *LiteDebug::widget()
{
    return m_widget->widget();
}

void LiteDebug::setDebugger(LiteApi::IDebugger *debug)
{
    m_debugger = debug;
    if (m_debugger) {
        connect(m_debugger,SIGNAL(debugStarted()),this,SIGNAL(debugStarted()));
        connect(m_debugger,SIGNAL(debugStoped()),this,SIGNAL(debugStoped()));
    }
    m_widget->setDebug(m_debugger);
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
    QString targetFilepath = m_liteBuild->targetFilePath();
    if (targetFilepath.isEmpty()) {
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
