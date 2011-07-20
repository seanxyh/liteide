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
// Module: documentbrowser.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-7
// $Id: documentbrowser.cpp,v 1.0 2011-7-7 visualfc Exp $

#include "documentbrowser.h"

#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>

DocumentBrowser::DocumentBrowser(LiteApi::IApplication *app) :
    m_liteApp(app)
{
    m_widget = new QWidget;

    m_browser = new QTextBrowser;

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);

    mainLayout->addWidget(m_browser);
    m_widget->setLayout(mainLayout);    
}

DocumentBrowser::~DocumentBrowser()
{
    if (m_widget) {
        delete m_widget;
    }
}

QWidget *DocumentBrowser::widget()
{
    return m_widget;
}

QString DocumentBrowser::displayName() const
{
    return tr("DocumentBrowser");
}

QIcon DocumentBrowser::icon() const
{
    return QIcon();
}

QTextBrowser *DocumentBrowser::browser()
{
    return m_browser;
}
