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
// Module: build.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: build.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef BUILD_H
#define BUILD_H

#include "litebuildapi/litebuildapi.h"
#include <QProcessEnvironment>

using namespace LiteApi;
class Build : public LiteApi::IBuild
{
public:
    Build(QObject *parent = 0);
    ~Build();
    virtual QString mimeType() const;
    virtual QString id() const;
    virtual QList<BuildAction*> actionList() const;
    virtual QList<BuildLookup*> lookupList() const;
    virtual BuildAction *findAction(const QString &id);
    virtual QStringList envIdList() const;
    virtual QString findEnvFile(const QString &id);
    virtual void setCurrentEnvId(const QString &id);
    virtual QString currentEnvId();
    virtual QProcessEnvironment currentEnv() const;
    virtual QString actionCommand(BuildAction *act,QMap<QString,QString> &liteEnv);
    virtual QString actionArgs(BuildAction *act,QMap<QString,QString> &liteEnv);
public:
    void setType(const QString &mimeType);
    void setId(const QString &id);
    void appendAction(BuildAction *act);
    void appendEnv(const QString &id,const QString &envFile);
public:
    static bool loadBuild(LiteApi::IBuildManager *manager, const QString &fileName);
    static bool loadBuild(LiteApi::IBuildManager *manager, QIODevice *dev, const QString &fileName);
protected:
    QString m_mimeType;
    QString m_id;
    QList<BuildAction*> m_actionList;
    QList<BuildLookup*> m_lookupList;
    QStringList         m_envIdList;
    QMap<QString,QString>     m_idEnvFileMap;
    QString m_buildEnvId;
    QProcessEnvironment m_buildEnv;
    QMap<BuildAction*,QString> m_actionCmdMap;
    QMap<BuildAction*,QString> m_actionArgMap;
    QMap<QString,QString>   m_liteEnv;
};

#endif // BUILD_H
