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
// Module: litedebug.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: litedebug.h,v 1.0 2011-8-12 visualfc Exp $

#ifndef LITEDEBUG_H
#define LITEDEBUG_H

#include "liteapi/liteapi.h"
#include "litedebugapi/litedebugapi.h"
#include "liteenvapi/liteenvapi.h"
#include "litebuildapi/litebuildapi.h"
#include "liteeditorapi/liteeditorapi.h"

class DebugManager;
class DebugWidget;
class QLabel;
class LiteDebug : public QObject
{
    Q_OBJECT
public:
    explicit LiteDebug(LiteApi::IApplication *app, QObject *parent = 0);
    QWidget *widget();
signals:
    void debugVisible(bool);
public slots:
    void appLoaded();
    void startDebug();
    void execContinue();
    void runToLine();
    void stopDebug();
    void abortDebug();
    void stepOver();
    void stepInto();
    void stepOut();
protected slots:
    void showDebug();
    void hideDebug();
    void setDebugger(LiteApi::IDebugger*);
protected:
    LiteApi::IApplication *m_liteApp;
    DebugManager *m_manager;
    QWidget      *m_widget;
    DebugWidget  *m_dbgWidget;
    QToolBar     *m_toolBar;
    LiteApi::IDebugger *m_debugger;
    LiteApi::ILiteBuild *m_liteBuild;
    LiteApi::IEnvManager *m_envManager;
    QAction *m_startDebugAct;
    QAction *m_stopDebugAct;
    QAction *m_abortDebugAct;
    QAction *m_execContinueAct;
    QAction *m_runToLineAct;
    QAction *m_stepOverAct;
    QAction *m_stepIntoAct;
    QAction *m_stepOutAct;
    QLabel *m_infoLabel;
    QAction *m_hideAct;
};

#endif // LITEDEBUG_H
