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
// Module: filemanager.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: filemanager.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "../liteapi/liteapi.h"

using namespace LiteApi;

class NewFileDialog;
class QFileSystemWatcher;
struct FileStateItem;

class FileManager : public IFileManager
{
    Q_OBJECT
public:
    FileManager();
    virtual bool initWithApp(IApplication *app);
    virtual ~FileManager();
public:
    virtual bool addFile(IFile *file);
    virtual bool removeFile(IFile *file);

    virtual bool openFile(const QString &fileName);
    virtual bool openEditor(const QString &fileName);
    virtual bool openProject(const QString &fileName);

    virtual void addRecentFile(const QString &fileName);
    virtual void addRecentProject(const QString &fileName);
    virtual void removeRecentFile(const QString &fileName);
    virtual void removeRecentProject(const QString &fileName);
    virtual QStringList recentFiles() const;
    virtual QStringList recentProjects() const;
public:
    QMenu *recentFileMenu() const;
    QMenu *recentProjectMenu() const;
    QString openAllTypeFilter() const;
    QString openProjectTypeFilter() const;
    QString openEditorTypeFilter() const;
protected:
    void updateRecentFileActions();
    void updateRecentProjectActions();
    void updateFileState(IFile *file);
public slots:
    void newFile();
    void openFiles();
    void openEditors();
    void openProjects();
    void openRecentFile();
    void openRecentProject();
    void clearRecentFiles();
    void clearRecentProjects();
    void fileChanged(QString);
    void editorSaved(LiteApi::IEditor*);
    void editorAboutToClose(LiteApi::IEditor*);
    void checkForReload();
protected:
    QList<IFile*>        m_files;
    QStringList          m_recentFiles;
    NewFileDialog        *m_newFileDialog;
    QFileSystemWatcher   *m_fileWatcher;
    QMap<QString,FileStateItem> m_fileStateMap;
    QStringList          m_changedFiles;
    bool                 m_checkActivated;
protected:    
    enum { MaxRecentFiles = 16 };
    enum { MaxRecentProjects = 16 };
    QAction     *m_clearRecentFilesAct;
    QAction     *m_clearRecentProjectsAct;
    QAction     *m_recentFileActs[MaxRecentFiles];
    QAction     *m_recentProjectActs[MaxRecentProjects];
    QMenu       *m_recentFileMenu;
    QMenu       *m_recentProjectMenu;
    QString      m_initPath;
};

#endif // FILEMANAGER_H
