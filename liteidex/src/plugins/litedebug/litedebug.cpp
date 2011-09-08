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
    m_debug(0),
    m_liteBuild(0),
    m_envManager(0)
{
    m_manager->initWithApp(app);

    connect(m_manager,SIGNAL(currentDebugChanged(LiteApi::IDebug*)),this,SLOT(setDebug(LiteApi::IDebug*)));
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

void LiteDebug::setDebug(LiteApi::IDebug *debug)
{
    m_debug = debug;
    if (m_debug) {
        connect(m_debug,SIGNAL(debugStarted()),this,SIGNAL(debugStarted()));
        connect(m_debug,SIGNAL(debugStoped()),this,SIGNAL(debugStoped()));
    }
    m_widget->setDebug(m_debug);
}

void LiteDebug::startDebug()
{
    if (!m_debug) {
        return;
    }
    if (m_debug->isDebugging()) {
        return;
    }
    if (!m_liteBuild || !m_liteBuild->buildManager()->currentBuild()) {
        return;
    }
    if (!m_envManager) {
        return;
    }

    QString workDir = m_liteBuild->buildEnvMap().value("$WORKDIR");
    QString target = m_liteBuild->buildEnvMap().value("${TARGETPATH}");
    QString args = m_liteBuild->buildEnvMap().value("${TARGETARGS}");
    m_debug->setEnvironment(m_envManager->currentEnvironment().toStringList());
    m_debug->setWorkingDirectory(workDir);
    m_debug->start(target,args.split(" "));
}

void LiteDebug::stopDebug()
{
    if (!m_debug || !m_debug->isDebugging()) {
        return;
    }
    m_debug->stop();
}

void LiteDebug::abortDebug()
{
    if (!m_debug || !m_debug->isDebugging()) {
        return;
    }
    m_debug->abort();
}

void LiteDebug::stepOver()
{
    if (!m_debug || !m_debug->isDebugging()) {
        return;
    }
    m_debug->stepOver();
}

void LiteDebug::stepInto()
{
    if (!m_debug || !m_debug->isDebugging()) {
        return;
    }
    m_debug->stepInto();
}

void LiteDebug::stepOut()
{
    if (!m_debug || !m_debug->isDebugging()) {
        return;
    }
    m_debug->stepOut();
}
