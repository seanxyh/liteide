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
// Module: liteeditorwidget.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: liteeditorwidget.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "liteeditorwidget.h"

#include <QCompleter>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QtDebug>
#include <QApplication>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QScrollBar>
#include <QTextBlock>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

LiteEditorWidget::LiteEditorWidget(QWidget *parent) :
    LiteEditorWidgetBase(parent), m_completer(0)
{
}

void LiteEditorWidget::setCompleter(QCompleter *completer)
 {
     if (m_completer)
         QObject::disconnect(m_completer, 0, this, 0);

     m_completer = completer;
 }

 QCompleter *LiteEditorWidget::completer() const
 {
     return m_completer;
 }

 QString LiteEditorWidget::wordUnderCursor() const
 {
     QTextCursor tc = textCursor();
     tc.select(QTextCursor::WordUnderCursor);
     return tc.selectedText();
 }

 QString LiteEditorWidget::textUnderCursor(QTextCursor tc) const
 {
     QString text = tc.block().text().left(tc.positionInBlock());
     if (text.isEmpty()) {
         return QString();
     }
     //int index = text.lastIndexOf(QRegExp("\\b[a-zA-Z_][a-zA-Z0-9_\.]+"));
     static QRegExp reg("[a-zA-Z_]+[a-zA-Z0-9_\\.]*$");
     int index = reg.indexIn(text);
     if (index < 0) {
         return QString();
     }
     return text.right(reg.matchedLength());
     //int index = text.lastIndexOf(QRegExp("[\w]+$"));
//     qDebug() << ">" << text << index;
//     int left = text.lastIndexOf(QRegExp("[ |\t|\"|\(|\)|\'|<|>]"));
//     text = text.right(text.length()-left+1);
     return "";
 }

 void LiteEditorWidget::focusInEvent(QFocusEvent *e)
 {
     if (m_completer)
         m_completer->setWidget(this);
     LiteEditorWidgetBase::focusInEvent(e);
 }

 void LiteEditorWidget::keyPressEvent(QKeyEvent *e)
 {
     if (m_completer && m_completer->popup()->isVisible()) {
         // The following keys are forwarded by the completer to the widget
        switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
             e->ignore();
             return; // let the completer do default behavior
        default:
            break;
        }
     }

     bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
     if (!m_completer || !isShortcut) // do not process the shortcut when we have a completer
         LiteEditorWidgetBase::keyPressEvent(e);

     const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
     if (!m_completer || (ctrlOrShift && e->text().isEmpty()))
         return;

     //static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
     static QString eow("~!@#$%^&*()+{}|:\"<>?,/;'[]\\-="); // end of word
     bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
     QString completionPrefix = textUnderCursor(textCursor());

     if (!isShortcut && (hasModifier || e->text().isEmpty()||
         ( completionPrefix.length() < 3 && completionPrefix.right(1) != ".")
                       || eow.contains(e->text().right(1)))) {
         m_completer->popup()->hide();
         return;
     }

     emit completionPrefixChanged(completionPrefix);

     if (completionPrefix != m_completer->completionPrefix()) {
         m_completer->setCompletionPrefix(completionPrefix);
         m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
     }
     QRect cr = cursorRect();
     cr.setWidth(m_completer->popup()->sizeHintForColumn(0)
                 + m_completer->popup()->verticalScrollBar()->sizeHint().width());
     m_completer->complete(cr); // popup it up!
 }
