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
#include <QPlainTextEdit>

class QTextEdit;
class QToolBar;
class QLabel;
class TerminalEdit;
class QVBoxLayout;

class Output : public QWidget
{
    Q_OBJECT
public:
    explicit Output(QWidget *parent = 0);
    void setCenter(QWidget *widget);
signals:
    void hideOutput();
    void clearRequest();
protected:
    QVBoxLayout  *m_mainLayout;
    QToolBar      *m_toolBar;
    QToolButton   *m_closeBtn;
    QLabel        *m_infoLabel;
    QAction       *m_clearAct;
    QAction       *m_hideAct;
};

class TextOutput : public Output
{
    Q_OBJECT
public:
    explicit TextOutput(bool readOnly = true, QWidget *parent = 0);
    void append(const QString &text);
    void append(const QString &text,const QBrush &foreground);
    void appendTag0(const QString &text);
    void appendTag1(const QString &text);
    void updateExistsTextColor(const QBrush &foreground = Qt::gray);
    void setReadOnly(bool bo);
    void setMaxLine(int max);
signals:
    void dbclickEvent(QTextCursor);
    void enterText(QString);
public slots:
    void clear();
protected:
    TerminalEdit  *m_editor;
    QTextCharFormat m_fmt;
};

#endif // TEXTOUTPUT_H
