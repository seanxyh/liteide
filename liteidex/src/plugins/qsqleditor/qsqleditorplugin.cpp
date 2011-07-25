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
// Module: qsqleditorplugin.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-14
// $Id: qsqleditorplugin.cpp,v 1.0 2011-7-14 visualfc Exp $

#include "qsqleditorplugin.h"
#include "qsqlfilefactory.h"

QSqlEditorPlugin::QSqlEditorPlugin()
{
    m_info->setId("plugin/QSqlEditor");
    m_info->setName("QSqlEditor");
    m_info->setAnchor("visualfc");
    m_info->setInfo("QtSql Editor Plugin");
}

bool QSqlEditorPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }

    QSqlFileFactory *factory = new QSqlFileFactory(app,this);
    if (factory) {
        m_liteApp->editorManager()->addFactory(factory);
    }

    return true;
}

Q_EXPORT_PLUGIN(QSqlEditorPlugin)