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
// $Id: liteapi.h,v 1.0 2011-4-22 visualfc Exp $

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
};

class IFile : public QObject
{
    Q_OBJECT
public:
    IFile(QObject *parent = 0) : QObject(parent) {}
    virtual ~IFile() { }

    virtual bool open(const QString &fileName, const QString &mimeType) = 0;
    virtual bool reload(bool externalModify) = 0;
    virtual bool save(const QString &fileName) = 0;
    virtual QString fileName() const = 0;
    virtual QString mimeType() const = 0;
signals:
    void aboutToReload();
    void reloaded();
};

class IFileFactory : public QObject
{
    Q_OBJECT
public:
    IFileFactory(QObject *parent = 0) : QObject(parent) {}
    virtual QStringList mimeTypes() const = 0;
    virtual IFile *open(const QString &fileName, const QString &mimeType) = 0;
    virtual QString target(const QString &fileName, const QString &mimetype) const = 0;
};

class IFileFactoryList
{
public:
    virtual IFile *createFile(const QString &fileName, const QString &mimeType) = 0;
    virtual void addFactory(IFileFactory *factory) = 0;
    virtual void removeFactory(IFileFactory *factory) = 0;
    virtual QList<IFileFactory*> factoryList() const = 0;
    virtual QStringList mimeTypeList() const = 0;
};

class IFileManager : public IManager
{
    Q_OBJECT
public:
    IFileManager(QObject *parent = 0) : IManager(parent) {}

    virtual bool openFile(const QString &fileName) = 0;
    virtual bool openEditor(const QString &fileName) = 0;
    virtual bool openProject(const QString &fileName) = 0;
    // recent files
    virtual void addRecentFile(const QString &fileName) = 0;
    virtual void addRecentProject(const QString &fileName) = 0;
    virtual void removeRecentFile(const QString &fileName) = 0;
    virtual void removeRecentProject(const QString &fileName) = 0;
    virtual QStringList recentFiles() const = 0;
    virtual QStringList recentProjects() const = 0;

    virtual QString getFileTarget(const QString &fileName) const = 0;
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
    virtual QString displayName() const = 0;
    virtual QIcon icon() const { return QIcon(); }
};

class IEditor : public IView
{
    Q_OBJECT
public:
    virtual bool open(const QString &fileName,const QString &mimeType) = 0;
    virtual bool save() = 0;
    virtual bool isModified() = 0;
    virtual IFile *file() = 0;
signals:
    void modificationChanged(bool);
    void contentsChanged();
};

class ITextEditor : public IEditor
{
    Q_OBJECT
public:
    virtual int line() const = 0;
    virtual int column() const = 0;
    virtual void gotoLine(int line, int column) = 0;
};

class IEditorManager : public IManager, public IFileFactoryList
{
    Q_OBJECT
public:
    IEditorManager(QObject *parent = 0) : IManager(parent) {}
    virtual QWidget *widget() = 0;
    virtual IEditor *currentEditor() const = 0;
    virtual void setCurrentEditor(IEditor *editor) = 0;
    virtual QList<IEditor*> editorList() const = 0;
    virtual void addEditor(IEditor *editor) = 0;
    virtual void addAutoReleaseEditor(IEditor *editor) = 0;
    virtual IEditor *loadEditor(const QString &fileName) = 0;
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

class IProject : public IView
{
    Q_OBJECT
public:
    virtual IFile *file() = 0;
    virtual QStringList fileList() const = 0;
    virtual QString fileNameToFullPath(const QString &fileName) = 0;
    virtual QString target() const = 0;
    virtual QString workPath() const = 0;
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

class IProjectManager : public IManager, public IFileFactoryList
{
    Q_OBJECT
public:
    IProjectManager(QObject *parent = 0) : IManager(parent) {}
    virtual void setCurrentProject(IProject *project) = 0;
    virtual IProject *currentProject() const = 0;
    virtual QList<IEditor*> editorList(IProject *project) const = 0;
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
    virtual QToolBar *toolBar() = 0;
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

class IActionManager : public IManager
{
    Q_OBJECT
public:
    IActionManager(QObject *parent = 0) : IManager(parent) {}
    virtual QMenu *addMenu(const QString &id, const QString &title) = 0;
    virtual QMenu *loadMenu(const QString &id, const QString &defTitle = QString()) = 0;
    virtual QList<QMenu*>  menuList() const = 0;
    virtual void addToolBar(QToolBar* toolBar) = 0;
    virtual void removeToolBar(QToolBar* toolBar) = 0;
    virtual void showToolBar(QToolBar* toolBar) = 0;
    virtual void hideToolBar(QToolBar* toolBar) = 0;
};

class IPlugin;
class IApplication : public IObject
{
    Q_OBJECT
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

    virtual QString resourcePath() const = 0;
    virtual QString applicationPath() const = 0;
    virtual QString pluginPath() const = 0;

    virtual QList<IPlugin*> pluginList() const = 0;

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
    void setAnchor(QString anchor) { m_anchor = anchor; }
    void setInfo(QString info) { m_info = info; }
    void setId(QString id) { m_id = id; }
    void setName(QString name) { m_name = name; }
    void setVer(QString ver) { m_ver = ver; }
protected:
    QString m_anchor;
    QString m_info;
    QString m_id;
    QString m_name;
    QString m_ver;
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
    virtual QStringList dependPluginList() {
        return QStringList();
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

