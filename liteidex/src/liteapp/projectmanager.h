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
// Module: projectmanager.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: projectmanager.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include "liteapi/liteapi.h"

#include <QPointer>

using namespace LiteApi;

class QComboBox;
class QStackedWidget;
class QStandardItem;
class QStandardItemModel;
class QVBoxLayout;
class QComboBox;
class QStackedLayout;
class QActionGroup;
class QScrollArea;

class ProjectManager : public IProjectManager
{
    Q_OBJECT
public:
    ProjectManager();
    ~ProjectManager();
    virtual bool initWithApp(IApplication *app);
    virtual IProject *openProject(const QString &fileName, const QString &mimeType);
    virtual void addFactory(IProjectFactory *factory);
    virtual void removeFactory(IProjectFactory *factory);
    virtual QList<IProjectFactory*> factoryList() const;
    virtual QStringList mimeTypeList() const;
public:
    virtual void setCurrentProject(IProject *project);
    virtual IProject *currentProject() const;
    virtual QList<IEditor*> editorList(IProject *project) const;
    virtual void addImportAction(QAction *act);
    QWidget *widget();
public slots:    
    virtual void saveProject(IProject *project = 0);
    virtual void closeProject(IProject *project = 0);
    void currentEditorChanged(LiteApi::IEditor*);
    void triggeredProject(QAction* act);
protected:
    virtual void closeProjectHelper(IProject *project);
protected:
    QMap<QString,QAction*>   m_mapNameToAction;
    QPointer<IProject>      m_currentProject;
    QList<IProjectFactory*>    m_factoryList;
    QWidget                 *m_widget;
    QScrollArea             *m_scrollArea;
    QMenu                   *m_projectMenu;
    QActionGroup            *m_projectActGroup;
    QMenu                   *m_importMenu;
};

#endif // PROJECTMANAGER_H
