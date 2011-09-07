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

#include <QLayout>
#include <QMenu>
#include <QAction>

LiteDebug::LiteDebug(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_manager(new DebugManager(this)),
    m_widget(new DebugWidget(app,this)),
    m_debug(0)
{
    m_manager->initWithApp(app);

    connect(m_manager,SIGNAL(currentDebugChanged(LiteApi::IDebug*)),m_widget,SLOT(setDebug(LiteApi::IDebug*)));

    m_liteApp->extension()->addObject("LiteApi.IDebugManager",m_manager);
}

QWidget *LiteDebug::widget()
{
    return m_widget->widget();
}

void LiteDebug::startDebug()
{
    if (!m_debug) {
        return;
    }
    if (m_debug->isDebugging()) {
        return;
    }
    m_debug->start();
}

void LiteDebug::stopDebug()
{
    if (!m_debug || !m_debug->isDebugging()) {
        return;
    }
    m_debug->stop();
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
