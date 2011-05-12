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
// Module: liteeditor.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: liteeditor.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "liteeditor.h"
#include "liteeditorwidget.h"
#include "liteeditorfile.h"
#include "litecompleter.h"

#include <QFileInfo>
#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextDocument>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTextCursor>
#include <QToolTip>
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

LiteEditor::LiteEditor(LiteApi::IApplication *app)
    : m_liteApp(app),
      m_extension(new Extension),
      m_completer(0)
{
    m_widget = new QWidget;
    m_editorWidget = new LiteEditorWidget(m_widget);

    createActions();
    createToolBars();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_toolBar);
    layout->addWidget(m_editorWidget);
    m_widget->setLayout(layout);
    m_file = new LiteEditorFile(this);
    m_file->setDocument(m_editorWidget->document());
    connect(m_file->document(),SIGNAL(modificationChanged(bool)),this,SIGNAL(modificationChanged(bool)));
    connect(m_file->document(),SIGNAL(contentsChanged()),this,SIGNAL(contentsChanged()));
    connect(m_liteApp->optionManager(),SIGNAL(applyOption(QString)),this,SLOT(applyOption(QString)));

    applyOption("option/liteeditor");

    m_extension->addObject("LiteApi.ITextEditor",this);
    m_extension->addObject("LiteApi.QToolBar",m_toolBar);
    m_extension->addObject("LiteApi.LiteEditorWidget",m_editorWidget);
    m_extension->addObject("LiteApi.QPlainTextEdit",m_editorWidget);
}

LiteEditor::~LiteEditor()
{
    delete m_extension;
    delete m_editorWidget;
    delete m_widget;
}

void LiteEditor::setCompleter(LiteCompleter *complter)
{
    if (m_completer) {
        QObject::disconnect(m_completer, 0, m_editorWidget, 0);
    }
    m_completer = complter;
    if (!m_completer) {
        return;
    }
    m_completer->setEditor(m_editorWidget);
    m_editorWidget->setCompleter(m_completer->completer());

    m_extension->addObject("LiteApi.ICompleter",complter);

    connect(m_editorWidget,SIGNAL(completionPrefixChanged(QString)),m_completer,SLOT(completionPrefixChanged(QString)));
    connect(m_completer,SIGNAL(wordCompleted(QString,QStringList)),this,SLOT(updateTip(QString,QStringList)));
}

void LiteEditor::clipbordDataChanged()
{
    QClipboard *clipboard = QApplication::clipboard();
    if (clipboard->mimeData()->hasText() ||
            clipboard->mimeData()->hasHtml()) {
        m_pasteAct->setEnabled(true);
    } else {
        m_pasteAct->setEnabled(false);
    }
}

void LiteEditor::createActions()
{
    m_undoAct = new QAction(QIcon(":/images/undo.png"),tr("Undo"),this);
    m_redoAct = new QAction(QIcon(":/images/redo.png"),tr("Redo"),this);
    m_cutAct = new QAction(QIcon(":/images/cut.png"),tr("Cut"),this);
    m_copyAct = new QAction(QIcon(":/images/copy.png"),tr("Copy"),this);
    m_pasteAct = new QAction(QIcon(":/images/paste.png"),tr("Paste"),this);

    m_undoAct->setEnabled(false);
    m_redoAct->setEnabled(false);
    m_cutAct->setEnabled(false);
    m_copyAct->setEnabled(false);
    m_pasteAct->setEnabled(false);

    connect(m_editorWidget,SIGNAL(undoAvailable(bool)),m_undoAct,SLOT(setEnabled(bool)));
    connect(m_editorWidget,SIGNAL(redoAvailable(bool)),m_redoAct,SLOT(setEnabled(bool)));
    connect(m_editorWidget,SIGNAL(copyAvailable(bool)),m_cutAct,SLOT(setEnabled(bool)));
    connect(m_editorWidget,SIGNAL(copyAvailable(bool)),m_copyAct,SLOT(setEnabled(bool)));

    connect(m_undoAct,SIGNAL(triggered()),m_editorWidget,SLOT(undo()));
    connect(m_redoAct,SIGNAL(triggered()),m_editorWidget,SLOT(redo()));
    connect(m_cutAct,SIGNAL(triggered()),m_editorWidget,SLOT(cut()));
    connect(m_copyAct,SIGNAL(triggered()),m_editorWidget,SLOT(copy()));
    connect(m_pasteAct,SIGNAL(triggered()),m_editorWidget,SLOT(paste()));

    QClipboard *clipboard = QApplication::clipboard();
    connect(clipboard,SIGNAL(dataChanged()),this,SLOT(clipbordDataChanged()));
    clipbordDataChanged();
}

