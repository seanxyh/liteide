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
// Module: liteeditoroption.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: liteeditoroption.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "liteeditoroption.h"
#include "ui_liteeditoroption.h"
#include <QFontDatabase>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

LiteEditorOption::LiteEditorOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::LiteEditorOption)
{
    ui->setupUi(m_widget);
    QFontDatabase db;
    const QStringList families = db.families();
    ui->familyComboBox->addItems(families);

    m_fontFamily = m_liteApp->settings()->value("editor/family","Courier").toString();
    m_fontSize = m_liteApp->settings()->value("editor/fontsize",12).toInt();

    const int idx = families.indexOf(m_fontFamily);
    ui->familyComboBox->setCurrentIndex(idx);

    updatePointSizes();
}

QWidget *LiteEditorOption::widget()
{
    return m_widget;
}

QString LiteEditorOption::displayName() const
{
    return "LiteEditor";
}

QString LiteEditorOption::mimeType() const
{
    return "option/liteeditor";
}

void LiteEditorOption::apply()
{
    m_fontFamily = ui->familyComboBox->currentText();
    if (ui->sizeComboBox->count()) {
        const QString curSize = ui->sizeComboBox->currentText();
        bool ok = true;
        int size = curSize.toInt(&ok);
        if (ok) {
            m_fontSize = size;
        }
    }
    m_liteApp->settings()->setValue("editor/family",m_fontFamily);
    m_liteApp->settings()->setValue("editor/fontsize",m_fontSize);
}

LiteEditorOption::~LiteEditorOption()
{
    delete m_widget;
    delete ui;
}

void LiteEditorOption::updatePointSizes()
{
    // Update point sizes
    const int oldSize = m_fontSize;
    if (ui->sizeComboBox->count()) {
        const QString curSize = ui->sizeComboBox->currentText();
        bool ok = true;
        int oldSize = curSize.toInt(&ok);
        if (!ok)
            oldSize = m_fontSize;
        ui->sizeComboBox->clear();
    }
    const QList<int> sizeLst = pointSizesForSelectedFont();
    int idx = 0;
    int i = 0;
    for (; i < sizeLst.count(); ++i) {
        if (idx == 0 && sizeLst.at(i) >= oldSize)
            idx = i;
        ui->sizeComboBox->addItem(QString::number(sizeLst.at(i)));
    }
    if (ui->sizeComboBox->count())
        ui->sizeComboBox->setCurrentIndex(idx);
}

QList<int> LiteEditorOption::pointSizesForSelectedFont() const
{
    QFontDatabase db;
    const QString familyName = ui->familyComboBox->currentText();
    QList<int> sizeLst = db.pointSizes(familyName);
    if (!sizeLst.isEmpty())
        return sizeLst;

    QStringList styles = db.styles(familyName);
    if (!styles.isEmpty())
        sizeLst = db.pointSizes(familyName, styles.first());
    if (sizeLst.isEmpty())
        sizeLst = QFontDatabase::standardSizes();

    return sizeLst;
}
