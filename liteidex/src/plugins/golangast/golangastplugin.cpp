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
// Module: golangastplugin.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: golangastplugin.cpp,v 1.0 2011-7-5 visualfc Exp $

#include "golangastplugin.h"
#include <QDockWidget>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


GolangAstPlugin::GolangAstPlugin() : m_golangAst(0)
{
    m_info->setId("plugin/golangast");
    m_info->setName("GolangAst");
    m_info->setAnchor("visualfc");
    m_info->setVer("x11.4");
    m_info->setInfo("Golang AstView Plugin");
}

bool GolangAstPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }
    m_golangAst = new GolangAst(m_liteApp,this);
    QDockWidget *dock = m_liteApp->dockManager()->addDock(m_golangAst->widget(),tr("Golang AstView"));
    connect(dock,SIGNAL(visibilityChanged(bool)),this,SLOT(visibilityChanged(bool)));
    connect(m_liteApp->projectManager(),SIGNAL(currentProjectChanged(LiteApi::IProject*)),this,SLOT(checkEnableGolangAst()));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(checkEnableGolangAst()));

    m_bVisible = dock->isVisible();
    m_bEnable = false;
    return true;
}

void GolangAstPlugin::visibilityChanged(bool b)
{
    m_bVisible = b;
    checkEnableGolangAst();
}

void GolangAstPlugin::checkEnableGolangAst()
{
    if (!m_bVisible) {
        return;
    }
    bool active = false;
    LiteApi::IProject *project = m_liteApp->projectManager()->currentProject();
    if (project && (project->mimeType() == "text/x-gopro" ||
                    project->mimeType() == "text/x-gomake" ) ) {
        active = true;
    } else {
        LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
        if (editor && editor->mimeType() == "text/x-gosrc") {
            active = true;
        }
    }
    if (active != m_bEnable) {
        m_bEnable = active;
        m_golangAst->setEnable(m_bEnable);
    }
}

Q_EXPORT_PLUGIN(GolangAstPlugin)
