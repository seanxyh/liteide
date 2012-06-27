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
// Module: liteeditormark.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: liteeditormark.cpp,v 1.0 2011-8-12 visualfc Exp $

#include "liteeditormark.h"
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


LiteEditorMarkTypeManager::LiteEditorMarkTypeManager(QObject *parent) :
    LiteApi::IEditorMarkTypeManager(parent)
{
}

void LiteEditorMarkTypeManager::registerMark(int type, const QIcon &icon)
{
    m_typeIconMap.insert(type,icon);
}

QList<int> LiteEditorMarkTypeManager::markTypeList() const
{
    return m_typeIconMap.keys();
}

QIcon LiteEditorMarkTypeManager::markIcon(int type) const
{
    return m_typeIconMap.value(type);
}

LiteEditorMark::LiteEditorMark(LiteApi::IEditorMarkTypeManager *manager, QObject *parent) :
    LiteApi::IEditorMark(parent),
    m_manager(manager)
{
}
/*
void LiteEditorMark::paint(QPainter *painter, const QRect &rect) const
{
    m_icon.paint(painter, rect, Qt::AlignCenter);
}
*/

void LiteEditorMark::addMark(int line, int type)
{
    QMap<int, QList<int> >::iterator it = m_lineMarkTypesMap.find(line);
    if (it == m_lineMarkTypesMap.end()) {
        m_lineMarkTypesMap.insert(line,QList<int>() << type);
        emit markChanged();
    } else {
        if (!it.value().contains(type)) {
            it.value().append(type);
            qSort(it.value());
            emit markChanged();
        }
    }
}

void LiteEditorMark::removeMark(int line, int type)
{
    QMap<int, QList<int> >::iterator it = m_lineMarkTypesMap.find(line);
    if (it != m_lineMarkTypesMap.end()) {
        it.value().removeOne(type);
        emit markChanged();
    }
}

QList<int> LiteEditorMark::markLineList() const
{
    return m_lineMarkTypesMap.keys();
}

QList<int> LiteEditorMark::lineTypeList(int line) const
{
    return m_lineMarkTypesMap.value(line);
}

void LiteEditorMark::paint(QPainter *painter, int blockNumber, int x, int y, int w, int h) const
{
    const int line = blockNumber + 1;
    QMap<int, QList<int> >::const_iterator it = m_lineMarkTypesMap.find(line);
    if (it != m_lineMarkTypesMap.end()) {
        int offset = x;
        foreach(int type, it.value()) {
            const QIcon &icon = m_manager->markIcon(type);
            icon.paint(painter,offset,y,w,h);
            offset += 2;
        }
    }
}
