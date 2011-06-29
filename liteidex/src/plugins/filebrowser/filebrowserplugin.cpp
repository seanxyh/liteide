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
// Module: filebrowserplugin.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-6-21
// $Id: filebrowserplugin.cpp,v 1.0 2011-6-29 visualfc Exp $

#include "filebrowserplugin.h"
#include "filebrowser.h"
#include "filebrowseroptionfactory.h"

FileBrowserPlugin::FileBrowserPlugin() : m_browser(0)
{
    m_info->setId("plugin/FileBrowser");
    m_info->setName("FileBrowser");
    m_info->setAnchor("visualfc");
    m_info->setInfo("FileBrowser Plugin");
}

bool FileBrowserPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }
    m_browser = new FileBrowser(app,this);
    app->optionManager()->addFactory(new FileBrowserOptionFactory(app,this));
    return true;
}

Q_EXPORT_PLUGIN(FileBrowserPlugin)
