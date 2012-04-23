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
// Module: packagetree.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-4-19
// $Id: packagetree.cpp,v 1.0 2012-4-20 visualfc Exp $

#include "packagetree.h"

#include <QStandardItem>
#include <QStandardItemModel>
#include <QDir>
#include <QFileInfo>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

PackageTree::PackageTree(QWidget *parent) :
    SymbolTreeView(parent),
    root(0)
{
    model = new QStandardItemModel(this);
    this->setModel(model);
}

void PackageTree::loadJson(const QMap<QString, QVariant> &json)
{
    if (root == 0) {
        root = new QStandardItem(json.value("ImportPath").toString());
        src = new QStandardItem("Source");
        imports = new QStandardItem("Imports");
        deps = new QStandardItem("Deps");
        model->appendRow(root);
        root->appendRow(src);
        this->expandAll();
        root->appendRow(imports);
        root->appendRow(deps);
    }
    root->setText(json.value("ImportPath").toString());
    src->removeRows(0,src->rowCount());
    imports->removeRows(0,imports->rowCount());
    deps->removeRows(0,deps->rowCount());

    fileList.clear();
    nameList.clear();

    QDir dir(json.value("Dir").toString());
    QStringList nameFilter;
    nameFilter << "*.go" << "*.h" << "*.c" << "*.cpp" << "*.s";
    foreach(QFileInfo info, dir.entryInfoList(nameFilter,QDir::Files,QDir::Type|QDir::Name)) {
        QStandardItem *item = new QStandardItem(info.fileName());
        item->setData(ITEM_SOURCE,RoleItem);
        item->setData(info.filePath(),RolePath);
        fileList.append(info.filePath());
        nameList.append(info.fileName());
        src->appendRow(item);
    }
    foreach(QVariant import , json.value("Imports").toList()) {
        QStandardItem *item = new QStandardItem(import.toString());
        item->setData(ITEM_IMPORT,RoleItem);
        imports->appendRow(item);
    }
    foreach(QVariant dep , json.value("Deps").toList()) {
        QStandardItem *item = new QStandardItem(dep.toString());
        item->setData(ITEM_DEP,RoleItem);
        deps->appendRow(item);
    }
}
