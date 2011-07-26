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
// Module: liteeditorwidgetbase.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: liteeditorwidgetbase.cpp,v 1.0 2011-7-26 visualfc Exp $

#include "liteeditorwidgetbase.h"
//#include "basetextdocumentlayout.h"

#include <QCoreApplication>
#include <QTextBlock>
#include <QPainter>
#include <QStyle>
#include <QDebug>
#include <QMessageBox>
#include <QToolTip>
#include <QTextCursor>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

//using namespace TextEditor;

class TextEditExtraArea : public QWidget {
public:
    TextEditExtraArea(LiteEditorWidgetBase *edit):QWidget(edit) {
        textEdit = edit;
        setAutoFillBackground(true);
    }
public:

    QSize sizeHint() const {
        return QSize(textEdit->extraAreaWidth(), 0);
    }
protected:
    void paintEvent(QPaintEvent *event){
        textEdit->extraAreaPaintEvent(event);
    }
    void mousePressEvent(QMouseEvent *event){
        textEdit->extraAreaMouseEvent(event);
    }
    void mouseMoveEvent(QMouseEvent *event){
        textEdit->extraAreaMouseEvent(event);
    }
    void mouseReleaseEvent(QMouseEvent *event){
        textEdit->extraAreaMouseEvent(event);
    }
    void leaveEvent(QEvent *event){
        textEdit->extraAreaLeaveEvent(event);
    }

    void wheelEvent(QWheelEvent *event) {
        QCoreApplication::sendEvent(textEdit->viewport(), event);
    }
protected:
    LiteEditorWidgetBase *textEdit;
};

LiteEditorWidgetBase::LiteEditorWidgetBase(QWidget *parent)
    : QPlainTextEdit(parent)
{
    setLineWrapMode(QPlainTextEdit::NoWrap);
    //document()->setDocumentLayout(new BaseTextDocumentLayout(document()));
    m_extraArea = new TextEditExtraArea(this);

    setLayoutDirection(Qt::LeftToRight);
    viewport()->setMouseTracking(true);
    m_lineNumbersVisible = true;
    m_marksVisible = false;
    m_lastSaveRevision = 0;
    m_extraAreaSelectionNumber = -1;
    m_autoIndent = true;
    m_autoBraces = true;
    setTabWidth(4);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(slotUpdateExtraAreaWidth()));
    connect(this, SIGNAL(modificationChanged(bool)), this, SLOT(slotModificationChanged(bool)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(slotCursorPositionChanged()));
    connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(slotUpdateRequest(QRect, int)));
    //connect(this, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
    //updateHighlights();
    //setFrameStyle(QFrame::NoFrame);
}

void LiteEditorWidgetBase::setTabWidth(int n)
{
    int charWidth = QFontMetrics(font()).averageCharWidth();
    setTabStopWidth(charWidth * n);
}

void LiteEditorWidgetBase::initLoadDocument()
{
    m_lastSaveRevision = document()->revision();
    document()->setModified(false);
    moveCursor(QTextCursor::Start);
}

void LiteEditorWidgetBase::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(180,200,200,128);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}


int LiteEditorWidgetBase::extraAreaWidth()
{
    int space = 0;
    const QFontMetrics fm(m_extraArea->fontMetrics());
    if (m_lineNumbersVisible) {
        QFont fnt = m_extraArea->font();
        fnt.setBold(true);
        const QFontMetrics linefm(fnt);
        int digits = 2;
        int max = qMax(1, blockCount());
        while (max >= 100) {
            max /= 10;
            ++digits;
        }
        space += linefm.width(QLatin1Char('9')) * digits;
    }
    if (m_marksVisible) {
        int markWidth = fm.lineSpacing();
        space += markWidth;
    } else {
        space += 2;
    }
    space += 4;
    return space;
}

