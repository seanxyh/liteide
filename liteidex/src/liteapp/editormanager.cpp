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
// Module: editormanager.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: editormanager.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "editormanager.h"

#include <QFileInfo>
#include <QTabWidget>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QStackedWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QDebug>
#include "litetabwidget.h"
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

bool EditorManager::initWithApp(IApplication *app)
{
    if (!IEditorManager::initWithApp(app)) {
        return false;
    }
    m_widget = new QWidget;
    m_editorTabWidget = new LiteTabWidget;

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(1);
    mainLayout->addWidget(m_editorTabWidget);
    m_widget->setLayout(mainLayout);

    connect(m_editorTabWidget,SIGNAL(currentChanged(int)),this,SLOT(editorTabChanged(int)));
    connect(m_editorTabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(editorTabCloseRequested(int)));
    connect(m_editorTabWidget,SIGNAL(tabAddRequest()),this,SIGNAL(tabAddRequest()));
    return true;
}

QWidget *EditorManager::widget()
{
    return m_widget;
}

void EditorManager::editorTabChanged(int /*index*/)
{
    QWidget *w = m_editorTabWidget->currentWidget();
    IEditor *ed = m_widgetEditorMap.value(w,0);
    this->setCurrentEditor(ed);
}

void EditorManager::editorTabCloseRequested(int index)
{
    QWidget *w = m_editorTabWidget->widget(index);
    IEditor *ed = m_widgetEditorMap.value(w,0);
    closeEditor(ed);
}

void EditorManager::addEditorHelper(IEditor *editor, bool autoRelease)
{
    QWidget *w = m_widgetEditorMap.key(editor);
    if (w == 0) {
        w = editor->widget();
        if (w == 0) {
            return;
        }
        QString tip;
        if (editor && editor->file()) {
            tip = editor->file()->fileName();
        } else {
            tip = editor->displayName();
        }
        m_editorTabWidget->addTab(w,QIcon(),editor->displayName(),tip);
        m_widgetEditorMap.insert(w,editor);
        if (autoRelease) {
            m_autoReleaseEditorList.append(editor);
        }
        emit editorCreated(editor);
        connect(editor,SIGNAL(modificationChanged(bool)),this,SLOT(modificationChanged(bool)));
    }
    setCurrentEditor(editor);
}

QList<IEditor*> EditorManager::sortedEditorList() const
{
    QList<IEditor*> editorList;
    foreach (QWidget *w,m_editorTabWidget->widgetList()) {
        IEditor *ed = m_widgetEditorMap.value(w);
        if (ed) {
            editorList << ed;
        }
    }
    return editorList;
}

void EditorManager::addAutoReleaseEditor(IEditor *editor)
{
    addEditorHelper(editor,true);
}

void EditorManager::addEditor(IEditor *editor)
{
    addEditorHelper(editor,false);
}

IEditor *EditorManager::loadEditor(const QString &fileName)
{
    foreach(IEditor *editor, editorList()) {
        if (!editor->file()) {
            continue;
        }
        if (FileUtil::compareFile(editor->file()->fileName(),fileName)) {
            return editor;
        }
    }
    m_liteApp->fileManager()->openEditor(fileName);
    foreach(IEditor *editor, editorList()) {
        if (!editor->file()) {
            continue;
        }
        if (FileUtil::compareFile(editor->file()->fileName(),fileName)) {
            return editor;
        }
    }
    return 0;
}

bool EditorManager::closeEditor(IEditor *editor)
{
    IEditor *cur = 0;
    if (editor) {
        cur = editor;
    } else {
        cur = m_currentEditor;
    }
    if (cur == 0) {
        return false;
    }

    if (cur->isModified()) {
        QString text = QString(tr("%1 is modified.")).arg(cur->file()->fileName());
        int ret = QMessageBox::question(m_editorTabWidget,tr("Save Modify"),text,QMessageBox::Save | QMessageBox::No | QMessageBox::Cancel);
        if (ret == QMessageBox::Cancel) {
            return false;
        } else if (ret == QMessageBox::SaveAll) {
            cur->save();
        }
    }
    emit editorAboutToClose(cur);
    int index = m_editorTabWidget->indexOf(cur->widget());
    m_editorTabWidget->removeTab(index);
    m_widgetEditorMap.remove(cur->widget());

    index = m_autoReleaseEditorList.indexOf(cur);
    if (index >= 0) {
        m_autoReleaseEditorList.removeAt(index);
        delete cur;
    }
    return true;
}

