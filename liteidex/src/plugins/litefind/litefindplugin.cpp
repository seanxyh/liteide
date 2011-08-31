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
// Module: litefindplugin.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: litefindplugin.cpp,v 1.0 2011-8-12 visualfc Exp $

#include "litefindplugin.h"
#include "findeditor.h"
#include "replaceeditor.h"

#include <QMenu>
#include <QAction>
#include <QLayout>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

LiteFindPlugin::LiteFindPlugin()
    : m_findEditor(0),
      m_replaceEditor(0)
{
    m_info->setId("plugin/LiteFind");
    m_info->setName("LiteFind");
    m_info->setAnchor("visualfc");
    m_info->setInfo("LiteIDE Find Plugin");
}

bool LiteFindPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }

    QLayout *layout = m_liteApp->editorManager()->widget()->layout();
    if (!layout) {
        return false;
    }

    QMenu *menu = m_liteApp->actionManager()->insertMenu("find",tr("F&ind"),"help");
    if (!menu) {
        return false;
    }

    m_findEditor = new FindEditor(m_liteApp,this);
    m_findEditor->widget()->hide();

    m_replaceEditor = new ReplaceEditor(m_liteApp,this);
    m_replaceEditor->widget()->hide();

    layout->addWidget(m_findEditor->widget());
    layout->addWidget(m_replaceEditor->widget());

    m_findAct = new QAction(tr("Find"),this);
    m_findAct->setShortcut(QKeySequence::Find);
    m_findAct->setCheckable(true);

    m_findNextAct = new QAction(tr("Find Next"),this);
    m_findNextAct->setShortcut(QKeySequence::FindNext);
    m_findPrevAct = new QAction(tr("Find Previous"),this);
    m_findPrevAct->setShortcut(QKeySequence::FindPrevious);

    m_replaceAct = new QAction(tr("Replace"),this);
    m_replaceAct->setShortcut(QKeySequence::Replace);
    m_replaceAct->setCheckable(true);

    menu->addAction(m_findAct);
    menu->addAction(m_findNextAct);
    menu->addAction(m_findPrevAct);
    menu->addSeparator();
    menu->addAction(m_replaceAct);

    connect(m_findAct,SIGNAL(toggled(bool)),this,SLOT(find(bool)));
    connect(m_findNextAct,SIGNAL(triggered()),m_findEditor,SLOT(findNext()));
    connect(m_findPrevAct,SIGNAL(triggered()),m_findEditor,SLOT(findPrev()));
    connect(m_replaceAct,SIGNAL(toggled(bool)),this,SLOT(replace(bool)));
    connect(m_findEditor,SIGNAL(hideFind()),this,SLOT(hideFind()));
    connect(m_replaceEditor,SIGNAL(hideReplace()),this,SLOT(hideReplace()));
    connect(m_findEditor,SIGNAL(swithReplace()),this,SLOT(switchReplace()));

    return true;
}

void LiteFindPlugin::hideFind()
{
    m_findAct->setChecked(false);
}

void LiteFindPlugin::hideReplace()
{
    m_replaceAct->setChecked(false);
}

void LiteFindPlugin::switchReplace()
{
    m_replaceAct->setChecked(true);
}

void LiteFindPlugin::find(bool b)
{
    if (b) {
        m_replaceAct->setChecked(false);
        m_findEditor->setReady();
    }
    m_findEditor->widget()->setVisible(b);
}

void LiteFindPlugin::replace(bool b)
{
    if (b) {
        if (m_findAct->isChecked()) {
            m_replaceEditor->setReady(m_findEditor->findText());
        } else {
            m_replaceEditor->setReady();
        }
        m_findAct->setChecked(false);
    }
    m_replaceEditor->widget()->setVisible(b);
}

Q_EXPORT_PLUGIN(LiteFindPlugin)
