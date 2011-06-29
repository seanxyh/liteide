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
// Module: liteeditorwidgetbase.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: liteeditorwidgetbase.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef LITEEDITORWIDGETBASE_H
#define LITEEDITORWIDGETBASE_H

#include <QPlainTextEdit>

class LiteEditorWidgetBase : public QPlainTextEdit
{
    Q_OBJECT
public:
    LiteEditorWidgetBase(QWidget *parent = 0);
    void initLoadDocument();
    void setTabWidth(int n);
public:
    int extraAreaWidth();
    void extraAreaPaintEvent(QPaintEvent *e);
    void extraAreaMouseEvent(QMouseEvent *e);
    void extraAreaLeaveEvent(QEvent *e);
    void resizeEvent(QResizeEvent *e);
    void showTip(const QString &tip);
    void hideTip();
protected slots:
    virtual void highlightCurrentLine();
    virtual void slotUpdateExtraAreaWidth();
    virtual void slotModificationChanged(bool);
    virtual void slotUpdateRequest(const QRect &r, int dy);
    virtual void slotCursorPositionChanged();
    virtual void slotUpdateBlockNotify(const QTextBlock &);
    QChar characterAt(int pos) const;
    void handleHomeKey(bool anchor);
public slots:
    void gotoLine(int line, int column);
    void gotoLineStart();
    void gotoLineStartWithSelection();
    void gotoLineEnd();
    void gotoLineEndWithSelection();
public:
    void keyPressEvent(QKeyEvent *e);
    void indentBlock(QTextBlock block, bool bIndent);
    void indentCursor(QTextCursor cur, bool bIndent);
    void indentText(QTextCursor cur, bool bIndent);
    void indentEnter(QTextCursor cur);
    QWidget *m_extraArea;
    bool m_lineNumbersVisible;
    bool m_marksVisible;
    bool m_autoIndent;
    int m_lastSaveRevision;
    int m_extraAreaSelectionNumber;
};

#endif // LITEEDITORWIDGETBASE_H
