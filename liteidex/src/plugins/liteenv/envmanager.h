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
// Module: envmanager.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-25
// $Id: envmanager.h,v 1.0 2011-7-25 visualfc Exp $

#ifndef ENVMANAGER_H
#define ENVMANAGER_H

#include "liteenvapi/liteenvapi.h"

class QComboBox;

class Env : public LiteApi::IEnv
{
    Q_OBJECT
public:
    Env(QObject *parent = 0);
public:
    virtual QString id() const;
    virtual QProcessEnvironment env() const;
    static void loadEnv(LiteApi::IEnvManager *manager, const QString &filePath);
protected:
    QString m_id;
    QProcessEnvironment m_env;
};

class EnvManager : public LiteApi::IEnvManager
{
    Q_OBJECT
public:
    EnvManager(QObject *parent = 0);   
    ~EnvManager();
public:
    virtual bool initWithApp(LiteApi::IApplication *app);
    virtual void addEnv(LiteApi::IEnv *build);
    virtual void removeEnv(LiteApi::IEnv *build);
    virtual LiteApi::IEnv *findEnv(const QString &id) const;
    virtual QList<LiteApi::IEnv*> envList() const;
    virtual void setCurrentEnv(LiteApi::IEnv *env);
    virtual LiteApi::IEnv *currentEnv() const;
    virtual QProcessEnvironment currentEnvironment() const;
protected slots:
    void envActivated(QString);
public:
    void loadEnvFiles(const QString &path);
protected:
    QList<LiteApi::IEnv*>    m_envList;
    LiteApi::IEnv           *m_curEnv;
    QToolBar        *m_toolBar;
    QComboBox       *m_envCmb;
};

#endif // ENVMANAGER_H