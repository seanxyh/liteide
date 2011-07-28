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
// Module: projectmanager.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: projectmanager.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "projectmanager.h"

#include <QFileInfo>
#include <QComboBox>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QToolBar>
#include "fileutil/fileutil.h"

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

ProjectManager::ProjectManager()
    : m_widget(0), m_mainLayout(0)
{
}

ProjectManager::~ProjectManager()
{
    if (m_widget) {
        delete m_widget;
        m_liteApp->dockManager()->removeDock(m_widget);
    }
}

QWidget *ProjectManager::widget()
{
    return m_widget;
}

IProject *ProjectManager::openProject(const QString &fileName, const QString &mimeType)
{
    if (m_currentProject && m_currentProject->fileName() == fileName) {
        return m_currentProject;
    }
    IProject *project = 0;
    foreach (IProjectFactory *factory , m_factoryList) {
        if (factory->mimeTypes().contains(mimeType)) {
            project = factory->open(fileName,mimeType);
            if (project) {
                break;
            }
        }
    }
    if (project) {
        setCurrentProject(project);
    }
    return project;
}

void ProjectManager::addFactory(IProjectFactory *factory)
{
    m_factoryList.append(factory);
}

void ProjectManager::removeFactory(IProjectFactory *factory)
{
    m_factoryList.removeOne(factory);
}

QList<IProjectFactory*> ProjectManager::factoryList() const
{
    return m_factoryList;
}

QStringList ProjectManager::mimeTypeList() const
{
    QStringList types;
    foreach(IProjectFactory *factory, m_factoryList) {
        types.append(factory->mimeTypes());
    }
    return types;
}

void ProjectManager::setCurrentProject(IProject *project)
{
    if (m_currentProject == project) {
        return;
    }
    if (m_currentProject) {
        closeProjectHelper(m_currentProject);
    }
    m_currentProject = project;
    if (m_currentProject) {
        if (m_widget == 0) {
            m_widget = new QWidget;
            m_mainLayout = new QVBoxLayout;
            m_mainLayout->setMargin(0);
            m_widget->setLayout(m_mainLayout);
            m_liteApp->dockManager()->addDock(m_widget,tr("Project"));
        }
        m_mainLayout->addWidget(m_currentProject->widget());
        m_liteApp->dockManager()->showDock(m_widget);
        m_currentProject->load();
        m_liteApp->appendConsole("ProjectManager","loadProject",project->name());
    }
    emit currentProjectChanged(project);
}

IProject *ProjectManager::currentProject() const
{
    return m_currentProject;
}

QList<IEditor*> ProjectManager::editorList(IProject *project) const
{
    QList<IEditor*> editors;
    if (project) {
        foreach (QString fileName, project->filePathList()) {
             foreach(IEditor *editor, m_liteApp->editorManager()->editorList()) {
                if (FileUtil::compareFile(editor->fileName(),fileName)) {
                    editors << editor;
                    break;
                }
            }
        }
    }
    return editors;
}

void ProjectManager::saveProject(IProject *project)
{
    IProject *cur = 0;
    if (project == 0) {
        cur = m_currentProject;
    } else {
        cur = project;
    }
    if (cur == 0) {
        return;
    }
    foreach (IEditor *editor, editorList(cur)) {
        if (editor->isModified()) {
            m_liteApp->editorManager()->saveEditor(editor);
        }
    }
}

void ProjectManager::closeProjectHelper(IProject *project)
{
    IProject *cur = 0;
    if (project) {
        cur = project;
    } else {
       cur = m_currentProject;
    }
    if (cur == 0) {
        return;
    }

    emit projectAboutToClose(cur);

    foreach (IEditor *editor, editorList(cur)) {
        m_liteApp->editorManager()->closeEditor(editor);
    }
    if (m_mainLayout) {
        m_mainLayout->removeWidget(cur->widget());
        m_liteApp->dockManager()->hideDock(m_widget);
    }

    m_liteApp->appendConsole("ProjectManager","closeProject",cur->name());

    delete cur;
}

void ProjectManager::closeProject(IProject *project)
{
    closeProjectHelper(project);
    emit currentProjectChanged(0);
}
