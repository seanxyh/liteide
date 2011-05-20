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
// Module: textoutput.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: textoutput.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef TEXTOUTPUT_H
#define TEXTOUTPUT_H

#include <QWidget>
#include <QTextEdit>
#include <QToolButton>

class QTextEdit;
class QToolBar;
class QLabel;

class TextOutEdit : public QTextEdit
{
    Q_OBJECT
public:
    TextOutEdit(QWidget *parent = 0);
protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);
signals:
    void dbclickEvent();    
    void enterText(QString);
protected:
    QString m_inputText;
};

class TextOutput : public QWidget
{
    Q_OBJECT
public:
    explicit TextOutput(bool readOnly = true, QWidget *parent = 0);
    void append(const QString &text);
    void append(const QString &text,const QBrush &foreground);
    void appendTag0(const QString &text);
    void appendTag1(const QString &text);
    void updateExistsTextColor(const QBrush &foreground = Qt::gray);
    QTextEdit *textEdit();
signals:
    void dbclickEvent();
    void enterText(QString);
    void hideOutput();
public slots:
    void clear();
protected:
    QToolBar      *m_toolBar;
    QToolButton   *m_closeBtn;
    TextOutEdit   *m_editor;
    QLabel        *m_space;
    QAction       *m_clearAct;
    QAction       *m_hideAct;
    QTextCharFormat m_fmt;
};

#endif // TEXTOUTPUT_H
