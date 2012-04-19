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
// Module: liteapi.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: liteapi.h,v 1.0 2011-7-27 visualfc Exp $

#ifndef __LITEAPI_H__
#define __LITEAPI_H__

#include "liteobj.h"

#include <QWidget>
#include <QSettings>
#include <QMainWindow>
#include <QDockWidget>
#include <QFlags>

namespace LiteApi {

class IApplication;
class IManager;
class IFile;
class IProject;
class IEditor;
/*
valueForKey

EDITORPATH
EDITORNAME
EDITORDIR

PROJECTPATH
PROJECTNAME
PROJECTDIR

WORKDIR

TARGETPATH
TARGETNAME
TARGETDIR
*/

struct TargetInfo
{
    QString workDir;
    QString targetPath;
    QString targetName;
    QString targetDir;
};

struct ProjectInfo
{
    QString projectPath;
    QString projectName;
    QString projectDir;
};

struct EditorInfo
{
    QString editorPath;
    QString editorName;
    QString editorDir;
};

class IManager : public QObject
{
    Q_OBJECT
public:
    IManager(QObject *parent = 0) : QObject(parent) {}
    virtual ~IManager() {}
    virtual bool initWithApp(IApplication *app) {
        m_liteApp = app;
        return true;
    }
protected:
    IApplication *m_liteApp;
};

class IMimeType
{
public:
    virtual ~IMimeType() {}

    virtual QString type() const = 0;
    virtual QString comment() const = 0;
    virtual QString codec() const = 0;
    virtual QStringList globPatterns() const = 0;
    virtual QStringList subClassesOf() const = 0;
    virtual void merge(const IMimeType *mimeType) = 0;
};

class IMimeTypeManager : public IManager
{
    Q_OBJECT
public:
    IMimeTypeManager(QObject *parent = 0) : IManager(parent) {}
    virtual bool addMimeType(IMimeType *mimeType) = 0;
    virtual void removeMimeType(IMimeType *mimeType) = 0;
    virtual QList<IMimeType*> mimeTypeList() const= 0;
    virtual IMimeType *findMimeType(const QString &type) const = 0;
    virtual QString findFileMimeType(const QString &fileName) const = 0;
    virtual QStringList findAllFilesByMimeType(const QString &dir, const QString &type, int deep = 0) const = 0;
};

class IFile : public QObject
{
    Q_OBJECT
public:
    IFile(QObject *parent = 0) : QObject(parent) {}
    virtual ~IFile() { }
    virtual bool open(const QString &filePath, const QString &mimeType) = 0;
    virtual bool reload() = 0;
    virtual bool save(const QString &filePath) = 0;
    virtual bool isReadOnly() const = 0;
    virtual QString filePath() const = 0;
    virtual QString mimeType() const = 0;
};

class IEditorFactory : public QObject
{
    Q_OBJECT
public:
    IEditorFactory(QObject *parent = 0) : QObject(parent) {}
    virtual QStringList mimeTypes() const = 0;
    virtual IEditor *open(const QString &fileName, const QString &mimeType) = 0;
    virtual IEditor *create(const QString &contents, const QString &mimeType) = 0;
};

class IProjectFactory : public QObject
{
    Q_OBJECT
public:
    IProjectFactory(QObject *parent = 0) : QObject(parent) {}
    virtual QStringList mimeTypes() const = 0;
    virtual IProject *open(const QString &fileName, const QString &mimeType) = 0;
    virtual bool findProjectInfo(const QString &fileName, const QString &mimetype, QMap<QString,QString>& projectInfo, QMap<QString,QString>& targetInfo) const = 0;
};

class IFileManager : public IManager
{
    Q_OBJECT
public:
    IFileManager(QObject *parent = 0) : IManager(parent) {}

    virtual void execFileWizard(const QString &projPath, const QString &filePath) = 0;
    virtual bool openFile(const QString &fileName) = 0;
    virtual IEditor *openEditor(const QString &fileName, bool bActive = true) = 0;
    virtual IEditor *createEditor(const QString &contents, const QString &_mimeType) = 0;
    virtual IEditor *createEditor(const QString &fileName) = 0;
    virtual IProject *openProject(const QString &fileName) = 0;
    // recent files
    virtual void addRecentFile(const QString &fileName) = 0;
    virtual void addRecentProject(const QString &fileName) = 0;
    virtual void removeRecentFile(const QString &fileName) = 0;
    virtual void removeRecentProject(const QString &fileName) = 0;
    virtual QStringList recentFiles() const = 0;
    virtual QStringList recentProjects() const = 0;

