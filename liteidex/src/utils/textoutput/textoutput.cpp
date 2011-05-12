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
// Module: textoutput.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: textoutput.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "textoutput.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextCharFormat>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTextCursor>
#include <QToolBar>
#include <QAction>
#include <QLabel>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

TextOutEdit::TextOutEdit(QWidget *parent) : QTextEdit(parent)
{
}

void TextOutEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
    e->accept();
    emit dbclickEvent();
}

void TextOutEdit::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
        emit enterText(m_inputText);
        m_inputText.clear();
        e->ignore();
        return;
    } else {
        m_inputText += e->text();
    }
    QTextEdit::keyPressEvent(e);
}


TextOutput::TextOutput(bool readOnly, QWidget *parent) :
    QWidget(parent)
{
    m_toolBar = new QToolBar(this);
    m_toolBar->setIconSize(QSize(16,16));

    m_editor = new TextOutEdit(this);
    m_editor->setReadOnly(readOnly);
    m_fmt = m_editor->currentCharFormat();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_toolBar);
    mainLayout->addWidget(m_editor);
    setLayout(mainLayout);

    m_hideAct = new QAction(tr("Hide"),this);
    m_hideAct->setToolTip("Hide");
    m_hideAct->setIcon(QIcon(":/images/hideoutput.png"));
    m_clearAct = new QAction(tr("Clear"),this);
    m_clearAct->setIcon(QIcon(":/images/cleanoutput.png"));
    m_space = new QLabel(this);
    m_space->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    m_toolBar->addAction(m_clearAct);
    m_toolBar->addWidget(m_space);
    m_toolBar->addAction(m_hideAct);

    connect(m_editor,SIGNAL(dbclickEvent()),this,SIGNAL(dbclickEvent()));
    connect(m_editor,SIGNAL(enterText(QString)),this,SIGNAL(enterText(QString)));
    connect(m_clearAct,SIGNAL(triggered()),this,SLOT(clear()));
    connect(m_hideAct,SIGNAL(triggered()),this,SIGNAL(hideOutput()));
}

void TextOutput::clear()
{
    m_editor->clear();
}

void TextOutput::append(const QString &text)
{
    m_editor->setCurrentCharFormat(m_fmt);
    m_editor->append(text);
}

void TextOutput::append(const QString &text,const QBrush &foreground)
{
    QTextCharFormat f = m_fmt;
    f.setForeground(foreground);
    m_editor->setCurrentCharFormat(f);
    m_editor->append(text);
}

void TextOutput::appendTag0(const QString &text)
{
    QTextCharFormat f = m_fmt;
    f.setFontWeight(QFont::Bold);
    f.setForeground(Qt::darkBlue);
    m_editor->setCurrentCharFormat(f);
    m_editor->append(text);
}

void TextOutput::appendTag1(const QString &text)
{
    QTextCharFormat f = m_fmt;
    f.setForeground(Qt::darkBlue);
    m_editor->setCurrentCharFormat(f);
    m_editor->append(text);
}

QTextEdit *TextOutput::textEdit()
{
    return m_editor;
}

void TextOutput::updateExistsTextColor(const QBrush &foreground)
{
    QTextCharFormat f = m_fmt;
    f.setForeground(foreground);
    QTextCursor cur = m_editor->textCursor();
    QTextCursor all = m_editor->textCursor();
    all.select(QTextCursor::Document);
    all.setCharFormat(f);
    m_editor->setTextCursor(all);
    m_editor->setTextCursor(cur);
    m_editor->setCurrentCharFormat(m_fmt);
}
