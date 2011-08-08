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
// Module: actionmanager.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: actionmanager.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include "liteapi/liteapi.h"

using namespace LiteApi;

class ActionManager : public IActionManager
{
    Q_OBJECT
public:
    virtual QMenu *addMenu(const QString &id, const QString &title);
    virtual QMenu *loadMenu(const QString &id, const QString &title);
    virtual QList<QMenu*>  menuList() const;
    virtual void addToolBar(QToolBar* toolBar);
    virtual void removeToolBar(QToolBar* toolBar);
    virtual void showToolBar(QToolBar* toolBar);
    virtual void hideToolBar(QToolBar* toolBar);
protected:
    QMap<QString,QMenu*>   m_idMenuMap;
};

#endif // ACTIONMANAGER_H