void LiteEditorWidgetBase::extraAreaPaintEvent(QPaintEvent *e)
{
    QTextDocument *doc = document();

    int selStart = textCursor().selectionStart();
    int selEnd = textCursor().selectionEnd();

    QPalette pal = m_extraArea->palette();
    pal.setCurrentColorGroup(QPalette::Active);
    QPainter painter(m_extraArea);
    const QFontMetrics fm(m_extraArea->font());

    int markWidth = 0;
    if (m_marksVisible)
        markWidth += fm.lineSpacing();

    const int extraAreaWidth = m_extraArea->width();

    painter.fillRect(e->rect(), pal.color(QPalette::Base));
    painter.fillRect(e->rect().intersected(QRect(0, 0, extraAreaWidth, INT_MAX)),
                     pal.color(QPalette::Background));

    painter.setPen(QPen(Qt::darkCyan,1,Qt::DotLine));
    painter.drawLine(extraAreaWidth - 3, e->rect().top(), extraAreaWidth - 3, e->rect().bottom());

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    qreal top = blockBoundingGeometry(block).translated(contentOffset()).top();
    qreal bottom = top;

    while (block.isValid() && top <= e->rect().bottom()) {

        top = bottom;
        const qreal height = blockBoundingRect(block).height();
        bottom = top + height;
        QTextBlock nextBlock = block.next();

        QTextBlock nextVisibleBlock = nextBlock;
        int nextVisibleBlockNumber = blockNumber + 1;

        if (!nextVisibleBlock.isVisible()) {
            // invisible blocks do have zero line count
            nextVisibleBlock = doc->findBlockByLineNumber(nextVisibleBlock.firstLineNumber());
            nextVisibleBlockNumber = nextVisibleBlock.blockNumber();
        }

        if (bottom < e->rect().top()) {
            block = nextVisibleBlock;
            blockNumber = nextVisibleBlockNumber;
            continue;
        }

        painter.setPen(pal.color(QPalette::Dark));

        if (block.revision() != m_lastSaveRevision) {
            painter.save();
            painter.setRenderHint(QPainter::Antialiasing, false);
            if (block.revision() < 0)
                painter.setPen(QPen(Qt::darkGreen, 2));
            else
                painter.setPen(QPen(Qt::red, 2));
            painter.drawLine(extraAreaWidth - 1, top, extraAreaWidth - 1, bottom - 1);
            painter.restore();
        }

        painter.setPen(QPen(Qt::darkCyan,2));//pal.color(QPalette::BrightText));
        if (m_lineNumbersVisible) {
            const QString &number = QString::number(blockNumber + 1);
            bool selected = (
                    (selStart < block.position() + block.length()
                    && selEnd > block.position())
                    || (selStart == selEnd && selStart == block.position())
                    );
            if (selected) {
                painter.save();
                QFont f = painter.font();
                f.setBold(true);
                painter.setFont(f);
                //painter.setPen(QPen(Qt::black,2));
            }
            painter.drawText(QRectF(markWidth, top, extraAreaWidth - markWidth - 4, height), Qt::AlignRight, number);
            if (selected)
                painter.restore();
        }

        block = nextVisibleBlock;
        blockNumber = nextVisibleBlockNumber;
    }

}

void LiteEditorWidgetBase::extraAreaMouseEvent(QMouseEvent *e)
{
    QTextCursor cursor = cursorForPosition(QPoint(0, e->pos().y()));
    if (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseButtonDblClick) {
        if (e->button() == Qt::LeftButton) {
            QTextCursor selection = cursor;
            selection.setVisualNavigation(true);
            m_extraAreaSelectionNumber = selection.blockNumber();
            selection.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            selection.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
            setTextCursor(selection);
        }
    } else if (m_extraAreaSelectionNumber >= 0) {
        QTextCursor selection = cursor;
        selection.setVisualNavigation(true);
        if (e->type() == QEvent::MouseMove) {
            QTextBlock anchorBlock = document()->findBlockByNumber(m_extraAreaSelectionNumber);
            selection.setPosition(anchorBlock.position());
            if (cursor.blockNumber() < m_extraAreaSelectionNumber) {
                selection.movePosition(QTextCursor::EndOfBlock);
                selection.movePosition(QTextCursor::Right);
            }
            selection.setPosition(cursor.block().position(), QTextCursor::KeepAnchor);
            if (cursor.blockNumber() >= m_extraAreaSelectionNumber) {
                selection.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                selection.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
            }
        } else {
            m_extraAreaSelectionNumber = -1;
            return;
        }
        setTextCursor(selection);
    }
}

void LiteEditorWidgetBase::extraAreaLeaveEvent(QEvent *)
{

}

void LiteEditorWidgetBase::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    m_extraArea->setGeometry(
        QStyle::visualRect(layoutDirection(), cr,
                           QRect(cr.left(), cr.top(), extraAreaWidth(), cr.height())));
}


void LiteEditorWidgetBase::slotUpdateExtraAreaWidth()
{
    if (isLeftToRight())
        setViewportMargins(extraAreaWidth(), 0, 0, 0);
    else
        setViewportMargins(0, 0, extraAreaWidth(), 0);
}

