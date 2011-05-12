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
// Module: browsereditor.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: browsereditor.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "browsereditor.h"

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

BrowserEditor::BrowserEditor()
    : m_widget(0)
{
}

QWidget *BrowserEditor::widget()
{
    return m_widget;
}

QString BrowserEditor::displayName() const
{
    return m_displayName;
}

QIcon BrowserEditor::icon() const
{
    return m_icon;
}

void BrowserEditor::setWidget(QWidget *w)
{
    m_widget = w;
}

void BrowserEditor::setDisplayName(const QString &name)
{
    m_displayName = name;
}

void BrowserEditor::setIcon(const QIcon &icon)
{
    m_icon = icon;
}
