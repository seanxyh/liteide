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
// Module: packageproject.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-4-19
// $Id: packageproject.cpp,v 1.0 2012-4-19 visualfc Exp $

#include "packageproject.h"
#include "filepathmodel.h"
#include <QDir>
#include <QTreeView>
#include <QVBoxLayout>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

PackageProject::PackageProject(LiteApi::IApplication *app) :
    m_liteApp(app)
{
    m_widget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    m_treeView = new QTreeView;
    m_model = new FilePathModel(this);
    m_treeView->setModel(m_model);
    layout->addWidget(m_treeView);
    m_widget->setLayout(layout);
}

void PackageProject::setJson(const QMap<QString,QVariant> &json)
{
    m_json = json;
}

QWidget *PackageProject::widget()
{
    return m_widget;
}

QString PackageProject::name() const
{
    return m_json.value("ImportPath").toString();
}
QString PackageProject::filePath() const
{
    return m_json.value("Dir").toString();
}
QString PackageProject::mimeType() const
{
    return "text/x-gopackage";
}
QStringList PackageProject::fileNameList() const
{
    return QStringList();
}
QStringList PackageProject::filePathList() const
{
    //QDir dir(filePath());
    return QStringList();
}
QString PackageProject::fileNameToFullPath(const QString &filePath)
{
    return filePath;
}

/*
valueForKey

EDITORPATH
EDITORNAME
EDITORDIR

PROJECTPATH
PROJECTNAME
PROJECTDIR

WORKDIR

TARGETPATH
TARGETNAME
TARGETDIR
*/

QMap<QString,QString> PackageProject::projectInfo() const
{
    QMap<QString,QString> m;
    QFileInfo info(m_json.value("Dir").toString());
    m.insert("PROJECTPATH",info.filePath());
    m.insert("PROJECTNAME",info.fileName());
    m.insert("PROJECTDIR",info.filePath());
    return m;
}

QMap<QString,QString> PackageProject::targetInfo() const
{
    QMap<QString,QString> m;
    QDir dir(m_json.value("Dir").toString());
    m.insert("WORKDIR",dir.path());
    QString name = m_json.value("ImportPath").toString();
    m.insert("TARGETPATH",QFileInfo(dir,name).filePath());
    m.insert("TARGETNAME",name);
    m.insert("TARGETDIR",dir.path());
    return m;
}

void PackageProject::load()
{
    m_model->setRootPath(m_json.value("Dir").toString());
}