void LiteEditorWidgetBase::slotModificationChanged(bool m)
{
    if (m)
        return;

    int oldLastSaveRevision = m_lastSaveRevision;
    m_lastSaveRevision = document()->revision();

    if (oldLastSaveRevision != m_lastSaveRevision) {
        QTextBlock block = document()->begin();
        while (block.isValid()) {
            if (block.revision() < 0 || block.revision() != oldLastSaveRevision) {
                block.setRevision(-m_lastSaveRevision - 1);
            } else {
                block.setRevision(m_lastSaveRevision);
            }
            block = block.next();
        }
    }
    m_extraArea->update();
}

void LiteEditorWidgetBase::slotUpdateRequest(const QRect &r, int dy)
{
    if (dy)
        m_extraArea->scroll(0, dy);
    else if (r.width() > 4) { // wider than cursor width, not just cursor blinking
        m_extraArea->update(0, r.y(), m_extraArea->width(), r.height());
        //if (!d->m_searchExpr.isEmpty()) {
        //    const int m = d->m_searchResultOverlay->dropShadowWidth();
        //    viewport()->update(r.adjusted(-m, -m, m, m));
        //}
    }

    if (r.contains(viewport()->rect()))
        slotUpdateExtraAreaWidth();
}

void LiteEditorWidgetBase::slotCursorPositionChanged()
{
    highlightCurrentLine();
}

void LiteEditorWidgetBase::slotUpdateBlockNotify(const QTextBlock &)
{

}

void LiteEditorWidgetBase::gotoLine(int line, int column)
{
    const int blockNumber = line - 1;
    const QTextBlock &block = document()->findBlockByNumber(blockNumber);
    if (block.isValid()) {
        QTextCursor cursor(block);
        if (column > 0) {
            cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column);
        } else {
            int pos = cursor.position();
            while (document()->characterAt(pos).category() == QChar::Separator_Space) {
                ++pos;
            }
            cursor.setPosition(pos);
        }
        setTextCursor(cursor);
        centerCursor();
    }
}

QChar LiteEditorWidgetBase::characterAt(int pos) const
{
    return document()->characterAt(pos);
}
void LiteEditorWidgetBase::handleHomeKey(bool anchor)
{
    QTextCursor cursor = textCursor();
    QTextCursor::MoveMode mode = QTextCursor::MoveAnchor;

    if (anchor)
        mode = QTextCursor::KeepAnchor;

    const int initpos = cursor.position();
    int pos = cursor.block().position();
    QChar character = characterAt(pos);
    const QLatin1Char tab = QLatin1Char('\t');

    while (character == tab || character.category() == QChar::Separator_Space) {
        ++pos;
        if (pos == initpos)
            break;
        character = characterAt(pos);
    }

    // Go to the start of the block when we're already at the start of the text
    if (pos == initpos)
        pos = cursor.block().position();

    cursor.setPosition(pos, mode);
    setTextCursor(cursor);
}


void LiteEditorWidgetBase::gotoLineStart()
{
    handleHomeKey(false);
}

void LiteEditorWidgetBase::gotoLineStartWithSelection()
{
    handleHomeKey(true);
}

void LiteEditorWidgetBase::gotoLineEnd()
{
    moveCursor(QTextCursor::EndOfLine);
}

