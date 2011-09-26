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
#include <QFile>

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
    m_debugger(0)
{
    m_tabWidget = new QTabWidget;

    m_asyncView = new QTreeView;
    m_varsView = new QTreeView;
    m_statckView = new QTreeView;
    m_libraryView = new QTreeView;

    m_asyncView->setEditTriggers(0);

    m_varsView->setEditTriggers(0);
    m_statckView->setEditTriggers(0);
    m_libraryView->setEditTriggers(0);

    m_debugLogEdit = new TerminalEdit;
    m_debugLogEdit->setReadOnly(false);
    m_debugLogEdit->setMaximumBlockCount(1024);

    m_tabWidget->addTab(m_asyncView,tr("AsyncRecord"));
    m_tabWidget->addTab(m_varsView,tr("Variables"));
    m_tabWidget->addTab(m_statckView,tr("CallStack"));
    m_tabWidget->addTab(m_libraryView,tr("Library"));
    m_tabWidget->addTab(m_debugLogEdit,tr("Console"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(m_tabWidget);

    m_widget->setLayout(layout);

    connect(m_debugLogEdit,SIGNAL(enterText(QString)),this,SLOT(enterText(QString)));
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

void DebugWidget::enterText(const QString &text)
{
    QString cmd = text.simplified();
    if (!cmd.isEmpty() && m_debugger && m_debugger->isRunning()) {
        m_debugger->command(cmd.toUtf8());
    }
}

void DebugWidget::clearLog()
{
    m_debugLogEdit->clear();
}

void DebugWidget::appendLog(const QString &log)
{
    m_debugLogEdit->append(log);
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

void DebugWidget::setDebugger(LiteApi::IDebugger *debug)
{
    if (m_debugger == debug) {
        return;
    }
    m_debugger = debug;
    if (!m_debugger) {
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
    connect(m_debugger,SIGNAL(setExpand(LiteApi::DEBUG_MODEL_TYPE,QModelIndex,bool)),this,SLOT(setExpand(LiteApi::DEBUG_MODEL_TYPE,QModelIndex,bool)));
}

void DebugWidget::expandedVarsView(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    if (!m_debugger) {
        return;
    }
    m_debugger->expandItem(index,LiteApi::VARS_MODEL);
}
void DebugWidget::setExpand(LiteApi::DEBUG_MODEL_TYPE type, const QModelIndex &index, bool expanded)
{
    if (!index.isValid()) {
        return;
    }
    if (!m_debugger) {
        return;
    }
    QTreeView *view = 0;
    switch (type) {
    case LiteApi::VARS_MODEL:
        view = m_varsView;
        break;
    case LiteApi::ASYNC_MODEL:
        view = m_asyncView;
        break;
    case LiteApi::CALLSTACK_MODEL:
        view = m_statckView;
        break;
    case LiteApi::LIBRARY_MODEL:
        view = m_libraryView;
        break;
    default:
        view = 0;
    }

    if (view) {
        view->setExpanded(index,expanded);
    }
}
