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
// Module: golangfmtplugin.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: golangfmtplugin.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "golangfmtplugin.h"
#include "litefindobj.h"
#include "golangfmt.h"
#include "fileutil/fileutil.h"

#include <QToolBar>
#include <QAction>
#include <QToolButton>
#include <QFileInfo>
#include <QDir>
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

GolangFmtPlugin::GolangFmtPlugin()
    : m_fmt(0),
      m_build(0)
{
    m_info->setId("plugin/golangfmt");
    m_info->setName("GolangFmt");
    m_info->setAnchor("visualfc");
    m_info->setInfo("GolangFmt Plugin");
}

bool GolangFmtPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }
    connect(m_liteApp->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    LiteApi::IBuildManager *manager = LiteApi::findExtensionObject<LiteApi::IBuildManager*>(m_liteApp,"LiteApi.IBuildManager");
    if (manager) {
        m_build = manager->currentBuild();
        connect(manager,SIGNAL(buildChanged(LiteApi::IBuild*)),this,SLOT(buildChanged(LiteApi::IBuild*)));
    }
    return true;
}

QStringList GolangFmtPlugin::dependPluginList() const{
    return QStringList() << "plugin/litebuild";
}

void GolangFmtPlugin::buildChanged(LiteApi::IBuild *build)
{
    m_build = build;
}

void GolangFmtPlugin::editorCreated(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }
    LiteApi::IFile *file = editor->file();
    if (!file) {
        return;
    }
    if (file->mimeType() != "text/x-gosrc") {
        return;
    }
    QToolBar *toolBar = LiteApi::findExtensionObject<QToolBar*>(editor,"LiteApi.QToolBar");
    if (!toolBar) {
        return;
    }
    if (m_fmt == 0) {
        m_fmt = new GolangFmt(m_liteApp,this);
        m_gofmtAct = new QAction(QIcon(":/images/gofmt.png"),tr("gofmt"),this);
        m_gofmtAct->setShortcut(QKeySequence("SHIFT+F7"));
        m_gofmtAct->setToolTip("gofmt(Shift+F7)");
        connect(m_gofmtAct,SIGNAL(triggered()),this,SLOT(gofmt()));
    }
    toolBar->addSeparator();
    toolBar->addAction(m_gofmtAct);
}

void GolangFmtPlugin::gofmt()
{
    if (!m_build || !m_fmt) {
        return;
    }
    QString gobin = m_build->currentEnv().value("GOBIN");
    if (gobin.isEmpty()) {
        QString goroot = m_build->currentEnv().value("GOROOT");
        gobin = goroot+"/bin";
    }
    QString cmd = FileUtil::findExecute(gobin+"/gofmt");
    if (cmd.isEmpty()) {
        cmd = FileUtil::lookPath("gofmt",m_build->currentEnv(),true);
    }
    m_fmt->gofmt(cmd);
}


Q_EXPORT_PLUGIN(GolangFmtPlugin)
