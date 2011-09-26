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
#include "terminaledit.h"

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

TextOutput::TextOutput(bool readOnly, QWidget *parent) :
    QWidget(parent)
{
    m_toolBar = new QToolBar(this);
    m_toolBar->setIconSize(QSize(16,16));

    m_editor = new TerminalEdit(this);
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
    m_infoLabel = new QLabel(this);
    m_infoLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    m_toolBar->addAction(m_clearAct);
    m_toolBar->addSeparator();
    m_toolBar->addWidget(m_infoLabel);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_hideAct);

    connect(m_editor,SIGNAL(dbclickEvent(QTextCursor)),this,SIGNAL(dbclickEvent(QTextCursor)));
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
    m_editor->append(text,&m_fmt);
}

void TextOutput::append(const QString &text,const QBrush &foreground)
{
    QTextCharFormat f = m_fmt;
    f.setForeground(foreground);
    m_editor->append(text,&f);
}

void TextOutput::appendTag0(const QString &text)
{
    QTextCharFormat f = m_fmt;
    f.setFontWeight(QFont::Bold);
    f.setForeground(Qt::darkBlue);
    m_editor->append(text,&f);
}

void TextOutput::appendTag1(const QString &text)
{
    QTextCharFormat f = m_fmt;
    f.setForeground(Qt::darkBlue);
    m_editor->append(text,&f);
}

void TextOutput::updateExistsTextColor(const QBrush &foreground)
{
    QTextCharFormat f = m_fmt;
    f.setForeground(foreground);
    QTextCursor all = m_editor->textCursor();
    all.select(QTextCursor::Document);
    all.setCharFormat(f);
}

void TextOutput::setReadOnly(bool bo)
{
    m_editor->setReadOnly(bo);
}

void TextOutput::setMaxLine(int max)
{
    m_editor->setMaximumBlockCount(max);;
}
