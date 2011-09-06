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
// Module: buildconfigdialog.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: buildconfigdialog.cpp,v 1.0 2011-8-12 visualfc Exp $

#include "buildconfigdialog.h"
#include "ui_buildconfigdialog.h"

#include <QAbstractItemModel>
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

BuildConfigDialog::BuildConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BuildConfigDialog)
{
    ui->setupUi(this);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->verticalHeader()->hide();
    connect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(editTabView(QModelIndex)));
}

BuildConfigDialog::~BuildConfigDialog()
{
    delete ui;
}

void BuildConfigDialog::editTabView(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    if (index.column() == 1) {
        ui->tableView->edit(index);
    }
}

void BuildConfigDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    resizeModel();
}

void BuildConfigDialog::resizeModel()
{
    QAbstractItemModel *model = ui->tableView->model();
    if (!model) {
        return;
    }
    ui->tableView->resizeColumnsToContents();

    int total = 0;
    for (int i = 0; i < model->columnCount(); ++i) {
        total += ui->tableView->columnWidth(i);
    }
    int width = ui->tableView->viewport()->width()-2;
    int count = model->columnCount();
    if (total < width && count == 2)
    {
        int extra = (width - total)/4;
        ui->tableView->setColumnWidth(0, ui->tableView->columnWidth(0) + extra);
        ui->tableView->setColumnWidth(1, ui->tableView->columnWidth(1) + extra*3);
    }
}

void BuildConfigDialog::setModel(QAbstractItemModel * model)
{
    ui->tableView->setModel(model);
}