    virtual bool findProjectInfo(const QString &fileName, QMap<QString,QString>& projectInfo, QMap<QString,QString>& findProjectInfo) const = 0;
signals:
    void fileListChanged();
    void recentProjectsChanged();
    void recentFilesChanged();
public slots:
    virtual void newFile() = 0;
    virtual void openFiles() = 0;
    virtual void openEditors() = 0;
    virtual void openProjects() = 0;
};

class IView : public IObject
{
    Q_OBJECT
public:
    IView(QObject *parent = 0) : IObject(parent) {}
    virtual QWidget *widget() = 0;
    virtual QString name() const = 0;
    virtual QIcon icon() const { return QIcon(); }
};

class IEditor : public IView
{
    Q_OBJECT
public:
    IEditor(QObject *parent = 0) : IView(parent) {}
    virtual bool open(const QString &filePath,const QString &mimeType) = 0;
    virtual bool reload() = 0;
    virtual bool save() = 0;
    virtual bool saveAs(const QString &filePath) = 0;
    virtual void setReadOnly(bool b) = 0;
    virtual bool isReadOnly() const = 0;
    virtual bool isModified() const = 0;
    virtual QString filePath() const = 0;
    virtual QString mimeType() const = 0;
    virtual QMap<QString,QString> editorInfo() const = 0;
    virtual QMap<QString,QString> targetInfo() const = 0;
signals:
    void modificationChanged(bool);
    void contentsChanged();
    void reloaded();
};

class ITextEditor : public IEditor
{
    Q_OBJECT
public:
    ITextEditor(QObject *parent = 0) : IEditor(parent) {}
    virtual int line() const = 0;
    virtual int column() const = 0;
    virtual void gotoLine(int line, int column, bool center = false) = 0;
};

class IEditorManager : public IManager
{
    Q_OBJECT
public:
    IEditorManager(QObject *parent = 0) : IManager(parent) {}
    virtual IEditor *openEditor(const QString &fileName, const QString &mimeType) = 0;
    virtual void addFactory(IEditorFactory *factory) = 0;
    virtual void removeFactory(IEditorFactory *factory) = 0;
    virtual QList<IEditorFactory*> factoryList() const = 0;
    virtual QStringList mimeTypeList() const = 0;
    virtual QWidget *widget() = 0;
    virtual IEditor *currentEditor() const = 0;
    virtual void setCurrentEditor(IEditor *editor) = 0;
    virtual IEditor *findEditor(const QString &fileName, bool canonical) const = 0;
    virtual QList<IEditor*> editorList() const = 0;
    virtual QAction *registerBrowser(IEditor *editor) = 0;
    virtual void activeBrowser(IEditor *editor) = 0;
public slots:
    virtual bool saveEditor(IEditor *editor = 0) = 0;
    virtual bool saveEditorAs(IEditor *editor = 0) = 0;
    virtual bool saveAllEditors() = 0;
    virtual bool closeEditor(IEditor *editor = 0) = 0;
    virtual bool closeAllEditors(bool autoSaveAll = false) = 0;
signals:
    void currentEditorChanged(LiteApi::IEditor *editor);
    void editorCreated(LiteApi::IEditor *editor);
    void editorAboutToClose(LiteApi::IEditor *editor);
    void editorSaved(LiteApi::IEditor *editor);
};

class IBrowserEditor : public IEditor
{
    Q_OBJECT
public:
    IBrowserEditor(QObject *parent = 0)  : IEditor(parent) {}
    virtual bool open(const QString &/*fileName*/,const QString &/*mimeType*/) { return false; }
    virtual bool reload() { return false; }
    virtual bool save() { return false; }
    virtual bool saveAs(const QString &/*fileName*/){ return false; }
    virtual void setReadOnly(bool /*b*/) {}
    virtual bool isReadOnly() const { return true; }
    virtual bool isModified() const { return false; }
    virtual QString filePath() const { return QString(); }
    virtual QMap<QString,QString> editorInfo() const { return QMap<QString,QString>(); }
    virtual QMap<QString,QString> targetInfo() const { return QMap<QString,QString>(); }
};

class IProject : public IView
{
    Q_OBJECT
public:
    virtual QString filePath() const = 0;
    virtual QString mimeType() const = 0;
    virtual QStringList fileNameList() const = 0;
    virtual QStringList filePathList() const = 0;
    virtual QString fileNameToFullPath(const QString &filePath) = 0;
    virtual QMap<QString,QString> projectInfo() const = 0;
    virtual QMap<QString,QString> targetInfo() const = 0;
    virtual void load() = 0;
signals:
    void reloaded();
};

class IOption : public IView
{
    Q_OBJECT
public:
    IOption(QObject *parent = 0) : IView(parent) {}
    virtual QString mimeType() const = 0;
    virtual void apply() = 0;
};

class IOptionFactory : public QObject
{
    Q_OBJECT
public:
    IOptionFactory(QObject *parent = 0) : QObject(parent) {}
    virtual QStringList mimeTypes() const = 0;
    virtual IOption *create(const QString &mimeType) = 0;
};

class IOptionManager : public IManager
{
    Q_OBJECT
public:
    IOptionManager(QObject *parent = 0) : IManager(parent) {}
    virtual void addFactory(IOptionFactory *factory) = 0;
    virtual void removeFactory(IOptionFactory *factory) = 0;
    virtual QList<IOptionFactory*> factoryList() const = 0;
public slots:
    virtual void exec() = 0;
signals:
    void applyOption(QString);
};

class IProjectManager : public IManager
{
    Q_OBJECT
public:
    IProjectManager(QObject *parent = 0) : IManager(parent) {}
    virtual IProject *openProject(const QString &fileName, const QString &mimeType) = 0;
    virtual void addFactory(IProjectFactory *factory) = 0;
    virtual void removeFactory(IProjectFactory *factory) = 0;
    virtual QList<IProjectFactory*> factoryList() const = 0;
    virtual QStringList mimeTypeList() const = 0;
    virtual void setCurrentProject(IProject *project) = 0;
    virtual IProject *currentProject() const = 0;
    virtual QList<IEditor*> editorList(IProject *project) const = 0;
    virtual void addImportAction(QAction *act) = 0;
    virtual QWidget *widget() = 0;
public slots:
    virtual void saveProject(IProject *project = 0) = 0;
    virtual void closeProject(IProject *project = 0) = 0;
signals:
    void currentProjectChanged(LiteApi::IProject *project);
    void projectAboutToClose(LiteApi::IProject *project);
};


class IDockManager : public IManager
{
    Q_OBJECT
public:
    IDockManager(QObject *parent = 0) : IManager(parent) {}
    virtual QWidget *widget() = 0;
    virtual QDockWidget *addDock(QWidget *widget,
                         const QString &title,
                         Qt::DockWidgetArea ares = Qt::LeftDockWidgetArea,
                         Qt::DockWidgetAreas alowedAreas = Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea,
                         QDockWidget::DockWidgetFeatures  features = QDockWidget::AllDockWidgetFeatures) = 0;
    virtual void removeDock(QWidget *widget) = 0;
    virtual void showDock(QWidget *widget) = 0;
    virtual void hideDock(QWidget *widget) = 0;
    virtual QDockWidget *dockWidget(QWidget *widget) = 0;
};

class IOutputManager : public IManager
{
    Q_OBJECT
public:
    IOutputManager(QObject *parent = 0) : IManager(parent) {}
    virtual QWidget *widget() = 0;
    virtual QStatusBar *statusBar() = 0;
    virtual void addOutuput(QWidget *w, const QString &label) = 0;
    virtual void addOutuput(QWidget *w, const QIcon &icon, const QString &label) = 0;
    virtual void showOutput(QWidget *w) = 0;
    virtual void hideOutput(QWidget *w) = 0;
    virtual void removeOutput(QWidget *w) = 0;
    virtual QWidget *currentOutput() = 0;
public slots:
    virtual void setCurrentOutput(QWidget *w = 0) = 0;
signals:
    void currentOutputChanged(QWidget *w);
};

enum VIEWMENU_ACTION_POS
{
    ViewMenuToolBarPos = 1,
    ViewMenuPanePos,
    ViewMenuPagePos
};

class IActionManager : public IManager
{
    Q_OBJECT
public:
    IActionManager(QObject *parent = 0) : IManager(parent) {}
    virtual QMenu *insertMenu(const QString &id, const QString &title, const QString &idBefore = QString()) = 0;
    virtual QMenu *loadMenu(const QString &id) = 0;
    virtual void removeMenu(QMenu *menu) = 0;
    virtual QList<QString>  menuList() const = 0;
    virtual QToolBar *insertToolBar(const QString &id, const QString &title, const QString &before = QString()) = 0;
    virtual QToolBar *loadToolBar(const QString &id) = 0;
    virtual void removeToolBar(QToolBar* toolBar) = 0;
    virtual QList<QString> toolBarList() const = 0;
    virtual void insertViewMenu(VIEWMENU_ACTION_POS pos, QAction *act) = 0;
};

class IPlugin;
class IApplication : public IObject
{
    Q_OBJECT
signals:
    void loaded();
public:
    virtual ~IApplication() {}
    virtual IProjectManager *projectManager() = 0;
    virtual IEditorManager  *editorManager() = 0;
    virtual IFileManager    *fileManager() = 0;
    virtual IActionManager  *actionManager() = 0;
    virtual IDockManager    *dockManager() = 0;
    virtual IOutputManager  *outputManager() = 0;
    virtual IMimeTypeManager *mimeTypeManager() = 0;
    virtual IOptionManager  *optionManager() = 0;

