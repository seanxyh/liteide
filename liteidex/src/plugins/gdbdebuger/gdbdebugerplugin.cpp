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
// Module: gdbdebugerplugin.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: gdbdebugerplugin.cpp,v 1.0 2011-8-12 visualfc Exp $

#include "gdbdebugerplugin.h"
#include "gdbdebuger.h"
#include "litedebugapi/litedebugapi.h"
#include "liteapi/litefindobj.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

GdbDebugerPlugin::GdbDebugerPlugin()
{
    m_info->setId("plugin/GdbDebuger");
    m_info->setName("GdbDebuger");
    m_info->setAnchor("visualfc");
    m_info->setInfo("Gdb Debuger Plugin");
}

bool GdbDebugerPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }

    LiteApi::IDebugManager *manager = LiteApi::findExtensionObject<LiteApi::IDebugManager*>(app,"LiteApi.IDebugManager");
    if (!manager) {
        return false;
    }
    GdbDebuger *debug = new GdbDebuger(app);
    manager->addDebug(debug);
    manager->setCurrentDebug(debug);

    return true;
}

QStringList GdbDebugerPlugin::dependPluginList() const
{
    return QStringList() << "plugin/litedebug";
}

Q_EXPORT_PLUGIN(GdbDebugerPlugin)