bool EditorManager::saveEditor(IEditor *editor)
{
    IEditor *cur = 0;
    if (editor) {
        cur = editor;
    } else {
        cur = m_currentEditor;
    }
    if (cur == 0) {
        return false;
    }

    IFile *file = cur->file();
    if (!file) {
        return false;
    }

    const QString &fileName = file->fileName();

    if (fileName.isEmpty())
        return saveEditorAs(cur);    

    bool b = file->save(fileName);

    emit editorSaved(cur);

    return b;
}

bool EditorManager::saveEditorAs(IEditor *editor)
{
    IEditor *cur = 0;
    if (editor) {
        cur = editor;
    } else {
        cur = m_currentEditor;
    }
    if (cur == 0) {
        return false;
    }

    return true;
}

bool EditorManager::saveAllEditors()
{
    QList<IEditor*> editorList = m_widgetEditorMap.values();
    foreach (IEditor *editor, editorList) {
        saveEditor(editor);
    }
    return true;
}

bool EditorManager::closeAllEditors(bool autoSaveAll)
{
    bool bSaveAll = false;
    if (autoSaveAll) {
        bSaveAll = true;
    }
    bool bCloseAll = true;

    QMapIterator<QWidget *, IEditor *> i (m_widgetEditorMap);
    while (i.hasNext()) {
        i.next();
        if (!closeEditor(i.value())) {
            bCloseAll = false;
            break;
        }
    }

    return bCloseAll;
}

IEditor *EditorManager::currentEditor() const
{
    return m_currentEditor;
}

void EditorManager::setCurrentEditor(IEditor *editor)
{
    if (m_currentEditor == editor) {
        return;
    }
    m_currentEditor = editor;
    if (editor != 0) {
        m_editorTabWidget->setCurrentWidget(editor->widget());
    }
    emit currentEditorChanged(editor);
}

QList<IEditor*> EditorManager::editorList() const
{
    return m_widgetEditorMap.values();
}

void EditorManager::addFactory(IFileFactory *factory)
{
    m_factoryList.append(factory);
}

void EditorManager::removeFactory(IFileFactory *factory)
{
    m_factoryList.removeOne(factory);
}

QList<IFileFactory*>  EditorManager::factoryList() const
{
    return m_factoryList;
}

QStringList EditorManager::mimeTypeList() const
{
    QStringList types;
    foreach(IFileFactory *factory, m_factoryList) {
        types.append(factory->mimeTypes());
    }
    return types;
}

IFile *EditorManager::createFile(const QString &fileName, const QString &mimeType)
{
    IFile *file = 0;
    QList<IEditor*> editors = m_widgetEditorMap.values();
    foreach(IEditor *editor, editors) {
        if (!editor->file()) {
            continue;
        }
        if (FileUtil::compareFile(editor->file()->fileName(),fileName)) {
            file = editor->file();
            setCurrentEditor(editor);
            break;
        }
    }
    if (file == 0) {
        foreach (IFileFactory *factory, m_factoryList) {
            if (factory->mimeTypes().contains(mimeType)) {
                file = factory->open(fileName,mimeType);
                break;
            }
        }
    }
    if (file == 0) {
        //open on default.editor
        QString type = "text/liteide.default.editor";
        foreach (IFileFactory *factory, m_factoryList) {
            if (factory->mimeTypes().contains(type)) {
                file = factory->open(fileName,type);
                break;
            }
        }
    }
    return file;
}

void EditorManager::modificationChanged(bool b)
{
    IEditor *editor = static_cast<IEditor*>(sender());
    if (editor) {
        QString text = editor->displayName();
        if (b) {
            text += " *";
        }
        int index = m_editorTabWidget->indexOf(editor->widget());
        if (index >= 0) {
            m_editorTabWidget->setTabText(index,text);
        }
    }
}

