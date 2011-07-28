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
// Module: editorreplacedialog.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-6
// $Id: editorreplacedialog.cpp,v 1.0 2011-7-6 visualfc Exp $


#include "editorreplacedialog.h"
#include "ui_editorreplacedialog.h"
#include "liteapi/litefindobj.h"

#include <QPlainTextEdit>
#include <QTextDocument>
#include <QTextCursor>
#include <QRegExp>
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

EditorReplaceDialog::EditorReplaceDialog(LiteApi::IApplication *app,QWidget *parent) :
    QDialog(parent),
    m_liteApp(app),
    ui(new Ui::EditorReplaceDialog)
{
    ui->setupUi(this);
    ui->infoLineEdit->setReadOnly(true);

    m_liteApp->settings()->beginGroup("codesearch_replace");
    ui->matchWordCheckBox->setChecked(m_liteApp->settings()->value("matchword",false).toBool());
    ui->matchCaseCheckBox->setChecked(m_liteApp->settings()->value("matchcase",false).toBool());
    ui->useRegexCheckBox->setChecked(m_liteApp->settings()->value("useregex",false).toBool());
    ui->wrapAroundCheckBox->setChecked(m_liteApp->settings()->value("wraparound",true).toBool());
    m_liteApp->settings()->endGroup();


    connect(ui->closePushButton,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->findNextPushButton,SIGNAL(clicked()),this,SLOT(findNext()));
    connect(ui->findPrevPushButton,SIGNAL(clicked()),this,SLOT(findPrev()));
    connect(ui->replacePushButton,SIGNAL(clicked()),this,SLOT(replace()));
    connect(ui->replaceAllPushButton,SIGNAL(clicked()),this,SLOT(replaceAll()));
}

EditorReplaceDialog::~EditorReplaceDialog()
{
    m_liteApp->settings()->beginGroup("codesearch_replace");
    m_liteApp->settings()->setValue("matchword",ui->matchWordCheckBox->isChecked());
    m_liteApp->settings()->setValue("matchcase",ui->matchCaseCheckBox->isChecked());
    m_liteApp->settings()->setValue("useregex",ui->useRegexCheckBox->isChecked());
    m_liteApp->settings()->setValue("wraparound",ui->wrapAroundCheckBox->isChecked());
    m_liteApp->settings()->endGroup();

    delete ui;
}

void EditorReplaceDialog::setFindText(const QString &text)
{
    ui->findComboBox->setEditText(text);
}

void EditorReplaceDialog::replaceAll()
{
    QString text = ui->findComboBox->currentText();
    if (text.isEmpty()) {
        return;
    }
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
    if (!ed) {
        return;
    }

    do {
        replaceFind();
        findNext();
    } while (!m_find.isNull());
}

void EditorReplaceDialog::replace()
{
    if (replaceFind()) {
        findNext();
    }
}

bool EditorReplaceDialog::replaceFind()
{
    if (m_find.isNull()) {
        return false;
    }

    QString text = m_find.selectedText();
    QString text1 = text;
    QString text2;
    if (ui->useRegexCheckBox) {
        text2 = text.replace(QRegExp(ui->findComboBox->currentText()),ui->replaceComboBox->currentText());
    } else {
        text2 = text.replace(ui->findComboBox->currentText(),ui->replaceComboBox->currentText());
    }
    m_find.removeSelectedText();
    m_find.insertText(text);

    ui->infoLineEdit->setText(QString("%1 => %2").arg(text1).arg(text2));

    return true;
}

void EditorReplaceDialog::findPrev()
{
    QString text = ui->findComboBox->currentText();
    if (text.isEmpty()) {
        return;
    }
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
    if (!ed) {
        return;
    }

    m_find = findEditor(ed,ed->textCursor(),true);
    if (m_find.isNull() && ui->wrapAroundCheckBox->isChecked()) {
        ed->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
        m_find = findEditor(ed,ed->textCursor(),true);
    }
    if (!m_find.isNull()) {
        ed->setTextCursor(m_find);
    }
}

void EditorReplaceDialog::findNext()
{
    QString text = ui->findComboBox->currentText();
    if (text.isEmpty()) {
        return;
    }
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
    if (!ed) {
        return;
    }
    m_find = findEditor(ed,ed->textCursor(),false);
    if (m_find.isNull() && ui->wrapAroundCheckBox->isChecked()) {
        ed->moveCursor(QTextCursor::Start,QTextCursor::MoveAnchor);
        m_find = findEditor(ed,ed->textCursor(),false);
    }
    if (!m_find.isNull()) {
        ed->setTextCursor(m_find);
    }
}

QTextCursor EditorReplaceDialog::findEditor(QPlainTextEdit *ed, const QTextCursor &cursor, bool findBackward)
{
    QString text = ui->findComboBox->currentText().trimmed();
    QTextDocument::FindFlags flags = 0;
    if (findBackward) {
        flags |= QTextDocument::FindBackward;
    }
    if (ui->matchCaseCheckBox->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (ui->matchWordCheckBox->isChecked()) {
        flags |= QTextDocument::FindWholeWords;
    }
    QTextCursor find;
    if (ui->useRegexCheckBox->isChecked()) {
        find = ed->document()->find(QRegExp(text),cursor,flags);
    } else {
        find = ed->document()->find(text,cursor,flags);
    }
    return find;
}
