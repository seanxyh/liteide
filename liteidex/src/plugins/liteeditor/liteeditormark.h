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
// Module: liteeditormark.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: liteeditormark.h,v 1.0 2011-8-12 visualfc Exp $

#ifndef LITEEDITORMARK_H
#define LITEEDITORMARK_H

#include "liteapi/liteapi.h"
#include "liteeditorapi/liteeditorapi.h"

class LiteEditorMarkTypeManager : public LiteApi::IEditorMarkTypeManager
{
    Q_OBJECT
public:
    LiteEditorMarkTypeManager(QObject *parent = 0);
    virtual void registerMark(int type, const QIcon &icon);
    virtual QList<int> markTypeList() const;
    virtual QIcon markIcon(int type) const;
protected:
    QMap<int,QIcon> m_typeIconMap;
};

typedef QMap<int,QList<int> > MarkTypesMap;

class LiteEditorMark : public LiteApi::IEditorMark
{
    Q_OBJECT
public:
    explicit LiteEditorMark(LiteApi::IEditorMarkTypeManager *manager, QObject *parent = 0);
    virtual void addMark(int line, int type);
    virtual void removeMark(int line, int type);
    virtual QList<int> markLineList() const;
    virtual QList<int> lineTypeList(int line) const;
    virtual void paint(QPainter *painter, int blockNumber, int x, int y, int w, int h) const;
protected:
    LiteApi::IEditorMarkTypeManager *m_manager;
    MarkTypesMap m_lineMarkTypesMap;
};

Q_DECLARE_METATYPE(MarkTypesMap)

#endif // LITEEDITORMARK_H
