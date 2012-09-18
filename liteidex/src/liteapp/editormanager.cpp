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
#include <QEvent>
#include <QKeyEvent>
#include <QTabBar>
#include <QApplication>
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

    m_currentNavigationHistoryPosition = 0;
    m_widget = new QWidget;
    m_editorTabWidget = new LiteTabWidget;
    m_editorTabWidget->tabBar()->setTabsClosable(true);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(1);
    mainLayout->addWidget(m_editorTabWidget);
    m_widget->setLayout(mainLayout);

    QToolBar *toolbar = m_liteApp->actionManager()->insertToolBar("toolbar/nav",tr("Navigation"));
    m_goBackAct = new QAction(tr("GoBack"),this);
    m_goBackAct->setIcon(QIcon("icon:images/backward.png"));
    m_goBackAct->setShortcut(QKeySequence(Qt::ALT+Qt::Key_Left));

    m_goForwardAct = new QAction(tr("GoForward"),this);
    m_goForwardAct->setIcon(QIcon("icon:images/forward.png"));
    m_goForwardAct->setShortcut(Qt::ALT+Qt::Key_Right);

    updateNavigatorActions();

    toolbar->addAction(m_goBackAct);
    toolbar->addAction(m_goForwardAct);

    connect(m_editorTabWidget,SIGNAL(currentChanged(int)),this,SLOT(editorTabChanged(int)));
    connect(m_editorTabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(editorTabCloseRequested(int)));
    connect(m_editorTabWidget,SIGNAL(tabAddRequest()),this,SIGNAL(tabAddRequest()));
    connect(m_goBackAct,SIGNAL(triggered()),this,SLOT(goBack()));
    connect(m_goForwardAct,SIGNAL(triggered()),this,SLOT(goForward()));

    m_editorTabWidget->installEventFilter(this);
    m_editorTabWidget->tabBar()->installEventFilter(this);

    createActions();
    return true;
}

void EditorManager::createActions()
{
    QAction *undo = new QAction(tr("Undo"),this);
    undo->setData(EA_UNDO);

    QAction *redo = new QAction(tr("Redo"),this);
    redo->setData(EA_REDO);

    QAction *cut = new QAction(tr("Cut"),this);
    cut->setShortcut(QKeySequence::Cut);
    cut->setData(EA_CUT);

    QAction *copy = new QAction(tr("Copy"),this);
    copy->setData(EA_COPY);

    QAction *paste = new QAction(tr("paste"),this);
    paste->setData(EA_PASTE);

    QAction *selectAll = new QAction(tr("Select All"),this);
    selectAll->setData(EA_SELECTALL);

    QMenu *menu = m_liteApp->actionManager()->loadMenu("edit");
    if (!menu) {
        menu = m_liteApp->actionManager()->insertMenu("edit",tr("Edit"));
    }


    menu->addAction(undo);
    menu->addAction(redo);
    menu->addSeparator();
    menu->addAction(cut);
    menu->addAction(copy);
    menu->addAction(paste);
    menu->addSeparator();
    menu->addAction(selectAll);

    connect(menu,SIGNAL(triggered(QAction*)),this,SLOT(executeEditAction(QAction*)));

    m_actionList << undo << redo << cut << copy << paste << selectAll;
}

void EditorManager::executeEditAction(QAction *action)
{
    bool ok = false;
    EditorAction id = (EditorAction)action->data().toInt(&ok);
    if (ok && m_currentEditor) {
        m_currentEditor->executeAction(id);
    }
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
        m_editorTabWidget->addTab(w,QIcon(),editor->name(),editor->filePath());
        m_widgetEditorMap.insert(w,editor);
        emit editorCreated(editor);
        connect(editor,SIGNAL(modificationChanged(bool)),this,SLOT(modificationChanged(bool)));
    }
}

bool EditorManager::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *e = static_cast<QKeyEvent*>(event);
        if ( (e->modifiers() & Qt::CTRL) &&
             ( e->key() == Qt::Key_Tab || e->key() == Qt::Key_Backtab) ) {
            int index = m_editorTabWidget->tabBar()->currentIndex();
            if (e->key() == Qt::Key_Tab) {
                index++;
                if (index >= m_editorTabWidget->tabBar()->count()) {
                    index = 0;
                }
            } else {
                index--;
                if (index < 0) {
                    index = m_editorTabWidget->tabBar()->count()-1;
                }
            }
            m_editorTabWidget->setCurrentIndex(index);
            return true;
        }
    } else if (event->type() == QEvent::MouseButtonDblClick && target == m_editorTabWidget->tabBar()) {
        emit doubleClickedTab();
    }
    return IEditorManager::eventFilter(target,event);
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

    if (cur->isModified() && !cur->isReadOnly()) {
        QString text = QString(tr("%1 is modified.")).arg(cur->filePath());
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

bool EditorManager::saveEditor(IEditor *editor, bool emitAboutSave)
{
    IEditor *cur = 0;
    if (editor) {
        cur = editor;
    } else {
        cur = m_currentEditor;
    }

    if (cur) {
        if (emitAboutSave) {
            emit editorAboutToSave(cur);
        }
        if (cur->save()) {
            emit editorSaved(cur);
        }
        return true;
    }
    return false;
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
    QString fileName = cur->filePath();
    if (fileName.isEmpty()) {
        return false;
    }

    QFileInfo info(fileName);
    QStringList filter;
    QString ext = info.suffix();
    if (!ext.isEmpty()) {
        filter.append(QString("%1 (*.%1)").arg(ext).arg(ext));
    }
    filter.append(tr("All Files (*)"));
    QString path = info.absolutePath();
    QString saveFileName = QFileDialog::getSaveFileName(m_liteApp->mainWindow(),tr("Save As"),path,filter.join(";;"));
    if (FileUtil::compareFile(fileName,saveFileName,false)) {
        return false;
    }
    if (!cur->saveAs(saveFileName)) {
        return false;
    }
    QWidget *w = m_widgetEditorMap.key(cur,0);
    if(w) {
        int index = m_editorTabWidget->indexOf(w);
        m_editorTabWidget->setTabText(index,cur->name());
    }

    emit currentEditorChanged(cur);
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
        editor->onActive();
    }

    emit currentEditorChanged(editor);
}

