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
// Module: astwidget.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-3
// $Id: astwidget.cpp,v 1.0 2011-8-3 visualfc Exp $

#include "astwidget.h"
#include "golangastitem.h"
#include "golangasticon.h"

#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QFont>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

AstWidget::AstWidget(LiteApi::IApplication *app, QWidget *parent) :
    SymbolTreeView(parent),
    m_liteApp(app)
{
    m_model = new QStandardItemModel(this);
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSourceModel(m_model);

    this->setModel(proxyModel);
    this->setExpandsOnDoubleClick(false);
}

void AstWidget::clear()
{
    m_model->clear();
}

static QStringList stringListFromIndex(const QModelIndex &index)
{
    QStringList list;
    if (!index.isValid())
        return list;
    list.append(stringListFromIndex(index.parent()));
    list.append(index.data().toString());
    return list;
}

static QModelIndex indexFromStringList(QSortFilterProxyModel *model, QStringList &list, const QModelIndex & parent = QModelIndex())
{
    if (list.isEmpty())
        return QModelIndex();
    QString text = list.front();
    for (int i = 0; i < model->rowCount(parent); i++) {
        QModelIndex child = model->index(i,0,parent);
        if (child.data().toString() == text) {
            list.pop_front();
            if (list.isEmpty()) {
                return child;
            } else {
                QModelIndex next = indexFromStringList(model,list,child);
                if (next.isValid())
                    return next;
                else
                    return child;
            }
        }
    }
    return QModelIndex();
}

GolangAstItem *AstWidget::astItemFromIndex(QModelIndex index)
{
    QModelIndex i = proxyModel->mapToSource(index);
    if (!i.isValid()) {
        return 0;
    }
    return (GolangAstItem*)m_model->itemFromIndex(i);
}

static QString tagName(const QString &tag)
{
    /*
    tools/goastview/packageview.go
    const (
            tag_package      = "p"
            tag_type         = "t"
            tag_struct       = "s"
            tag_interface    = "i"
            tag_value        = "v"
            tag_const        = "c"
            tag_func         = "f"
            tag_value_folder = "+v"
            tag_const_folder = "+c"
            tag_func_folder  = "+f"
            tag_type_method  = "tm"
            tag_type_factor  = "tf"
            tag_type_value   = "tv"
    )
    */
    if (tag == "p") {
        return "package";
    } else if (tag == "t") {
        return "type";
    } else if (tag == "s") {
        return "struct";
    } else if (tag == "i") {
        return "interface";
    } else if (tag == "v") {
        return "value";
    } else if (tag == "c") {
        return "const";
    } else if (tag == "f") {
        return "func";
    } else if (tag == "+v") {
        return "value folder";
    } else if (tag == "+c") {
        return "const folder";
    } else if (tag == "+f") {
        return "func folder";
    } else if (tag == "tm") {
        return "method";
    } else if (tag == "tf") {
        return "factory";
    } else if (tag == "tv") {
        return "field";
    }
    return QString();
}

// level,tag,name,index,x,y
void AstWidget::updateModel(const QByteArray &data)
{
    //save state
    QList<QStringList> expands;
    QSetIterator<QModelIndex> i(this->expandIndexs());
    while (i.hasNext()) {
        QStringList path = stringListFromIndex(i.next());
        expands.append(path);
    }
    QStringList topState = stringListFromIndex(this->topViewIndex());
    QStringList curState = stringListFromIndex(this->currentIndex());

    m_model->clear();

    QList<QByteArray> array = data.split('\n');
    QMap<int,QStandardItem*> items;
    QStringList indexFiles;
    bool ok = false;
    bool bmain = false;
    QMap<QString,GolangAstItem*> level1NameItemMap;
    foreach (QByteArray line, array) {
        QList<QByteArray> info = line.split(',');
        if (info.size() == 2 && info.at(0) == "@") {
            indexFiles.append(info.at(1));
        }
        if (info.size() < 3) {
            continue;
        }
        int level = info[0].toInt(&ok);
        if (!ok) {
            continue;
        }
        QString tag = info[1];
        QString name = info[2];
        if (name.isEmpty() || tag.isEmpty()) {
            continue;
        }
        if (level == 0) {
            level1NameItemMap.clear();
        }
        if (tag == "p") {
            if (name == "main") {
                bmain = true;
            } else {
                bmain = false;
            }
        }
        GolangAstItem *item = 0;
        if (level == 1) {
            item = level1NameItemMap.value(name);
            if (item != 0) {
                items[level] = item;
                continue;
            }
        }
        item = new GolangAstItem;
        if (level == 1) {
            level1NameItemMap.insert(name,item);
        }
        item->setText(name);
        if (!bmain && (name.at(0).isLower() || name.at(0) == '_')) {
            item->setIcon(GolangAstIcon::instance()->iconFromTag(tag,false));
        } else {
            item->setIcon(GolangAstIcon::instance()->iconFromTag(tag));
        }
        if (tag.at(0) == '+') {
            item->setToolTip(QString("%1").arg(tagName(tag)));
        } else {
            item->setToolTip(QString("%1 : %2").arg(tagName(tag)).arg(name));
        }
        if (info.size() >= 6) {
            int index = info[3].toInt(&ok);
            if (ok && index >= 0 && index < indexFiles.size()) {
                item->setFileName(indexFiles.at(index));
            }
            int line = info[4].toInt(&ok);
            if (ok) {
                item->setLine(line);
            }
            int col = info[5].toInt(&ok);
            if (ok) {
                item->setCol(col);
            }
        }
        QStandardItem *parent = items.value(level-1,0);
        if (parent ) {
            parent->appendRow(item);
        } else {
            m_model->appendRow(item);
        }
        items[level] = item;
    }

    //load state
    this->expandToDepth(0);

    QListIterator<QStringList> ie(expands);
    while (ie.hasNext()) {
        QStringList expandPath = ie.next();
        QModelIndex expandIndex = indexFromStringList(proxyModel,expandPath);
        if (expandIndex.isValid()) {
            this->setExpanded(expandIndex,true);
        }
    }

    QModelIndex curIndex = indexFromStringList(proxyModel,curState);
    if (curIndex.isValid()) {
        this->setCurrentIndex(curIndex);
    }

    QModelIndex topIndex = indexFromStringList(proxyModel,topState);
    if (topIndex.isValid()) {
        this->scrollTo(topIndex, QTreeView::PositionAtTop);
    }
}
