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
// Module: litedebugplugin.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: litedebugplugin.cpp,v 1.0 2011-8-12 visualfc Exp $

#include "litedebugplugin.h"

LiteDebugPlugin::LiteDebugPlugin()
{
    m_info->setId("plugin/LiteDebug");
    m_info->setName("LiteDebug");
    m_info->setAnchor("visualfc");
    m_info->setInfo("LiteIDE Debug Plugin");
}

bool LiteDebugPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }
    return true;
}

Q_EXPORT_PLUGIN(LiteDebugPlugin)