void LiteEditor::createToolBars()
{
    m_toolBar = new QToolBar(tr("editor"),m_widget);
    m_toolBar->setIconSize(QSize(16,16));

    m_toolBar->addAction(m_cutAct);
    m_toolBar->addAction(m_copyAct);
    m_toolBar->addAction(m_pasteAct);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_undoAct);
    m_toolBar->addAction(m_redoAct);
    m_toolBar->addSeparator();

    m_findComboBox = new QComboBox(m_widget);
    m_findComboBox->setEditable(true);
    m_findComboBox->setMinimumWidth(120);
    m_findComboBox->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    m_toolBar->addWidget(m_findComboBox);

    m_tip = new QLabel(m_widget);
    m_tip->setAlignment(Qt::AlignCenter);
    m_tip->setText("...");
    m_toolBar->addSeparator();
    m_toolBar->addWidget(m_tip);

    connect(m_findComboBox->lineEdit(),SIGNAL(returnPressed()),this,SLOT(findNextText()));
}

void LiteEditor::findNextText()
{
    QString text = m_findComboBox->lineEdit()->text();
    if (text.isEmpty()) {
        return;
    }
    m_editorWidget->find(text);
}

LiteApi::IExtension *LiteEditor::extension()
{
    return m_extension;
}

QWidget *LiteEditor::widget()
{
    return m_widget;
}

QString LiteEditor::displayName() const
{
    return QFileInfo(m_file->fileName()).fileName();
}

QIcon LiteEditor::icon() const
{
    return QIcon();
}

bool LiteEditor::createNew(const QString &/*contents*/)
{
    return false;
}

bool LiteEditor::open(const QString &fileName,const QString &mimeType)
{
    bool success = m_file->open(fileName,mimeType);
    if (success) {
        m_editorWidget->initLoadDocument();
    }
    return success;
}

bool LiteEditor::save()
{
    return m_file->save(m_file->fileName());
}

bool LiteEditor::isModified()
{
    return m_file->document()->isModified();
}

LiteApi::IFile *LiteEditor::file()
{
    return m_file;
}

int LiteEditor::line() const
{
    return 0;
}

int LiteEditor::column() const
{
    return 0;
}

void LiteEditor::gotoLine(int line, int column)
{
    m_editorWidget->gotoLine(line,column);
}

void LiteEditor::applyOption(QString id)
{
    if (id != "option/liteeditor") {
        return;
    }
    QString fontFamily = m_liteApp->settings()->value("editor/family","Courier").toString();
    int fontSize = m_liteApp->settings()->value("editor/fontsize",12).toInt();
    QFont font = m_editorWidget->font();
    font.setFamily(fontFamily);
    font.setPointSize(fontSize);
    m_editorWidget->setFont(font);
    m_editorWidget->setTabWidth(4);
}

void LiteEditor::updateTip(QString func,QStringList args)
{
    if (args.isEmpty()) {
        return;
    }
    m_tip->setToolTip(func+args.join(";"));
    QToolTip::showText(m_toolBar->mapToGlobal(m_tip->pos()),func+args.join(";"),m_tip);
}