    virtual QMainWindow *mainWindow() const = 0;
    virtual QSettings *settings() = 0;
    virtual QMap<QString,QVariant> &cookie() = 0;

    virtual QString resourcePath() const = 0;
    virtual QString applicationPath() const = 0;
    virtual QString pluginPath() const = 0;

    virtual QList<IPlugin*> pluginList() const = 0;

    virtual void loadSession(const QString &name) = 0;
    virtual void saveSession(const QString &name) = 0;
    virtual void loadState() = 0;
    virtual void saveState() = 0;

    virtual void appendConsole(const QString &model, const QString &action, const QString &log = QString()) = 0;
};

class PluginInfo
{
public:
    virtual QString anchor() const { return m_anchor; }
    virtual QString info() const { return m_info; }
    virtual QString id() const { return m_id; }
    virtual QString name() const { return m_name; }
    virtual QString ver() const { return m_ver; }
    virtual QStringList dependList() const { return m_dependList; }
    void setAnchor(QString anchor) { m_anchor = anchor; }
    void setInfo(QString info) { m_info = info; }
    void setId(QString id) { m_id = id.toLower(); }
    void setName(QString name) { m_name = name; }
    void setVer(QString ver) { m_ver = ver; }
    void setDependList(const QStringList &dependList) { m_dependList = dependList; }
    void appendDepend(const QString &depend) { m_dependList.append(depend); }
protected:
    QString m_anchor;
    QString m_info;
    QString m_id;
    QString m_name;
    QString m_ver;
    QStringList m_dependList;
};

class IPlugin : public IObject
{
    Q_OBJECT
public:
    IPlugin() : m_info(new PluginInfo)
    {
        m_info->setVer("1.0");
    }
    virtual ~IPlugin()
    {
        delete m_info;
    }

    virtual bool initWithApp(IApplication *app) {
        m_liteApp = app;
        return true;
    }
    virtual QString id() const {
        return m_info->id();
    }
    virtual QString name() const {
        return m_info->name();
    }
    virtual QStringList dependPluginList() const{
        return m_info->dependList();
    }
    virtual const PluginInfo *info() const {
        return m_info;
    }
protected:
    PluginInfo   *m_info;
    IApplication *m_liteApp;
};

} //namespace LiteApi

Q_DECLARE_INTERFACE(LiteApi::IPlugin,"LiteApi.IPlugin/1.0")


#endif //__LITEAPI_H__

