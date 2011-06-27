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
// Module: liteeditor.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: liteeditor.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef LITEEDITOR_H
#define LITEEDITOR_H

#include "liteapi.h"
#include "extension/extension.h"
#include <QSet>
#include <QHash>

class TreeModelCompleter;
class QAbstractItemModel;
class QStandardItemModel;
class QStandardItem;
class LiteEditorWidget;
class LiteEditorFile;
class QComboBox;
class QLabel;
class LiteCompleter;

class LiteEditor : public LiteApi::ITextEditor
{
    Q_OBJECT
public:
    LiteEditor(LiteApi::IApplication *app);
    virtual ~LiteEditor();
    void setCompleter(LiteCompleter *complter);
    void createActions();
    void createToolBars();
    virtual LiteApi::IExtension *extension();
    virtual QWidget *widget();
    virtual QString displayName() const;
    virtual QIcon icon() const;

    virtual bool createNew(const QString &contents);
    virtual bool open(const QString &fileName, const QString &mimeType);
    virtual bool save();
    virtual bool isModified();
    virtual LiteApi::IFile *file();

    virtual int line() const;
    virtual int column() const;
    virtual void gotoLine(int line, int column);
public slots:
    void applyOption(QString);
    void clipbordDataChanged();
    void findNextText();
    void updateTip(QString,QStringList);
    void printPdf();
public:
    LiteApi::IApplication *m_liteApp;
    Extension   *m_extension;
    QWidget *m_widget;
    QToolBar *m_toolBar;
    LiteEditorWidget    *m_editorWidget;
    LiteCompleter   *m_completer;
    QAction *m_undoAct;
    QAction *m_redoAct;
    QAction *m_cutAct;
    QAction *m_copyAct;
    QAction *m_pasteAct;
    QAction *m_printPdfAct;
    QComboBox *m_findComboBox;
    QLabel    *m_tip;
    LiteEditorFile *m_file;
};

#endif //LITEEDITOR_H
