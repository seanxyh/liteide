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
// Module: golangdocplugin.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-7
// $Id: golangdocplugin.cpp,v 1.0 2011-7-7 visualfc Exp $

#include "golangdocplugin.h"
#include "golangdoc.h"
#include "liteapi/litefindobj.h"

GolangDocPlugin::GolangDocPlugin()
    : m_golangDoc(0)
{
    m_info->setId("plugin/golangdoc");
    m_info->setName("GolangDoc");
    m_info->setAnchor("visualfc");
    m_info->setInfo("GolangDoc Plugin");
}

GolangDocPlugin::~GolangDocPlugin()
{
    if (m_golangDoc) {
        delete m_golangDoc;
    }
}

bool GolangDocPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }

    m_golangDoc = new GolangDoc(app,this);

    return true;
}

QStringList GolangDocPlugin::dependPluginList() const
{
    return QStringList() << "plugin/litebuild";
}

void GolangDocPlugin::currentEditorChanged(LiteApi::IEditor *editor)
{
    bool active = false;
    if (editor) {
        LiteApi::IFile *file = editor->file();
        if (file &&file->mimeType() == "text/x-gosrc") {
            active = true;
        }
    }
    //m_golangDoc->changeState()
}

Q_EXPORT_PLUGIN(GolangDocPlugin)