void LiteEditorWidgetBase::gotoLineEndWithSelection()
{
    moveCursor(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
}

void LiteEditorWidgetBase::keyPressEvent(QKeyEvent *e)
{
    bool ro = isReadOnly();

    QChar mr;
    switch (e->key()) {
        case '{':
            mr = '}';
            break;
        case '(':
            mr = ')';
            break;
        case '[':
            mr = ']';
            break;
        case '\'':
            mr = '\'';
            break;
        case '\"':
            mr = '\"';
            break;
    }
    if (!mr.isNull() && m_autoBraces) {
        QPlainTextEdit::keyPressEvent(e);
        QTextCursor cursor = textCursor();
        int pos = cursor.positionInBlock();
        QString text = cursor.block().text();
        if (pos <= text.length() && text.at(pos) != mr) {
            cursor.beginEditBlock();
            pos = cursor.position();
            cursor.insertText(mr);
            cursor.setPosition(pos);
            cursor.endEditBlock();
            setTextCursor(cursor);
        }
        return;
    }
    if ( e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return ) {
        if (m_autoIndent) {
            indentEnter(textCursor());
            return;
        }
    }
    if (e == QKeySequence::MoveToStartOfBlock
            || e == QKeySequence::SelectStartOfBlock){
        if ((e->modifiers() & (Qt::AltModifier | Qt::ShiftModifier)) == (Qt::AltModifier | Qt::ShiftModifier)) {
            e->accept();
            return;
        }
        handleHomeKey(e == QKeySequence::SelectStartOfBlock);
        e->accept();
    } else if (e == QKeySequence::MoveToStartOfLine
               || e == QKeySequence::SelectStartOfLine){
        if ((e->modifiers() & (Qt::AltModifier | Qt::ShiftModifier)) == (Qt::AltModifier | Qt::ShiftModifier)) {
            e->accept();
            return;
        }
        QTextCursor cursor = textCursor();
        if (QTextLayout *layout = cursor.block().layout()) {
            if (layout->lineForTextPosition(cursor.position() - cursor.block().position()).lineNumber() == 0) {
                handleHomeKey(e == QKeySequence::SelectStartOfLine);
                e->accept();
                return;
            }
        }
    } else {
        switch (e->key()) {
        case Qt::Key_Tab:
        case Qt::Key_Backtab: {
            if (ro) break;
            QTextCursor cursor = textCursor();
            indentText(cursor, e->key() == Qt::Key_Tab);
            e->accept();
            return;
        }
        }
    }
    QPlainTextEdit::keyPressEvent(e);
}

void LiteEditorWidgetBase::indentBlock(QTextBlock block, bool bIndent)
{
    QTextCursor cursor(block);
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.removeSelectedText();
    if (bIndent) {
        cursor.insertText("\t");
    } else {
        QString text = block.text();
        if (!text.isEmpty() && (text.at(0) == '\t' || text.at(0) == ' ')) {
            cursor.deleteChar();
        }
    }
    cursor.endEditBlock();
}

void LiteEditorWidgetBase::indentCursor(QTextCursor cur, bool bIndent)
{
    if (bIndent) {
        cur.insertText("\t");
    } else {
        QString text = cur.block().text();
        int pos = cur.position()-cur.block().position()-1;
        int count = text.count();
        if (count > 0 && pos >= 0 && pos < count) {
            if (text.at(pos) == '\t' || text.at(pos) == ' ') {
                cur.deletePreviousChar();
            }
        }
    }
}

void LiteEditorWidgetBase::indentText(QTextCursor cur,bool bIndent)
{
    QTextDocument *doc = document();
    cur.beginEditBlock();
    if (!cur.hasSelection()) {
        indentCursor(cur,bIndent);
    } else {
        QTextBlock block = doc->findBlock(cur.selectionStart());
        QTextBlock end = doc->findBlock(cur.selectionEnd());
        if (!cur.atBlockStart()) {
            end = end.next();
        }
        do {
            indentBlock(block,bIndent);
            block = block.next();
        } while (block.isValid() && block != end);
    }
    cur.endEditBlock();
}

void LiteEditorWidgetBase::indentEnter(QTextCursor cur)
{
    cur.beginEditBlock();
    int pos = cur.position()-cur.block().position();
    QString text = cur.block().text();
    int i = 0;
    int tab = 0;
    int space = 0;
    QString inText = "\n";
    while (i < text.size()) {
        if (!text.at(i).isSpace())
            break;
        if (text.at(0) == ' ') {
            space++;
        }
        else if (text.at(0) == '\t') {
            inText += "\t";
            tab++;
        }
        i++;
    }
    text.trimmed();
    if (!text.isEmpty()) {
        if (pos >= text.size()) {
            const QChar ch = text.at(text.size()-1);
            if (ch == '{' || ch == '(') {
                inText += "\t";
            }
        } else if (pos == text.size()-1 && text.size() >= 3) {
            const QChar l = text.at(text.size()-2);
            const QChar r = text.at(text.size()-1);
            if ( (l == '{' && r == '}') ||
                 (l == '(' && r== ')') ) {
                cur.insertText(inText);
                int pos = cur.position();
                cur.insertText(inText);
                cur.setPosition(pos);
                this->setTextCursor(cur);
                cur.insertText("\t");
                cur.endEditBlock();
                return;
            }
        }
    }
    cur.insertText(inText);
    cur.endEditBlock();
    ensureCursorVisible();
}

void LiteEditorWidgetBase::showTip(const QString &tip)
{
    QRect rc = cursorRect();
    QPoint pt = mapToGlobal(rc.topRight());
    QToolTip::showText(pt,tip,this);
}

void LiteEditorWidgetBase::hideTip()
{
    QToolTip::hideText();
}

