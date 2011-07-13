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
// Module: codesearchplugin.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-5
// $Id: codesearchplugin.cpp,v 1.0 2011-7-13 visualfc Exp $

#include "codesearchplugin.h"
#include "liteapi/litefindobj.h"
#include "editorsearchdialog.h"
#include "editorreplacedialog.h"

#include <QToolBar>
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QPlainTextEdit>
#include <QTextCursor>
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

CodeSearchPlugin::CodeSearchPlugin()
    : m_editorSearchDialog(0),
      m_editorReplaceDialog(0),
      m_toolMenu(0)
{
    m_info->setId("plugin/codesearch");
    m_info->setName("CodeSearch");
    m_info->setAnchor("visualfc");
    m_info->setInfo("CodeSearch Plugin");
}

CodeSearchPlugin::~CodeSearchPlugin()
{
    if (m_toolMenu) {
        delete m_toolMenu;
    }
    if (m_editorSearchDialog) {
        delete m_editorSearchDialog;
    }
    if (m_editorReplaceDialog) {
        delete m_editorReplaceDialog;
    }
}

bool CodeSearchPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }

    m_editorSearchAct = new QAction(QIcon(":/images/search.png"),tr("Find"),this);
    m_editorSearchAct->setShortcut(QKeySequence::Find);

    m_editorReplaceAct = new QAction(QIcon(":/images/replace.png"),tr("Replace"),this);
    m_editorReplaceAct->setShortcut(QKeySequence::Replace);

    m_findNext = new QAction(tr("Find Next"),this);
    m_findPrev = new QAction(tr("Find Prev"),this);
    m_findNext->setShortcut(QKeySequence::FindNext);
    m_findPrev->setShortcut(QKeySequence::FindPrevious);

    m_toolMenu = new QMenu(tr("Find"));
    m_toolMenu->addAction(m_editorSearchAct);
    m_toolMenu->addAction(m_editorReplaceAct);
    m_toolMenu->addSeparator();
    m_toolMenu->addAction(m_findNext);
    m_toolMenu->addAction(m_findPrev);

    connect(m_liteApp->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(m_editorSearchAct,SIGNAL(triggered()),this,SLOT(editorSearch()));
    connect(m_editorReplaceAct,SIGNAL(triggered()),this,SLOT(editorReplace()));

    return true;
}

void CodeSearchPlugin::editorCreated(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }
    LiteApi::ITextEditor *textEditor = LiteApi::findExtensionObject<LiteApi::ITextEditor*>(editor,"LiteApi.ITextEditor");
    if (!textEditor) {
        return;
    }
    QToolBar *toolBar = LiteApi::findExtensionObject<QToolBar*>(editor,"LiteApi.QToolBar");
    if (!toolBar) {
        return;
    }

    QToolButton *toolButton = new QToolButton;
    toolButton->setDefaultAction(m_editorSearchAct);
    toolButton->setPopupMode(QToolButton::MenuButtonPopup);
    toolButton->setMenu(m_toolMenu);
    toolBar->addSeparator();
    toolBar->addWidget(toolButton);
}

void CodeSearchPlugin::editorSearch()
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }

    QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
    if (!ed) {
        return;
    }

    if (m_editorSearchDialog == 0) {
        m_editorSearchDialog = new EditorSearchDialog(m_liteApp,m_liteApp->mainWindow());
        connect(m_findNext,SIGNAL(triggered()),m_editorSearchDialog,SLOT(findNext()));
        connect(m_findPrev,SIGNAL(triggered()),m_editorSearchDialog,SLOT(findPrev()));
    }

    QTextCursor cursor = ed->textCursor();
    if (!cursor.isNull()) {
        QString text = cursor.selectedText().trimmed();
        if (!text.isEmpty()) {
            m_editorSearchDialog->setFindText(text);
        }
    }

    m_editorSearchDialog->exec();
}

void CodeSearchPlugin::editorReplace()
{
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
    if (!ed) {
        return;
    }
    if (m_editorReplaceDialog == 0) {
        m_editorReplaceDialog = new EditorReplaceDialog(m_liteApp,m_liteApp->mainWindow());
    }
    QTextCursor cursor = ed->textCursor();
    if (!cursor.isNull()) {
        QString text = cursor.selectedText().trimmed();
        if (!text.isEmpty()) {
            m_editorReplaceDialog->setFindText(text);
        }
    }
    m_editorReplaceDialog->exec();
}


Q_EXPORT_PLUGIN(CodeSearchPlugin)
