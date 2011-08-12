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
// Module: golangast.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: golangast.cpp,v 1.0 2011-7-5 visualfc Exp $

#include "golangast.h"
#include "liteapi/litefindobj.h"
#include "golangastitem.h"
#include "golangasticon.h"
#include "astwidget.h"

#include <QStackedWidget>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QTimer>
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

GolangAst::GolangAst(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app)
{
    m_widget = new QWidget;

    m_currentEditor = 0;

    m_stackedWidget = new QStackedWidget;

    m_projectAstWidget = new AstWidget(m_liteApp);
    m_stackedWidget->addWidget(m_projectAstWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->addWidget(m_stackedWidget);
    m_widget->setLayout(mainLayout);
    m_process = new QProcess(this);

    m_timer = new QTimer(this);
    connect(m_liteApp->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(m_liteApp->editorManager(),SIGNAL(editorAboutToClose(LiteApi::IEditor*)),this,SLOT(editorAboutToClose(LiteApi::IEditor*)));
    connect(m_liteApp->projectManager(),SIGNAL(currentProjectChanged(LiteApi::IProject*)),this,SLOT(projectChanged(LiteApi::IProject*)));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(editorChanged(LiteApi::IEditor*)));
    connect(m_liteApp->editorManager(),SIGNAL(editorSaved(LiteApi::IEditor*)),this,SLOT(editorSaved(LiteApi::IEditor*)));
    connect(m_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finishedProcess(int,QProcess::ExitStatus)));
    connect(m_timer,SIGNAL(timeout()),this,SLOT(updateAstNow()));
    connect(m_projectAstWidget,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickedTree(QModelIndex)));

    m_stackedWidget->setVisible(false);
    m_liteApp->extension()->addObject("GoAstView.Widget",m_widget);
}

GolangAst::~GolangAst()
{
    if (m_timer->isActive()) {
        m_timer->stop();
    }
    delete m_process;
    m_liteApp->dockManager()->removeDock(m_widget);
    delete m_widget;
}

QWidget *GolangAst::widget()
{
    return m_widget;
}

void GolangAst::setEnable(bool b)
{
    if (b) {
        m_stackedWidget->setVisible(true);
        projectChanged(m_liteApp->projectManager()->currentProject());
        editorChanged(m_liteApp->editorManager()->currentEditor());
        updateAst();
    } else {
        //m_stackedWidget->setVisible(false);
    }
}

void GolangAst::projectChanged(LiteApi::IProject *project)
{
    m_updateFileNames.clear();
    m_updateFilePaths.clear();
    if (project) {
        foreach(QString file, project->fileNameList()) {
            if (QFileInfo(file).suffix() == "go") {
                m_updateFileNames.append(file);
            }
        }
        foreach(QString file, project->filePathList()) {
            QFileInfo info(file);
            if (info.suffix() == "go") {
                m_updateFilePaths.append(info.filePath());
            }
        }
        m_workPath = project->workPath();
        m_process->setWorkingDirectory(m_workPath);
        m_projectAstWidget->setWorkPath(m_workPath);
        m_stackedWidget->setCurrentWidget(m_projectAstWidget);
        updateAst();
    } else {
        m_projectAstWidget->clear();
        LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
        if (editor) {
            editorChanged(editor);
        }
    }
}

void GolangAst::editorCreated(LiteApi::IEditor *editor)
{
    AstWidget *w = m_editorAstWidgetMap.value(editor);
    if (w) {
        return;
    }
    if (editor) {
        QString fileName = editor->fileName();
        if (!fileName.isEmpty()) {
            QFileInfo info(fileName);
            if (info.suffix() == "go") {
                AstWidget *w = new AstWidget(m_liteApp);
                w->setWorkPath(info.absolutePath());
                connect(w,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickedTree(QModelIndex)));
                m_stackedWidget->addWidget(w);
                m_editorAstWidgetMap.insert(editor,w);
            }
        }
    }
}

void GolangAst::editorAboutToClose(LiteApi::IEditor *editor)
{
    AstWidget *w = m_editorAstWidgetMap.value(editor);
    if (w == 0) {
        return;
    }
    m_stackedWidget->removeWidget(w);
    m_editorAstWidgetMap.remove(editor);
}

void GolangAst::editorChanged(LiteApi::IEditor *editor)
{
    if (!m_liteApp->projectManager()->currentProject()) {
        m_updateFileNames.clear();
        m_updateFilePaths.clear();
        m_currentEditor = editor;
        AstWidget *w = m_editorAstWidgetMap.value(editor);
        if (w) {
            m_stackedWidget->setCurrentWidget(w);
        }
        if (editor) {
            QString fileName = editor->fileName();
            if (!fileName.isEmpty()) {
                QFileInfo info(fileName);
                m_workPath = info.absolutePath();
                m_process->setWorkingDirectory(info.absolutePath());
                if (info.suffix() == "go") {
                    m_updateFileNames.append(info.fileName());
                    m_updateFilePaths.append(info.filePath());
                }
            }
            updateAst();
        }
    } else {
        m_currentEditor = 0;
    }
}

void GolangAst::editorSaved(LiteApi::IEditor *editor)
{
    if (editor) {
        QString fileName = editor->fileName();
        QFileInfo info(fileName);
        if (!fileName.isEmpty() && info.suffix() == "go" && m_updateFilePaths.contains(info.filePath())) {
            updateAst();
        }
    }
}

void GolangAst::updateAst()
{
    m_timer->start(1000);
}

void GolangAst::updateAstNow()
{
    if (m_timer->isActive()) {
        m_timer->stop();
    }
    if (m_updateFileNames.isEmpty()) {
        return;
    }
#ifdef Q_OS_WIN
    QString goastview = "goastview.exe";
#else
    QString goastview = "goastview";
#endif
    QString cmd = m_liteApp->applicationPath();
    cmd += "/";
    cmd += goastview;
    QStringList args;
    args << "-files";
    args << m_updateFileNames.join(" ");
    m_process->start(cmd,args);
}

void GolangAst::finishedProcess(int code,QProcess::ExitStatus status)
{
    if (code == 0 && status == QProcess::NormalExit) {
        if (m_liteApp->projectManager()->currentProject()) {
            m_projectAstWidget->updateModel(m_process->readAllStandardOutput());
        } else if (m_currentEditor) {
            AstWidget *w = m_editorAstWidgetMap.value(m_currentEditor);
            if (w) {
                w->updateModel(m_process->readAllStandardOutput());
            }
        }
    } else {
        //qDebug() << m_process->readAllStandardError();
    }
}

void GolangAst::doubleClickedTree(QModelIndex index)
{
    AstWidget *w = (AstWidget*)sender();
    if (!w) {
        return;
    }
    GolangAstItem *item = w->astItemFromIndex(index);
    if (item == NULL)
        return;
    QString fileName = item->fileName();
    if (fileName.isEmpty()) {
        if (w->isExpanded(index)) {
            w->collapse(index);
        } else {
            w->expand(index);
        }
        return;
    }
    QFileInfo info(QDir(w->workPath()),fileName);
    LiteApi::IEditor *editor = m_liteApp->fileManager()->openEditor(info.filePath());
    if (!editor) {
        return;
    }
    editor->widget()->setFocus();
    LiteApi::ITextEditor *textEditor = LiteApi::findExtensionObject<LiteApi::ITextEditor*>(editor,"LiteApi.ITextEditor");
    if (!textEditor) {
        return;
    }
    textEditor->gotoLine(item->line(),item->col());
}
