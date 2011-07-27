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
// Module: liteappoption.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-6-27
// $Id: liteappoption.cpp,v 1.0 2011-6-27 visualfc Exp $

#include "liteappoption.h"
#include "ui_liteappoption.h"
#include <QDir>
#include <QFileInfo>
#include <QLocale>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

LiteAppOption::LiteAppOption(LiteApi::IApplication *app,QObject *parent) :
    LiteApi::IOption(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::LiteAppOption)
{
    ui->setupUi(m_widget);
    const QString &liteideTrPath = m_liteApp->resourcePath()+"/translations";
    ui->langComboBox->addItem(QLocale::languageToString(QLocale::English),QLocale::English);
    QDir dir(liteideTrPath);
    if (dir.exists()) {
        foreach (QFileInfo info,dir.entryInfoList(QStringList() << "liteide_*.qm")) {
            QString base = info.baseName();
            QString lc = base.right(base.length()-8);
            QLocale::Language lang = QLocale(lc).language();
            ui->langComboBox->addItem(QLocale::languageToString(lang),lang);
        }
    }
    QString locale = QLocale::system().name();
    locale = m_liteApp->settings()->value("General/Language",locale).toString();
    if (!locale.isEmpty()) {
        QLocale::Language lang = QLocale(locale).language();
        for (int i = 0; i < ui->langComboBox->count(); i++) {
            if (lang == ui->langComboBox->itemData(i).toInt()) {
                ui->langComboBox->setCurrentIndex(i);
                break;
            }
        }
    }
}

LiteAppOption::~LiteAppOption()
{
    delete m_widget;
    delete ui;
}

QWidget *LiteAppOption::widget()
{
    return m_widget;
}

QString LiteAppOption::name() const
{
    return "LiteApp";
}

QString LiteAppOption::mimeType() const
{
    return "option/liteapp";
}
void LiteAppOption::apply()
{
    int index = ui->langComboBox->currentIndex();
    if (index < 0) {
        return;
    }
    QLocale::Language lc = (QLocale::Language)ui->langComboBox->itemData(index).toInt();
    m_liteApp->settings()->setValue("General/Language",QLocale(lc).name());

}
