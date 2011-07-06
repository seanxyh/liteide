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
// Module: editorreplacedialog.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-6
// $Id: editorreplacedialog.h,v 1.0 2011-7-6 visualfc Exp $

#ifndef EDITORREPLACEDIALOG_H
#define EDITORREPLACEDIALOG_H

#include <QDialog>
#include <QTextCursor>
#include "liteapi.h"

class QPlainTextEdit;
namespace Ui {
    class EditorReplaceDialog;
}

class EditorReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditorReplaceDialog(LiteApi::IApplication *app, QWidget *parent = 0);
    ~EditorReplaceDialog();
    void setFindText(const QString &text);
protected slots:
    void findNext();
    void findPrev();
    void replace();
    void replaceAll();
protected:
    bool replaceFind();
    QTextCursor findEditor(QPlainTextEdit *ed, const QTextCursor &cursor, bool findBackward);
private:
    LiteApi::IApplication *m_liteApp;
    Ui::EditorReplaceDialog *ui;
    QTextCursor m_find;
};

#endif // EDITORREPLACEDIALOG_H
