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
// Module: editorsearchdialog.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-5
// $Id: editorsearchdialog.h,v 1.0 2011-7-6 visualfc Exp $


#ifndef EDITORSEARCHDIALOG_H
#define EDITORSEARCHDIALOG_H

#include <QDialog>
#include <QTextCursor>
#include "liteapi.h"

class QPlainTextEdit;

namespace Ui {
    class EditorSearchDialog;
}

class EditorSearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditorSearchDialog(LiteApi::IApplication *app, QWidget *parent = 0);
    ~EditorSearchDialog();
    void setFindText(const QString &text);
public slots:
    void findNext();
    void findPrev();
protected slots:
    void onFindNext();
    void onFindPrev();
protected:
    QTextCursor findEditor(QPlainTextEdit*,const QTextCursor &cursor,bool findBackward);
private:
    Ui::EditorSearchDialog *ui;
    LiteApi::IApplication *m_liteApp;
};

#endif // EDITORSEARCHDIALOG_H
