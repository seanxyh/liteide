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
// Module: sqlitemdelegate.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-25
// $Id: sqlitemdelegate.cpp,v 1.0 2011-7-25 visualfc Exp $

#include "sqlitemdelegate.h"
#include <QDebug>

SqlItemDelegate::SqlItemDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget *SqlItemDelegate::createEditor(QWidget *parent,
                      const QStyleOptionViewItem &option,
                      const QModelIndex &index) const
{
    QVariant var = index.data(Qt::EditRole);
    if (var.type() == QVariant::ByteArray) {
        return 0;
    }
    return QItemDelegate::createEditor(parent,option,index);
}
