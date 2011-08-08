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
// Module: liteenvapi.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-25
// $Id: liteenvapi.h,v 1.0 2011-7-25 visualfc Exp $

#ifndef __LITEENVAPI_H__
#define __LITEENVAPI_H__

#include "liteapi/liteapi.h"
#include <QProcessEnvironment>

namespace LiteApi {

class IEnv : public QObject
{
    Q_OBJECT
public:
    IEnv(QObject *parent = 0): QObject(parent) {}
    virtual ~IEnv() {}
    virtual QString id() const = 0;
    virtual QProcessEnvironment env() const = 0;
    virtual void reload() = 0;
};

class IEnvManager : public IManager
{
    Q_OBJECT
public:
    IEnvManager(QObject *parent = 0) : IManager(parent) {}
    virtual QList<IEnv*> envList() const = 0;
    virtual IEnv *findEnv(const QString &id) const = 0;
    virtual void setCurrentEnvId(const QString &id) = 0;
    virtual IEnv *currentEnv() const = 0;
    virtual QProcessEnvironment currentEnvironment() const = 0;
signals:
    void currentEnvChanged(LiteApi::IEnv*);
};

} //namespace LiteApi


#endif //__LITEENVAPI_H__

