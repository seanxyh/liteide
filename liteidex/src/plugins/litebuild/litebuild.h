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
// Module: litebuild.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: litebuild.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef LITEBUILD_H
#define LITEBUILD_H

#include "liteapi.h"
#include "litebuildapi/litebuildapi.h"

class BuildManager;
class QComboBox;
class ProcessEx;
class LiteOutput;
class LiteBuild : public QObject
{
    Q_OBJECT
public:
    explicit LiteBuild(LiteApi::IApplication *app, QObject *parent = 0);
    virtual ~LiteBuild();
    void setCurrentBuild(LiteApi::IBuild *build);
    void resetProjectEnv(LiteApi::IProject *project);
public slots:
    void currentProjectChanged(LiteApi::IProject*);
    void reloadProject();
    void currentEditorChanged(LiteApi::IEditor*);
    void buildAction();
    void execAction(const QString &id);
    void extOutput(const QByteArray &output,bool bError);
    void extFinish(bool error,int exitCode, QString msg);
    void stopAction();
    void dbclickBuildOutput();
    void enterTextBuildOutput(QString);
    void envIndexChanged(QString);
protected:
    LiteApi::IApplication   *m_liteApp;
    BuildManager    *m_manager;
    LiteApi::IBuild *m_build;
    QToolBar    *m_toolBar;
    QComboBox   *m_envCombBox;
    QAction     *m_envAct;
    QList<QAction*> m_actions;
    QMap<QString,QString> m_liteEnv;
    ProcessEx *m_process;
    LiteOutput *m_output;
    QString     m_outputRegex;
};

#endif // LITEBUILD_H
