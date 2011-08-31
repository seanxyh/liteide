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
// Module: actionmanager.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: actionmanager.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "actionmanager.h"

#include <QMenuBar>
#include <QToolBar>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

QMenu *ActionManager::insertMenu(const QString &id, const QString &title, const QString &idBefore)
{    
    QMenu *menu = m_idMenuMap.value(id);
    if (menu) {
        return menu;
    }
    if (idBefore.isEmpty()) {
        menu = m_liteApp->mainWindow()->menuBar()->addMenu(title);
    } else {
        QMenu *m = m_idMenuMap.value(idBefore);
        if (m) {
            menu = new QMenu(title,m_liteApp->mainWindow());
            m_liteApp->mainWindow()->menuBar()->insertMenu(m->menuAction(),menu);
        }
    }
    if (menu) {
        m_idMenuMap.insert(id,menu);
    }
    return menu;
}

QList<QMenu*>  ActionManager::menuList() const
{
    return m_idMenuMap.values();
}

QMenu *ActionManager::loadMenu(const QString &id)
{
    return m_idMenuMap.value(id);
}

void ActionManager::addToolBar(QToolBar* toolBar)
{
    if (toolBar) {
        m_liteApp->mainWindow()->addToolBar(toolBar);
    }
}

void ActionManager::removeToolBar(QToolBar* toolBar)
{
    if (toolBar){
        m_liteApp->mainWindow()->removeToolBar(toolBar);
    }
}

void ActionManager::showToolBar(QToolBar* toolBar)
{
    if (toolBar) {
        toolBar->setVisible(true);
    }
}

void ActionManager::hideToolBar(QToolBar* toolBar)
{
    if (toolBar) {
        toolBar->setVisible(false);
    }
}
