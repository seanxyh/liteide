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

#include "liteapi/liteapi.h"

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
    virtual void execFileWizard(const QString &projPath, const QString &filePath, const QString &gopath = QString());
    virtual bool openFile(const QString &fileName);
    virtual IEditor *openEditor(const QString &fileName, bool bActive = true);
    virtual IEditor *createEditor(const QString &contents, const QString &mimeType);
    virtual IEditor *createEditor(const QString &fileName);
    virtual IProject *openProject(const QString &fileName);
    virtual IProject *openProjectScheme(const QString &fileName, const QString &scheme);

    virtual QStringList schemeList() const;
    virtual void addRecentFile(const QString &fileName, const QString &scheme);
    virtual void removeRecentFile(const QString &fileName, const QString &scheme);
    virtual QStringList recentFiles(const QString &scheme) const;

    virtual bool findProjectInfo(const QString &fileName, QMap<QString,QString>& projectInfo, QMap<QString,QString>& findProjectInfo) const;
public:
    QString openAllTypeFilter() const;
    QString openProjectTypeFilter() const;
    QString openEditorTypeFilter() const;
protected:
    QString schemeKey(const QString &scheme) const;
    void updateFileState(const QString &fileName);
public slots:
    void updateRecentFileActions(const QString &scheme);
    void openRecentFile();
    void newFile();
    void openFiles();
    void openEditors();
    void openProjects();
    void fileChanged(QString);
    void editorSaved(LiteApi::IEditor*);
    void editorCreated(LiteApi::IEditor*);
    void editorAboutToClose(LiteApi::IEditor*);
    void checkForReload();
protected:
    NewFileDialog        *m_newFileDialog;
    QFileSystemWatcher   *m_fileWatcher;
    QMap<QString,QDateTime> m_fileStateMap;
    QStringList          m_changedFiles;
    bool                 m_checkActivated;
protected:    
    enum { MaxRecentFiles = 8 };
    QMenu       *m_recentMenu;
    QString      m_initPath;
};

#endif // FILEMANAGER_H