IEditor *EditorManager::findEditor(const QString &fileName, bool canonical) const
{
    QMapIterator<QWidget *, IEditor *> i(m_widgetEditorMap);
    while (i.hasNext()) {
        i.next();
        if (FileUtil::compareFile(i.value()->filePath(),fileName,canonical)) {
            return i.value();
        }
    }
    return 0;
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
    IEditor *editor = findEditor(fileName,true);
    if (editor) {
        return editor;
    }
    foreach (IEditorFactory *factory, m_factoryList) {
        if (factory->mimeTypes().contains(mimeType)) {
            editor = factory->open(fileName,mimeType);
            if (editor) {
                break;
            }
        }
    }
    if (editor == 0) {
        QString type = "liteide/default.editor";
        foreach (IEditorFactory *factory, m_factoryList) {
            if (factory->mimeTypes().contains(type)) {
                editor = factory->open(fileName,type);
                if (editor) {
                    break;
                }
            }
        }
    }
    if (editor) {
        addEditor(editor);
    }
    return editor;
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

void EditorManager::addNavigationHistory(IEditor *editor,const QByteArray &saveState)
{
    if (editor && editor != currentEditor()) {
        return; // we only save editor sate for the current editor, when the user interacts
    }

    if (!editor)
        editor = currentEditor();
    if (!editor)
        return;

    QString filePath = editor->filePath();
    if (filePath.isEmpty()) {
        return;
    }

    QByteArray state;
    if (saveState.isNull()) {
        state = editor->saveState();
    } else {
        state = saveState;
    }

    EditLocation location;
    location.filePath = filePath;
    location.state = state;
    m_currentNavigationHistoryPosition = qMin(m_currentNavigationHistoryPosition, m_navigationHistory.size()); // paranoia
    m_navigationHistory.insert(m_currentNavigationHistoryPosition, location);
    ++m_currentNavigationHistoryPosition;

    while (m_navigationHistory.size() >= 30) {
        if (m_currentNavigationHistoryPosition > 15) {
            m_navigationHistory.removeFirst();
            --m_currentNavigationHistoryPosition;
        } else {
            m_navigationHistory.removeLast();
        }
    }
    updateNavigatorActions();
}

void EditorManager::goBack()
{
    updateCurrentPositionInNavigationHistory();
    while (m_currentNavigationHistoryPosition > 0) {
        --m_currentNavigationHistoryPosition;
        EditLocation location = m_navigationHistory.at(m_currentNavigationHistoryPosition);
        IEditor *editor = m_liteApp->fileManager()->openEditor(location.filePath,true);
        if (editor) {
            editor->restoreState(location.state);
        } else {
            m_navigationHistory.removeAt(m_currentNavigationHistoryPosition);
        }
        break;
    }
    updateNavigatorActions();
}

void EditorManager::goForward()
{
    updateCurrentPositionInNavigationHistory();
    if (m_currentNavigationHistoryPosition >= m_navigationHistory.size()-1)
        return;
    ++m_currentNavigationHistoryPosition;
    EditLocation location = m_navigationHistory.at(m_currentNavigationHistoryPosition);
    IEditor *editor = m_liteApp->fileManager()->openEditor(location.filePath);
    if (!editor) {
        return;
    }
    editor->restoreState(location.state);
    updateNavigatorActions();
}

void EditorManager::updateNavigatorActions()
{
    m_goBackAct->setEnabled(m_currentNavigationHistoryPosition > 0);
    m_goForwardAct->setEnabled(m_currentNavigationHistoryPosition < m_navigationHistory.size()-1);
}

void EditorManager::cutForwardNavigationHistory()
{
    while (m_currentNavigationHistoryPosition < m_navigationHistory.size() - 1)
        m_navigationHistory.removeLast();
}

void EditorManager::updateCurrentPositionInNavigationHistory()
{
    IEditor *editor = currentEditor();
    if (!editor)
        return;
    QString filePath = editor->filePath();
    if(filePath.isEmpty()) {
        return;
    }

    EditLocation *location;
    if (m_currentNavigationHistoryPosition < m_navigationHistory.size()) {
        location = &m_navigationHistory[m_currentNavigationHistoryPosition];
    } else {
        m_navigationHistory.append(EditLocation());
        location = &m_navigationHistory[m_navigationHistory.size()-1];
    }
    location->filePath = filePath;
    location->state = editor->saveState();
}

void EditorManager::setActionEnable(IEditor *editor, EditorAction id, bool b)
{
    if (editor != m_currentEditor) {
        return;
    }
    foreach(QAction* act, m_actionList) {
        if (act->data().toInt() == id) {
            act->setEnabled(b);
            break;
        }
    }
}
