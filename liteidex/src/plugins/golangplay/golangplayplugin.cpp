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
// Module: golangplayplugin.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-3-8
// $Id: golangplayplugin.cpp,v 1.0 2012-3-8 visualfc Exp $

#include "golangplayplugin.h"
#include "goplaybrowser.h"
#include "liteapi/liteobj.h"
#include <QAction>

GolangPlayPlugin::GolangPlayPlugin()
{
    m_info->setId("plugin/GolangPlay");
    m_info->setName("GolangPlay");
    m_info->setAnchor("visualfc");
    m_info->setInfo("GolangPlay Plugin");
}

bool GolangPlayPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }

    GoplayBrowser *gopaly = new GoplayBrowser(app,this);
    QAction *act = m_liteApp->editorManager()->registerBrowser(gopaly);
    act->setIcon(QIcon(":/images/gopher.png"));
    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuPagePos,act);
    return true;
}

QStringList GolangPlayPlugin::dependPluginList() const
{
    return QStringList() << "plugin/liteeditor";
}

Q_EXPORT_PLUGIN(GolangPlayPlugin)
