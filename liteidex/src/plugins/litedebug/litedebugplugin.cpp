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
#include <QMenu>
#include <QLayout>
#include <QAction>

LiteDebugPlugin::LiteDebugPlugin()
{
    m_info->setId("plugin/LiteDebug");
    m_info->setName("LiteDebug");
    m_info->setAnchor("visualfc");
    m_info->setInfo("LiteIDE Debug Plugin");
}

bool LiteDebugPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }

    QLayout *layout = m_liteApp->editorManager()->widget()->layout();
    if (!layout) {
        return false;
    }

    QMenu *menu = m_liteApp->actionManager()->insertMenu("Debug",tr("&Debug"),"help");
    if (!menu) {
        return false;
    }

    m_liteDebug = new LiteDebug(app,this);
    m_liteDebug->widget()->hide();
    layout->addWidget(m_liteDebug->widget());

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

    connect(m_startDebugAct,SIGNAL(triggered()),this,SLOT(startDebug()));
    connect(m_stopDebugAct,SIGNAL(triggered()),this,SLOT(stopDebug()));

    return true;
}

void LiteDebugPlugin::startDebug()
{
    m_liteDebug->widget()->show();
}

void LiteDebugPlugin::stopDebug()
{
    m_liteDebug->widget()->hide();
}

Q_EXPORT_PLUGIN(LiteDebugPlugin)
