/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011-2012 LiteIDE Team. All rights reserved.
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
// Module: astwidget.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-3
// $Id: astwidget.h,v 1.0 2012-4-26 visualfc Exp $

#ifndef ASTWIDGET_H
#define ASTWIDGET_H

#include "liteapi/liteapi.h"
#include "symboltreeview/symboltreeview.h"
#include "qtc_editutil/filterlineedit.h"

class GolangAstItem;
class QStandardItemModel;
class QSortFilterProxyModel;

class AstWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AstWidget(bool outline, LiteApi::IApplication *app, QWidget *parent = 0);
public:
    void clear();
    void updateModel(const QByteArray &data);
    void setWorkPath(const QString &path) {
        m_workPath = path;
    }
    QString workPath() const {
        return m_workPath;
    }
    GolangAstItem *astItemFromIndex(QModelIndex index);
    SymbolTreeView *tree() { return m_tree; }
public slots:
    bool filterModel(QString filter, QModelIndex parent, QModelIndex &first);
    void clearFilter(QModelIndex parent);
    void filterChanged(QString);
signals:
    void doubleClicked(QModelIndex);
protected:
    bool                m_bOutline;
    bool                m_bFirst;
    SymbolTreeView     *m_tree;
    Utils::FilterLineEdit *m_filterEdit;
    QStandardItemModel *m_model;
    QSortFilterProxyModel *proxyModel;
    LiteApi::IApplication *m_liteApp;
    QString m_workPath;
};

#endif // ASTWIDGET_H
