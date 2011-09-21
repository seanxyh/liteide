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
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QHeaderView>
#include <QPlainTextEdit>
#include <QVariant>
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

DebugWidget::DebugWidget(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    m_debug(0)
{
    m_tabWidget = new QTabWidget;

    m_asyncView = new QTreeView;
    m_varsView = new QTreeView;
    m_statckView = new QTreeView;
    m_libraryView = new QTreeView;

    //m_watchesView = new QTreeView;
    //m_bkpointView = new QTreeView;
    //m_threadsView = new QTreeView;

    m_asyncView->setEditTriggers(0);

    m_varsView->setEditTriggers(0);
    m_statckView->setEditTriggers(0);
    m_libraryView->setEditTriggers(0);
   // m_watchesView->setEditTriggers(0);
   // m_bkpointView->setEditTriggers(0);
   // m_threadsView->setEditTriggers(0);

    m_cmdLineEdit = new QLineEdit;
    m_debugLogEdit = new QPlainTextEdit;
    m_debugLogEdit->setReadOnly(true);
    QPushButton *clearBtn = new QPushButton(tr("Clear"));
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(m_cmdLineEdit);
    hLayout->addWidget(clearBtn);
    QVBoxLayout *cmdLayout = new QVBoxLayout;
    cmdLayout->addLayout(hLayout);
    cmdLayout->addWidget(m_debugLogEdit);
    QWidget *cmdWidget = new QWidget;
    cmdWidget->setLayout(cmdLayout);

    m_tabWidget->addTab(m_asyncView,tr("AsyncRecord"));
    m_tabWidget->addTab(m_varsView,tr("Variables"));
    m_tabWidget->addTab(m_statckView,tr("CallStack"));
    m_tabWidget->addTab(m_libraryView,tr("Library"));
//    m_tabWidget->addTab(m_watchesView,tr("Watches"));
//    m_tabWidget->addTab(m_bkpointView,tr("BreakPoints"));
//    m_tabWidget->addTab(m_threadsView,tr("Threads"));
    m_tabWidget->addTab(cmdWidget,tr("Console"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(m_tabWidget);

    m_widget->setLayout(layout);

    connect(m_cmdLineEdit,SIGNAL(returnPressed()),this,SLOT(cmdLineInput()));
    connect(clearBtn,SIGNAL(clicked()),this,SLOT(clearLog()));
    connect(m_varsView,SIGNAL(expanded(QModelIndex)),this,SLOT(expandedVarsView(QModelIndex)));
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

void DebugWidget::cmdLineInput()
{
    QString text = m_cmdLineEdit->text();
    if (!text.isEmpty() && m_debug && m_debug->isDebugging()) {
        m_debug->command(text.toUtf8());
    }
}

void DebugWidget::clearLog()
{
    m_debugLogEdit->clear();
}

void DebugWidget::debugLog(const QByteArray &log)
{
    m_debugLogEdit->appendPlainText(log);
    m_debugLogEdit->moveCursor(QTextCursor::End);
}

static void setResizeView(QTreeView *view)
{
    QAbstractItemModel *model = view->model();
    if (!model) {
        return;
    }
    if (model->columnCount() <= 1) {
        view->setHeaderHidden(true);
        return;
    }
    view->header()->setResizeMode(0,QHeaderView::ResizeToContents);
}

void DebugWidget::setDebug(LiteApi::IDebugger *debug)
{
    if (m_debug == debug) {
        return;
    }
    m_debug = debug;
    if (!m_debug) {
        return;
    }
    m_asyncView->setModel(debug->debugModel(LiteApi::ASYNC_MODEL));
    m_varsView->setModel(debug->debugModel(LiteApi::VARS_MODEL));
    m_statckView->setModel(debug->debugModel(LiteApi::CALLSTACK_MODEL));
    m_libraryView->setModel(debug->debugModel(LiteApi::LIBRARY_MODEL));
    setResizeView(m_asyncView);
    setResizeView(m_varsView);
    setResizeView(m_statckView);
    setResizeView(m_libraryView);
    connect(m_debug,SIGNAL(setExpand(LiteApi::DEBUG_MODEL_TYPE,QModelIndex,bool)),this,SLOT(setExpand(LiteApi::DEBUG_MODEL_TYPE,QModelIndex,bool)));
}

void DebugWidget::expandedVarsView(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    if (!m_debug) {
        return;
    }
    m_debug->expandItem(index,LiteApi::VARS_MODEL);
}
void DebugWidget::setExpand(LiteApi::DEBUG_MODEL_TYPE type, const QModelIndex &index, bool expanded)
{
    if (!index.isValid()) {
        return;
    }
    if (!m_debug) {
        return;
    }
    QTreeView *view = 0;
    if (type == LiteApi::VARS_MODEL) {
        view = m_varsView;
    }
    if (view) {
        view->setExpanded(index,expanded);
    }
}
