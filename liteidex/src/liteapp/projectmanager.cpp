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
#include <QHBoxLayout>
#include <QPushButton>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QStackedLayout>
#include <QMenu>
#include <QActionGroup>
#include <QFileDialog>
#include <QDebug>
#include <QScrollArea>
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
    : m_widget(0), m_scrollArea(0)
{
}

ProjectManager::~ProjectManager()
{
    if (m_widget) {
        delete m_widget;
        m_liteApp->dockManager()->removeDock(m_widget);
    }
}

bool ProjectManager::initWithApp(IApplication *app)
{
    if (!IProjectManager::initWithApp(app)) {
        return false;
    }

    m_widget = new QWidget;
    m_projectMenu = new QMenu(m_widget);
    m_projectActGroup = new QActionGroup(m_widget);

    m_importMenu = new QMenu(m_widget);

    QBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    m_scrollArea = new QScrollArea(m_widget);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setWidgetResizable(true);

    QHBoxLayout *headLayout = new QHBoxLayout;
    headLayout->setMargin(0);
    QPushButton *btn = new QPushButton(tr("Import"));
    btn->setMenu(m_importMenu);
    headLayout->addWidget(btn);
    headLayout->addStretch(0);

    layout->addLayout(headLayout);
    layout->addWidget(m_scrollArea);
    m_widget->setLayout(layout);
    m_liteApp->dockManager()->addDock(m_widget,tr("Projects"));

    connect(m_projectMenu,SIGNAL(triggered(QAction*)),this,SLOT(triggeredProject(QAction*)));
    connect(m_liteApp,SIGNAL(loaded()),this,SLOT(appLoaded()));

    return true;
}

void ProjectManager::appLoaded()
{
    QAction *openProjectAct = new QAction(QIcon(":/images/openproject.png"),tr("Project File"),this);
    connect(openProjectAct,SIGNAL(triggered()),m_liteApp->fileManager(),SLOT(openProjects()));
    m_importMenu->addAction(openProjectAct);

    foreach (IProjectFactory *factory, this->m_factoryList) {
        foreach(QString type, factory->mimeTypes()) {
            IMimeType *mimeType = m_liteApp->mimeTypeManager()->findMimeType(type);
            if (mimeType && !mimeType->scheme().isEmpty()) {
                QAction *act = new QAction(QString(tr("Project <%1>").arg(mimeType->scheme())),this);
                act->setData(mimeType->scheme());
                connect(act,SIGNAL(triggered()),this,SLOT(openSchemeAct()));
                m_importMenu->addAction(act);
            }
        }
    }
}

QWidget *ProjectManager::widget()
{
    return m_widget;
}

void ProjectManager::triggeredProject(QAction* act)
{
    QString fileName = act->text();
    m_liteApp->fileManager()->openProject(fileName);
}

void ProjectManager::openSchemeAct()
{
    QAction *act = (QAction*)sender();
    if (!act) {
        return;
    }
    QString scheme = act->data().toString();
    if (scheme.isEmpty()) {
        return;
    }
    QString dir = QFileDialog::getExistingDirectory(m_liteApp->mainWindow(), tr("Open Directory"),
                                                     "/home",
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty()) {
        return;
    }

    m_liteApp->fileManager()->openProjectScheme(dir,scheme);
}

void ProjectManager::currentEditorChanged(LiteApi::IEditor* editor)
{
    if (!editor) {
        return;
    }
//    QString fileName = editor->fileName();
//    IProject *project = 0;
//    foreach (IProjectFactory *factory , m_factoryList) {
//        project = factory->findByEditor(editor);
//        if (project) {
//            break;
//        }
//    }
//    if (project) {
//        QAction *act = m_mapNameToAction.value(fileName);
//        if (act == 0) {
//            act = m_projectActGroup->addAction(fileName);
//            act->setCheckable(true);
//            m_mapNameToAction.insert(fileName,act);
//            m_projectMenu->addAction(act);
//        }
//        setCurrentProject(project);
//    }
}

IProject *ProjectManager::openProject(const QString &fileName, const QString &mimeType)
{
    if (m_currentProject && m_currentProject->filePath() == fileName) {
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
        QAction *act = m_mapNameToAction.value(fileName);
        if (act == 0) {
            act = m_projectActGroup->addAction(fileName);
            act->setCheckable(true);
            m_mapNameToAction.insert(fileName,act);
            m_projectMenu->addAction(act);
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
    if (m_currentProject) {
        closeProjectHelper(m_currentProject);
    }
    m_currentProject = project;

    if (m_currentProject) {
        m_scrollArea->setWidget(m_currentProject->widget());
        m_currentProject->load();
        QAction *act = m_mapNameToAction.value(project->filePath());
        if (act) {
            act->setChecked(true);
        }

        m_liteApp->appendConsole("ProjectManager","loadProject",m_currentProject->name());
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
                if (FileUtil::compareFile(editor->filePath(),fileName)) {
                    editors << editor;
                    break;
                }
            }
        }
    }
    return editors;
}

void ProjectManager::addImportAction(QAction *act)
{
    m_importMenu->addAction(act);
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

    m_scrollArea->takeWidget();

    foreach (IEditor *editor, editorList(cur)) {
        m_liteApp->editorManager()->closeEditor(editor);
    }
    QAction *act = m_mapNameToAction.value(cur->filePath());
    if (act) {
        act->setChecked(false);
    }

    m_liteApp->appendConsole("ProjectManager","closeProject",cur->name());

    delete cur;
}

void ProjectManager::closeProject(IProject *project)
{
    closeProjectHelper(project);
    emit currentProjectChanged(0);
}
