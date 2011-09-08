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
// Module: litedebugplugin.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: litedebugplugin.cpp,v 1.0 2011-8-12 visualfc Exp $

#include "litedebugplugin.h"
#include "litedebug.h"
#include "liteapi/litefindobj.h"
#include <QMenu>
#include <QLayout>
#include <QAction>
#include <QSplitter>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

LiteDebugPlugin::LiteDebugPlugin()
{
    m_info->setId("plugin/LiteDebug");
    m_info->setName("LiteDebug");
    m_info->setAnchor("visualfc");
    m_info->setInfo("LiteIDE Debug Manager Plugin");
}

QStringList LiteDebugPlugin::dependPluginList() const
{
    return QStringList() << "plugin/litebuild" <<"plugin/litefind";
}

bool LiteDebugPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }

    QSplitter *splitter = LiteApi::findExtensionObject<QSplitter*>(m_liteApp,"LiteApi.QMainWindow.QSplitter");
    if (!splitter) {
        return false;
    }

    QMenu *menu = m_liteApp->actionManager()->insertMenu("Debug",tr("&Debug"),"help");
    if (!menu) {
        return false;
    }

    m_liteDebug = new LiteDebug(app,this);
    m_liteDebug->widget()->hide();
    int index = splitter->indexOf(m_liteApp->outputManager()->widget());
    splitter->insertWidget(index,m_liteDebug->widget());

    m_startDebugAct = new QAction(tr("Start Debugging"),this);
    m_stopDebugAct = new QAction(tr("Stop Debugger"),this);
    m_abortDebugAct = new QAction(tr("Abort Debugging"),this);
    m_stepOverAct = new QAction(tr("Step Over"),this);
    m_stepIntoAct = new QAction(tr("Step Into"),this);
    m_stepOutAct = new QAction(tr("Step Out"),this);

    menu->addAction(m_startDebugAct);
    menu->addAction(m_stopDebugAct);
    menu->addAction(m_abortDebugAct);
    menu->addSeparator();
    menu->addAction(m_stepOverAct);
    menu->addAction(m_stepIntoAct);
    menu->addAction(m_stepOutAct);

    connect(m_startDebugAct,SIGNAL(triggered()),m_liteDebug,SLOT(startDebug()));
    connect(m_stopDebugAct,SIGNAL(triggered()),m_liteDebug,SLOT(stopDebug()));
    connect(m_abortDebugAct,SIGNAL(triggered()),m_liteDebug,SLOT(abortDebug()));
    connect(m_stepOverAct,SIGNAL(triggered()),m_liteDebug,SLOT(stepOver()));
    connect(m_stepIntoAct,SIGNAL(triggered()),m_liteDebug,SLOT(stepInto()));
    connect(m_stepOutAct,SIGNAL(triggered()),m_liteDebug,SLOT(stepOut()));

    connect(m_liteDebug,SIGNAL(debugStarted()),m_liteDebug->widget(),SLOT(show()));
    connect(m_liteDebug,SIGNAL(debugStoped()),m_liteDebug->widget(),SLOT(hide()));

    return true;
}

Q_EXPORT_PLUGIN(LiteDebugPlugin)
