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
// Module: browsereditormanager.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: browsereditormanager.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "browsereditormanager.h"
#include "browsereditorimpl.h"

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

bool BrowserEditorManager::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IManager::initWithApp(app)) {
        return false;
    }
    connect(m_liteApp->editorManager(),SIGNAL(editorAboutToClose(LiteApi::IEditor*)),this,SLOT(editorAboutToClose(LiteApi::IEditor*)));
    return true;
}

QAction *BrowserEditorManager::addBrowser(BrowserEditorImpl *impl)
{
    QAction *act = new QAction(impl->displayName(),this);
    act->setCheckable(true);
    act->setChecked(false);
    connect(act,SIGNAL(toggled(bool)),this,SLOT(toggleBrowserAction(bool)));
    m_browserActionMap.insert(impl,act);
    return act;
}

void BrowserEditorManager::setActive(BrowserEditorImpl *impl)
{
    QAction *act = m_browserActionMap.value(impl);
    if (!act) {
        return;
    }
    if (!act->isChecked()) {
        act->toggle();
    }
    m_liteApp->editorManager()->setCurrentEditor(impl);
}

void BrowserEditorManager::toggleBrowserAction(bool b)
{
    QAction *act = (QAction*)sender();
    if (act) {
        BrowserEditorImpl *browser = m_browserActionMap.key(act);
        if (browser) {
            if (b) {
                m_liteApp->editorManager()->addEditor(browser);
            } else {
                m_liteApp->editorManager()->closeEditor(browser);
            }
        }
    }
}

void BrowserEditorManager::editorAboutToClose(LiteApi::IEditor *editor)
{
    QMapIterator<BrowserEditorImpl*,QAction*> i(m_browserActionMap);
    while (i.hasNext()) {
        i.next();
        if (i.key() == editor) {
            i.value()->blockSignals(true);
            i.value()->setChecked(false);
            i.value()->blockSignals(false);
            break;
        }
    }
}
