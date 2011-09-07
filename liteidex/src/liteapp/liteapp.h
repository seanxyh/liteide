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
// Module: liteapp.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: liteapp.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef LITEAPP_H
#define LITEAPP_H

#include "liteapi/liteapi.h"
#include "textoutput/textoutput.h"
#include "extension/extension.h"

using namespace LiteApi;

class MainWindow;
class PluginManager;
class EditorManager;
class FileManager;
class ProjectManager;
class DockManager;
class OutputManager;
class ActionManager;
class MimeTypeManager;
class OptionManager;
class QSettings;
class QSplitter;
class LiteAppOptionFactory;

class LiteApp : public IApplication
{
    Q_OBJECT
public:
    LiteApp();
    virtual ~LiteApp();
    virtual IExtension *extension();
    virtual IProjectManager *projectManager();
    virtual IEditorManager  *editorManager();
    virtual IFileManager    *fileManager();
    virtual IActionManager  *actionManager();
    virtual IDockManager    *dockManager();
    virtual IOutputManager  *outputManager();
    virtual IMimeTypeManager *mimeTypeManager();
    virtual IOptionManager  *optionManager();

    virtual QMainWindow *mainWindow() const;
    virtual QSettings *settings();

    virtual QString resourcePath() const;
    virtual QString applicationPath() const;
    virtual QString pluginPath() const;

    virtual QList<IPlugin*> pluginList() const;

    virtual void loadSession(const QString &name);
    virtual void saveSession(const QString &name);
    virtual void loadState();
    virtual void saveState();

    virtual void appendConsole(const QString &model, const QString &action, const QString &log = QString());
public:
    void load();
    void createActions();
    void createMenus();
    void createToolBars();
    void loadPlugins();
    void loadMimeType();
    void initPlugins();    
    void setPluginPath(const QString &path); 
    void setResourcePath(const QString &path);
protected slots:
    void currentProjectChanged(LiteApi::IProject *project);
    void currentEditorChanged(LiteApi::IEditor *editor);
    void editorModifyChanged(bool);
    void cleanup();
protected:
    QSettings       *m_settings;
    Extension     *m_extension;
    MainWindow      *m_mainwindow;
    ActionManager  *m_actionManager;
    PluginManager   *m_pluginManager;
    ProjectManager *m_projectManager;
    EditorManager   *m_editorManager;
    FileManager    *m_fileManager;
    DockManager    *m_dockManager;
    OutputManager  *m_outputManager;
    MimeTypeManager *m_mimeTypeManager;
    OptionManager   *m_optionManager;
    TextOutput    *m_logOutput;
    LiteAppOptionFactory *m_liteAppOptionFactory;
    QString         m_pluginPath;
    QString         m_resourcePath;
protected:
    QAction     *m_newAct;
    QAction     *m_openAct;
    QAction     *m_closeAct;
    QAction     *m_closeAllAct;
    QAction     *m_openProjectAct;
    QAction     *m_saveProjectAct;
    QAction     *m_closeProjectAct;
    QAction     *m_saveAct;
    QAction     *m_saveAsAct;
    QAction     *m_saveAllAct;
    QAction     *m_exitAct;
    QAction     *m_optionAct;
    QAction     *m_aboutAct;
    QAction     *m_aboutPluginsAct;
    QToolBar    *m_stdToolBar;
    QToolBar    *m_navToolBar;
    QMenu       *m_fileMenu;
    QMenu       *m_viewMenu;
    //QMenu       *m_toolMenu;
    QMenu       *m_helpMenu;    
};

#endif // LITEAPP_H
