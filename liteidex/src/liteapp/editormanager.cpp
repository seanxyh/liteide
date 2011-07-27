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
#include <QFileDialog>
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

EditorManager::~EditorManager()
{
    m_browserActionMap.clear();
}

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
    if (w == 0) {
        this->setCurrentEditor(0);
    } else {
        IEditor *ed = m_widgetEditorMap.value(w,0);
        this->setCurrentEditor(ed);
    }
}

void EditorManager::editorTabCloseRequested(int index)
{
    QWidget *w = m_editorTabWidget->widget(index);
    IEditor *ed = m_widgetEditorMap.value(w,0);
    closeEditor(ed);
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

void EditorManager::addEditor(IEditor *editor)
{
    QWidget *w = m_widgetEditorMap.key(editor);
    if (w == 0) {
        w = editor->widget();
        if (w == 0) {
            return;
        }
        m_editorTabWidget->addTab(w,QIcon(),editor->name(),editor->displayName());
        m_widgetEditorMap.insert(w,editor);
        emit editorCreated(editor);
        connect(editor,SIGNAL(modificationChanged(bool)),this,SLOT(modificationChanged(bool)));
    }
}

QAction *EditorManager::registerBrowser(IEditor *editor)
{
    QAction *act = new QAction(editor->name(),this);
    act->setCheckable(true);
    act->setChecked(false);
    connect(act,SIGNAL(toggled(bool)),this,SLOT(toggleBrowserAction(bool)));
    m_browserActionMap.insert(editor,act);
    return act;
}

void EditorManager::activeBrowser(IEditor *editor)
{
    QAction *act = m_browserActionMap.value(editor);
    if (!act) {
        return;
    }
    if (!act->isChecked()) {
        act->toggle();
    }
    setCurrentEditor(editor);
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
        } else if (ret == QMessageBox::Save) {
            cur->save();
        }
    }
    emit editorAboutToClose(cur);
    int index = m_editorTabWidget->indexOf(cur->widget());
    m_editorTabWidget->removeTab(index);
    m_widgetEditorMap.remove(cur->widget());

    QMapIterator<IEditor*,QAction*> i(m_browserActionMap);
    while (i.hasNext()) {
        i.next();
        if (i.key() == cur) {
            i.value()->blockSignals(true);
            i.value()->setChecked(false);
            i.value()->blockSignals(false);
            return true;
        }
    }
    delete cur;
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
    IFile *file = cur->file();
    if (!file) {
        return false;
    }
    QString fileName = file->fileName();
    QFileInfo info(fileName);
    QStringList filter;
    QString ext = info.suffix();
    if (!ext.isEmpty()) {
        filter.append(QString("%1 (*.%1)").arg(ext).arg(ext));
    }
    filter.append(tr("All Files (*)"));
    QString path = info.absolutePath();
    QString saveFileName = QFileDialog::getSaveFileName(m_liteApp->mainWindow(),tr("Save As"),path,filter.join(";;"));
    if (saveFileName.isEmpty()) {
        return false;
    }
    if (!cur->file()->save(saveFileName)) {
        return false;
    }
    m_liteApp->fileManager()->openEditor(saveFileName);
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

void EditorManager::addFactory(IEditorFactory *factory)
{
    m_factoryList.append(factory);
}

void EditorManager::removeFactory(IEditorFactory *factory)
{
    m_factoryList.removeOne(factory);
}

QList<IEditorFactory*>  EditorManager::factoryList() const
{
    return m_factoryList;
}

QStringList EditorManager::mimeTypeList() const
{
    QStringList types;
    foreach(IEditorFactory *factory, m_factoryList) {
        types.append(factory->mimeTypes());
    }
    return types;
}

IEditor *EditorManager::openEditor(const QString &fileName, const QString &mimeType)
{
    QList<IEditor*> editors = m_widgetEditorMap.values();
    foreach(IEditor *editor, editors) {
        if (!editor->file()) {
            continue;
        }
        if (FileUtil::compareFile(editor->file()->fileName(),fileName)) {
            return editor;
        }
    }
    IEditor *ed = 0;
    foreach (IEditorFactory *factory, m_factoryList) {
        if (factory->mimeTypes().contains(mimeType)) {
            ed = factory->open(fileName,mimeType);
            break;
        }
    }
    if (ed == 0) {
        QString type = "text/liteide.default.editor";
        foreach (IEditorFactory *factory, m_factoryList) {
            if (factory->mimeTypes().contains(type)) {
                ed = factory->open(fileName,type);
                break;
            }
        }
    }
    if (ed) {
        addEditor(ed);
    }
    return ed;
}

void EditorManager::toggleBrowserAction(bool b)
{
    QAction *act = (QAction*)sender();
    if (act) {
        IEditor *editor = m_browserActionMap.key(act);
        if (editor) {
            if (b) {
                addEditor(editor);
                setCurrentEditor(editor);
            } else {
                closeEditor(editor);
            }
        }
    }
}

void EditorManager::modificationChanged(bool b)
{
    IEditor *editor = static_cast<IEditor*>(sender());
    if (editor) {
        QString text = editor->name();
        if (b) {
            text += " *";
        }
        int index = m_editorTabWidget->indexOf(editor->widget());
        if (index >= 0) {
            m_editorTabWidget->setTabText(index,text);
        }
    }
}

