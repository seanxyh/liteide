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
// Module: outputmanager.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: outputmanager.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "outputmanager.h"
#include "mainwindow.h"

#include <QStackedWidget>
#include <QToolBar>
#include <QActionGroup>
#include <QPushButton>
#include <QHBoxLayout>
#include <QToolButton>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

OutputManager::~OutputManager()
{
    QMutableMapIterator<QWidget*,QToolButton*> i(m_widgetButtonMap);
    while (i.hasNext()) {
        i.next();
        delete i.value();
        i.remove();
    }
}

bool OutputManager::initWithApp(IApplication *app)
{
    if (!IOutputManager::initWithApp(app)) {
        return false;
    }
    m_stackedWidget = new QStackedWidget(m_liteApp->mainWindow());

    m_outputToolBar = new QToolBar("Output Panes",m_liteApp->mainWindow());
    m_outputToolBar->setObjectName("OutputPanes");
    m_outputActGroup = new QActionGroup(this);

    connect(m_outputActGroup,SIGNAL(triggered(QAction*)),this,SLOT(selectedOutputAct(QAction*)));

    m_buttonsWidget = new QWidget(m_outputToolBar);
    m_buttonsLayout = new QHBoxLayout;
    m_buttonsLayout->setMargin(0);
    m_buttonsLayout->setSpacing(0);
    m_buttonsWidget->setLayout(m_buttonsLayout);

    m_stackedWidget->hide();
    m_outputToolBar->addWidget(m_buttonsWidget);
    return true;
}

QWidget *OutputManager::widget()
{
    return m_stackedWidget;
}

QToolBar *OutputManager::toolBar()
{
    return m_outputToolBar;
}

void OutputManager::addOutuput(QWidget *w, const QString &label)
{
    addOutuput(w,QIcon(),label);
}

void OutputManager::addOutuput(QWidget *w, const QIcon &icon, const QString &label)
{
    QAction *act = m_outputActGroup->addAction(icon,label);
    act->setCheckable(true);
    //act->setShortcut(QKeySequence(Qt::ALT+Qt::Key_1+count));
    //act->setToolTip(QString("%1 Alt+%2").arg(label).arg(count+1));
    m_widgetActionMap.insert(w,act);
    m_stackedWidget->addWidget(w);
    QToolButton *btn = new QToolButton(m_outputToolBar);
    btn->setDefaultAction(act);
    m_buttonsLayout->addWidget(btn);
    m_widgetButtonMap.insert(w,btn);
}

void OutputManager::removeOutput(QWidget *w)
{
    if (!w) {
        return;
    }
    QToolButton *btn = m_widgetButtonMap.value(w);
    if (btn) {
        m_buttonsLayout->removeWidget(btn);
    }
    m_widgetActionMap.remove(w);
    m_stackedWidget->removeWidget(w);
}

void OutputManager::showOutput(QWidget *w)
{
    QToolButton *btn = m_widgetButtonMap.value(w);
    if (btn) {
        btn->show();
    }
}

void OutputManager::hideOutput(QWidget *w)
{
    QToolButton *btn = m_widgetButtonMap.value(w);
    if (btn) {
        btn->hide();
        if (m_lastOutputAct == btn->defaultAction()) {
            m_lastOutputAct->setChecked(false);
            m_stackedWidget->hide();
            m_lastOutputAct = NULL;
        }
    }
}


QWidget *OutputManager::currentOutput()
{
    return m_stackedWidget->currentWidget();
}

void OutputManager::setCurrentOutput(QWidget *w)
{
    if (w) {
        if (m_stackedWidget->isHidden()) {
            m_stackedWidget->show();
        }
        m_stackedWidget->setCurrentWidget(w);
        QAction *act = m_widgetActionMap.value(w);
        m_lastOutputAct = act;
        if (act && !act->isChecked()) {
            act->setChecked(true);
        }
    } else {
        m_stackedWidget->hide();
        if (m_lastOutputAct) {
            m_lastOutputAct->setChecked(false);
        }
        m_lastOutputAct = NULL;
    }
    emit currentOutputChanged(w);
}

void OutputManager::selectedOutputAct(QAction *act)
{
    if (m_lastOutputAct == act) {
        act->setChecked(false);
        m_lastOutputAct = NULL;
    } else {
        m_lastOutputAct = act;
    }
    if (m_lastOutputAct == NULL) {
        m_stackedWidget->hide();
    } else  {
        QWidget *w = m_widgetActionMap.key(act);
        setCurrentOutput(w);
    }
}
