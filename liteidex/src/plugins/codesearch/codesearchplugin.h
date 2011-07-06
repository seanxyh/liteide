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
// Module: codesearchplugin.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-5
// $Id: codesearchplugin.h,v 1.0 2011-7-6 visualfc Exp $

#ifndef CODESEARCHPLUGIN_H
#define CODESEARCHPLUGIN_H

#include "codesearch_global.h"
#include "liteapi.h"
#include <QtPlugin>

class EditorSearchDialog;
class EditorReplaceDialog;

class CodeSearchPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPlugin)
public:
    CodeSearchPlugin();
    ~CodeSearchPlugin();
    virtual bool initWithApp(LiteApi::IApplication *app);
protected slots:
    void editorCreated(LiteApi::IEditor*);
    void editorSearch();
    void editorReplace();
protected:
    EditorSearchDialog *m_editorSearchDialog;
    EditorReplaceDialog *m_editorReplaceDialog;
    QMenu       *m_toolMenu;
    QAction *m_findNext;
    QAction *m_findPrev;
    QAction *m_editorSearchAct;
    QAction *m_editorReplaceAct;
};

#endif // CODESEARCHPLUGIN_H
