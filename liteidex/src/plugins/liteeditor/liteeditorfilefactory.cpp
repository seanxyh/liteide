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
// Module: liteeditorfilefactory.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: liteeditorfilefactory.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "liteeditorfilefactory.h"
#include "liteeditor.h"
#include "liteeditorwidget.h"
#include "golanghighlighter.h"
#include "litewordcompleter.h"
#include "wordapimanager.h"
#include <QDir>
#include <QFileInfo>
#include "mimetype/mimetype.h"
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

LiteEditorFileFactory::LiteEditorFileFactory(LiteApi::IApplication *app, QObject *parent)
    : LiteApi::IEditorFactory(parent),
      m_liteApp(app)
{
    m_mimeTypes.append("text/x-gosrc");
    m_mimeTypes.append("text/x-gopro");
    m_mimeTypes.append("text/x-lua");
    m_mimeTypes.append("text/x-profile");
    m_mimeTypes.append("text/liteide.default.editor");
    QDir dir(m_liteApp->resourcePath());
    if (dir.cd("katesyntax")) {
        m_kate->loadPath(dir.absolutePath());
        foreach (QString type, m_kate->mimeTypes()) {
            if (!m_liteApp->mimeTypeManager()->findMimeType(type)) {
                MimeType *mimeType = new MimeType;
                mimeType->setType(type);
                foreach(QString pattern, m_kate->mimeTypePatterns(type)) {
                    mimeType->appendGlobPatterns(pattern);
                }
                mimeType->setComment(m_kate->mimeTypeName(type));
                m_liteApp->mimeTypeManager()->addMimeType(mimeType);
            }
            m_mimeTypes.append(type);
        }
    }
    m_mimeTypes.removeDuplicates();

    m_manager = new WordApiManager(this);
    if (m_manager->initWithApp(app)) {
        m_liteApp->extension()->addObject("LiteApi.IWordApiManager",m_manager);
        m_manager->load(m_liteApp->resourcePath()+"/wordapi");
    }
}

QStringList LiteEditorFileFactory::mimeTypes() const
{
    return m_mimeTypes;
}

void LiteEditorFileFactory::colorStyleChanged()
{
    LiteEditor *editor = static_cast<LiteEditor *>(sender());
    if (!editor) {
        return;
    }
    TextEditor::SyntaxHighlighter *h = static_cast<TextEditor::SyntaxHighlighter*>(editor->extension()->findObject("TextEditor::SyntaxHighlighter"));
    if (h) {
        m_kate->setColorStyle(h,editor->colorStyleScheme());
    }
}

LiteApi::IEditor *LiteEditorFileFactory::open(const QString &fileName, const QString &mimeType)
{
    LiteEditor *editor = new LiteEditor(m_liteApp);
    if (!editor->open(fileName,mimeType)) {
        delete editor;
        return 0;
    }

    QTextDocument *doc = editor->m_editorWidget->document();
    TextEditor::SyntaxHighlighter *h = m_kate->create(doc,mimeType);
    if (h) {
        editor->extension()->addObject("TextEditor::SyntaxHighlighter",h);
        connect(editor,SIGNAL(colorStyleChanged()),this,SLOT(colorStyleChanged()));
    }
    editor->applyOption("option/liteeditor");

    LiteWordCompleter *wordCompleter = new LiteWordCompleter(editor);
    editor->setCompleter(wordCompleter);
    if (wordCompleter) {
        LiteApi::IWordApi *wordApi = m_manager->findWordApi(mimeType);
        if (wordApi && wordApi->loadApi()) {
            wordCompleter->appendItems(wordApi->wordList(),false);
        }        
    }
    return editor;
}

bool LiteEditorFileFactory::targetInfo(const QString &fileName, const QString &mimetype, QString &target, QString &targetPath, QString &workPath) const
{
    if (!m_mimeTypes.contains(mimetype)) {
        return false;
    }
    QFileInfo info(fileName);
    target = info.absolutePath()+"/"+info.baseName();
    targetPath = info.absolutePath();
    workPath = info.absolutePath();
    return true;
}
