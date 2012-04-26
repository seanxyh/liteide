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
// Module: litecompleter.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: litecompleter.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "litecompleter.h"
#include "treemodelcompleter/treemodelcompleter.h"

#include <QCompleter>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QAbstractItemView>
#include <QScrollBar>
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


class WordItem : public QStandardItem
{
public:
    WordItem(const QString &text) : QStandardItem(text)
    {
        this->setName(text);
    }
    enum {
        NameRole = Qt::UserRole+2,
        KindRole,
        InfoRole,
        TempRole
    };
    virtual QVariant data(int role = Qt::UserRole + 1) const
    {
        if (role == Qt::DisplayRole) {
            if (this->info().isEmpty()) {
                return this->name();
            }
            return QString("%1\t%2").arg(this->name()).arg(this->info());
        }
        return QStandardItem::data(role);
    }
    void setName(const QString &word)
    {
        this->setData(word,NameRole);
    }
    QString name() const
    {
        return QStandardItem::data(NameRole).toString();
    }
    void setKind(const QString &kind)
    {
        this->setData(kind,KindRole);
    }
    QString kind() const
    {
        return QStandardItem::data(KindRole).toString();
    }
    void setInfo(const QString &info)
    {
        this->setData(info,InfoRole);
    }
    QString info() const
    {
        return QStandardItem::data(InfoRole).toString();
    }
    void setTemp(bool temp)
    {
        this->setData(temp,TempRole);
    }
    bool isTemp() const
    {
        return QStandardItem::data(TempRole).toBool();
    }
};

LiteCompleter::LiteCompleter(QObject *parent) :
    LiteApi::ICompleter(parent),
    m_completer( new TreeModelCompleter(this)),
    m_model(new QStandardItemModel(this))
{
    m_completer->setModel(m_model);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setCaseSensitivity(Qt::CaseSensitive);
    m_completer->setSeparator(".");
    m_stop = '(';
    QObject::connect(m_completer, SIGNAL(activated(QModelIndex)),
                     this, SLOT(insertCompletion(QModelIndex)));
}

LiteCompleter::~LiteCompleter()
{
    delete m_completer;
    delete m_model;
}

void LiteCompleter::setEditor(QPlainTextEdit *editor)
{
    m_editor = editor;
    m_completer->setWidget(m_editor);
}

QCompleter *LiteCompleter::completer() const
{
    return m_completer;
}

void LiteCompleter::clear()
{
    m_model->clear();
}

void LiteCompleter::clearTemp()
{
    int i = m_model->rowCount();
    while (i--) {
        QModelIndex index = m_model->index(i,0);
        if (index.data(WordItem::TempRole).toBool() == true) {
            m_model->removeRow(i);
        }
    }
}

void LiteCompleter::show()
{
    if (!m_editor) {
        return;
    }
    m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
    QRect cr = m_editor->cursorRect();
    cr.setWidth(m_completer->popup()->sizeHintForColumn(0)
                + m_completer->popup()->verticalScrollBar()->sizeHint().width());
    m_completer->complete(cr); // popup it up!

}

void LiteCompleter::appendItems(QStringList items,const QString &kind, const QString &info,const QIcon &icon, bool temp)
{
    foreach(QString item,items) {
        appendItemEx(item,kind,info,icon,temp);
    }
}


bool LiteCompleter::appendItem(const QString &name, const QIcon &icon, bool temp)
{
    QString func;
    int pos = name.indexOf(m_stop);
    if (pos == -1) {
        func = name.trimmed();
    } else {
        func = name.left(pos).trimmed();
        //arg = text.right(text.length()-pos).trimmed();
    }
    return this->appendItemEx(func,"","",icon,temp);
}

bool LiteCompleter::appendItemEx(const QString &name,const QString &kind, const QString &info, const QIcon &icon, bool temp)
{
    QStringList words = name.split(m_completer->separator(),QString::SkipEmptyParts);
    WordItem *root = 0;
    WordItem *item = 0;
    bool bnew = false;    
    foreach (QString word, words) {
        item = 0;
        QModelIndex parent = m_model->indexFromItem(root);
        for (int i = 0; i < m_model->rowCount(parent); i++) {
            QModelIndex index = m_model->index(i,0,parent);
            if (index.data(WordItem::NameRole).toString() == word) {
                item = static_cast<WordItem*>(m_model->itemFromIndex(index));
                break;
            }
        }
        if (item == 0) {
            item = new WordItem(word);
            if (root == 0) {
                m_model->appendRow(item);
            } else {
                root->appendRow(item);
            }
            bnew = true;
        }
        root = item;
    }
    if (item && item->kind().isEmpty()) {
        item->setKind(kind);
        item->setInfo(info);
        item->setTemp(temp);
        item->setIcon(icon);
    }
    return bnew;
}

void LiteCompleter::completionPrefixChanged(QString prefix)
{
    if (!m_editor) {
        return;
    }
    if (m_completer->widget() != m_editor) {
        return;
    }
    emit prefixChanged(m_editor->textCursor(),prefix);
}

void LiteCompleter::insertCompletion(QModelIndex index)
{   
    if (!m_editor) {
        return;
    }
    if (m_completer->widget() != m_editor)
        return;
    if (!index.isValid()) {
        return;
    }
    QString text = index.data(WordItem::NameRole).toString();
    QString prefix = m_completer->completionPrefix();
    //IsAbs r.URL.
    int pos = prefix.lastIndexOf(m_completer->separator());
    int length = prefix.length();
    if (pos != -1) {
        length = prefix.length()-pos-1;
    }
    QString extra = text.right(text.length()-length);
    QTextCursor tc = m_editor->textCursor();
    tc.insertText(extra);
    m_editor->setTextCursor(tc);
    emit wordCompleted(prefix+extra,index.data(WordItem::InfoRole).toString());
}
