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
// Module: debugwidget.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: debugwidget.cpp,v 1.0 2011-8-12 visualfc Exp $

#include "debugwidget.h"
#include <QTreeView>
#include <QTabWidget>
#include <QVBoxLayout>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

DebugWidget::DebugWidget(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    m_debug(0)
{
    m_tabWidget = new QTabWidget;

    m_localsView = new QTreeView;
    m_watchesView = new QTreeView;
    m_statckView = new QTreeView;
    m_bkpointView = new QTreeView;
    m_threadsView = new QTreeView;

    m_tabWidget->addTab(m_localsView,tr("Locals"));
    m_tabWidget->addTab(m_watchesView,tr("Watches"));
    m_tabWidget->addTab(m_statckView,tr("CallStack"));
    m_tabWidget->addTab(m_bkpointView,tr("BreakPoints"));
    m_tabWidget->addTab(m_threadsView,tr("Threads"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_tabWidget);

    m_widget->hide();
    m_widget->setLayout(layout);
}

DebugWidget::~DebugWidget()
{
    if (m_widget) {
        delete m_widget;
    }
}

QWidget *DebugWidget::widget()
{
    return m_widget;
}

void DebugWidget::setDebug(LiteApi::IDebug *debug)
{
    if (m_debug == debug) {
        return;
    }
    m_debug = debug;
    if (!m_debug) {
        return;
    }
    m_localsView->setModel(debug->debugModel(LiteApi::LOCALS_MODEL));
    m_watchesView->setModel(debug->debugModel(LiteApi::WATCHES_MODEL));
    m_statckView->setModel(debug->debugModel(LiteApi::CALLSTACK_MODEL));
    m_bkpointView->setModel(debug->debugModel(LiteApi::BREAKPOINTS_MODEL));
    m_threadsView->setModel(debug->debugModel(LiteApi::THREADS_MODEL));
}
