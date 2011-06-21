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
// Module: filemanager.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: filemanager.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "filemanager.h"
#include "newfiledialog.h"
#include "fileutil/fileutil.h"

#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QFileInfo>
#include <QRegExp>
#include <QMenu>
#include <QMessageBox>
#include <QFileInfo>
#include <QDateTime>
#include <QFileSystemWatcher>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end


struct FileStateItem
{
    QDateTime modified;
    IFile     *file;
};

bool FileManager::initWithApp(IApplication *app)
{
    if (!IFileManager::initWithApp(app)) {
        return false;
    }

    m_fileWatcher = new QFileSystemWatcher(this);
    connect(m_fileWatcher,SIGNAL(fileChanged(QString)),this,SLOT(fileChanged(QString)));

    m_clearRecentFilesAct = new QAction(tr("Clear All"),this);
    m_clearRecentProjectsAct = new QAction(tr("Clear All"),this);
    m_newFileDialog = 0;

    connect(m_clearRecentFilesAct,SIGNAL(triggered()),this,SLOT(clearRecentFiles()));
    connect(m_clearRecentProjectsAct,SIGNAL(triggered()),this,SLOT(clearRecentProjects()));

    for (int i = 0; i < MaxRecentFiles; ++i) {
        m_recentFileActs[i] = new QAction(this);
        m_recentFileActs[i]->setVisible(false);
        connect(m_recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }
    for (int i = 0; i < MaxRecentProjects; ++i) {
        m_recentProjectActs[i] = new QAction(this);
        m_recentProjectActs[i]->setVisible(false);
        connect(m_recentProjectActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentProject()));
    }

    m_recentFileMenu = new QMenu(tr("Recent Files"),m_liteApp->mainWindow());
    m_recentProjectMenu = new QMenu(tr("Recent Projects"),m_liteApp->mainWindow());

    for (int i = 0; i < MaxRecentFiles; ++i) {
         m_recentFileMenu->addAction(m_recentFileActs[i]);
    }
    m_recentFileMenu->addSeparator();
    m_recentFileMenu->addAction(m_clearRecentFilesAct);
    for (int i = 0; i < MaxRecentProjects; ++i) {
         m_recentProjectMenu->addAction(m_recentProjectActs[i]);
    }
    m_recentProjectMenu->addSeparator();
    m_recentProjectMenu->addAction(m_clearRecentProjectsAct);

    updateRecentFileActions();
    updateRecentProjectActions();

    m_initPath = m_liteApp->settings()->value("FileManager/initpath",QDir::homePath()).toString();
    return true;
}

FileManager::FileManager()
    : m_newFileDialog(0),
      m_checkActivated(false)
{
}

FileManager::~FileManager()
{
    delete m_fileWatcher;
    m_liteApp->settings()->setValue("FileManager/initpath",m_initPath);
    if (m_newFileDialog) {
        delete m_newFileDialog;
    }
}

bool FileManager::addFile(IFile *file)
{
    if (!file) {
        return false;
    }
    foreach (IFile *f, m_files) {
        if (FileUtil::compareFile(f->fileName(),file->fileName()))
            return false;
    }
    m_files.push_back(file);
    if (!file->fileName().isEmpty()) {
        updateFileState(file);
        m_fileWatcher->addPath(file->fileName());
    }
    return true;
}

bool FileManager::removeFile(IFile *file)
{
    if (!file->fileName().isEmpty()) {
        m_fileStateMap.remove(file->fileName());
        m_changedFiles.removeOne(file->fileName());
        m_fileWatcher->removePath(file->fileName());
    }
    return m_files.removeOne(file);
}

QStringList FileManager::recentFiles() const
{
    return m_liteApp->settings()->value("LiteApp/recentFileList").toStringList();;
}

QStringList FileManager::recentProjects() const
{
    return m_liteApp->settings()->value("LiteApp/recentProjectList").toStringList();;
}

QString FileManager::getFileTarget(const QString &fileName) const
{
    QString mimeType = m_liteApp->mimeTypeManager()->findFileMimeType(fileName);
    QList<IFileFactory*> factoryList;
    if (m_liteApp->projectManager()->mimeTypeList().contains(mimeType)) {
        factoryList = m_liteApp->projectManager()->factoryList();
    } else {
        factoryList = m_liteApp->editorManager()->factoryList();
    }
    QString target;
    foreach(LiteApi::IFileFactory *factory, factoryList) {
        if (factory->mimeTypes().contains(mimeType)) {
            target = factory->target(fileName,mimeType);
            if (!target.isEmpty()) {
                break;
            }
        }
    }
    return target;
}


QString FileManager::openAllTypeFilter() const
{
    QStringList types;
    QStringList filter;
    foreach (IMimeType *mimeType, m_liteApp->mimeTypeManager()->mimeTypeList()) {
        types.append(mimeType->globPatterns());
        filter.append(QString("%1 (%2)").arg(mimeType->comment()).arg(mimeType->globPatterns().join(" ")));
    }
    types.removeDuplicates();
    filter.removeDuplicates();
    if (!types.isEmpty()) {
        QString all = QString(tr("All Support File (%1)")).arg(types.join(" "));
        filter.insert(0,all);
    }
    filter.append(tr("All Files (*)"));
    return filter.join(";;");
}

QString FileManager::openProjectTypeFilter() const
{
    QStringList types;
    QStringList filter;
    QStringList projectMimeTypes = m_liteApp->projectManager()->mimeTypeList();
    foreach (IMimeType *mimeType, m_liteApp->mimeTypeManager()->mimeTypeList()) {
        if (projectMimeTypes.contains(mimeType->type())) {
            types.append(mimeType->globPatterns());
            filter.append(QString("%1 (%2)").arg(mimeType->comment()).arg(mimeType->globPatterns().join(" ")));
        }
    }
    types.removeDuplicates();
    filter.removeDuplicates();
    if (!types.isEmpty()) {
        QString all = QString(tr("All Support File (%1)")).arg(types.join(" "));
        filter.insert(0,all);
    }
    filter.append(tr("All Files (*)"));
    return filter.join(";;");
}

QString FileManager::openEditorTypeFilter() const
{
    QStringList types;
    QStringList filter;
    QStringList projectMimeTypes = m_liteApp->editorManager()->mimeTypeList();
    foreach (IMimeType *mimeType, m_liteApp->mimeTypeManager()->mimeTypeList()) {
        if (projectMimeTypes.contains(mimeType->type())) {
            types.append(mimeType->globPatterns());
            filter.append(QString("%1 (%2)").arg(mimeType->comment()).arg(mimeType->globPatterns().join(" ")));
        }
    }
    types.removeDuplicates();
    filter.removeDuplicates();
    if (!types.isEmpty()) {
        QString all = QString(tr("All Support File (%1)")).arg(types.join(" "));
        filter.insert(0,all);
    }
    filter.append(tr("All Files (*)"));
    return filter.join(";;");
}

QMenu *FileManager::recentFileMenu() const
{
    return m_recentFileMenu;
}

QMenu *FileManager::recentProjectMenu() const
{
    return m_recentProjectMenu;
}

void FileManager::newFile()
{
    if (!m_newFileDialog) {
        m_newFileDialog = new NewFileDialog(m_liteApp->mainWindow());
        m_newFileDialog->setTemplatePath(m_liteApp->resourcePath()+"/template");
    }
    QString location = m_initPath;
    IProject *project = m_liteApp->projectManager()->currentProject();
    if (project) {
        location = project->workPath();
    } else {
        IEditor *editor = m_liteApp->editorManager()->currentEditor();
        if (editor) {
            IFile *file = editor->file();
            if (file) {
                location = QFileInfo(file->fileName()).absolutePath();
            }
        }
    }
    QDir dir(location);
    m_newFileDialog->setFileLocation(dir.absolutePath());
    dir.cdUp();
    m_newFileDialog->setProjectLocation(dir.absolutePath());
    m_newFileDialog->updateLocation();

    if (m_newFileDialog->exec() == QDialog::Accepted) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::question(m_liteApp->mainWindow(), tr("LiteIDE"),
                                    tr("Project '%1' is created.\n"
                                       "Do you want to load?")
                                    .arg(m_newFileDialog->openFiles().join(" ")),
                                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                    QMessageBox::Yes);
        if (ret == QMessageBox::Yes) {
            foreach(QString file, m_newFileDialog->openFiles()) {
                this->openFile(file);
            }
        }
    }
}

void FileManager::openFiles()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(m_liteApp->mainWindow(),
           tr("Open Project or File"), m_initPath,openAllTypeFilter());
    if (fileNames.isEmpty())
        return;
    foreach (QString fileName, fileNames) {
        if (openFile(fileName)) {
            m_initPath = QFileInfo(fileName).canonicalPath();
        }
    }
}

void FileManager::openEditors()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(m_liteApp->mainWindow(),
           tr("Open Files"), m_initPath,openEditorTypeFilter());
    if (fileNames.isEmpty())
        return;
    foreach (QString fileName, fileNames) {
        if (openEditor(fileName)) {
            m_initPath = QFileInfo(fileName).canonicalPath();
        }
    }
}

void FileManager::openProjects()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(m_liteApp->mainWindow(),
           tr("Open Project"), m_initPath,openProjectTypeFilter());
    if (fileNames.isEmpty())
        return;
    foreach (QString fileName, fileNames) {
        if (openProject(fileName)) {
            m_initPath = QFileInfo(fileName).canonicalPath();
        }
    }
}

bool FileManager::openFile(const QString &fileName)
{
    QString mimeType = m_liteApp->mimeTypeManager()->findFileMimeType(fileName);
    if (m_liteApp->projectManager()->mimeTypeList().contains(mimeType)) {
        return openProject(fileName);
    } else {
        return openEditor(fileName);
    }
}

bool FileManager::openEditor(const QString &fileName)
{
    QString mimeType = m_liteApp->mimeTypeManager()->findFileMimeType(fileName);
    IFile *file = m_liteApp->editorManager()->createFile(fileName,mimeType);
    if (file) {
        if (addFile(file) ) {
            addRecentFile(fileName);
            return true;
        }
    } else {
        removeRecentFile(fileName);
    }
    return false;
}

bool FileManager::openProject(const QString &fileName)
{
    QString mimeType = m_liteApp->mimeTypeManager()->findFileMimeType(fileName);
    IFile *file = m_liteApp->projectManager()->createFile(fileName,mimeType);
    if (file) {
        if (addFile(file)) {
            addRecentProject(fileName);
            return true;
        }
    } else {
        removeRecentProject(fileName);
    }
    return false;
}

void FileManager::updateRecentFileActions()
{
    QStringList files = m_liteApp->settings()->value("LiteApp/recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        m_recentFileActs[i]->setText(text);
        m_recentFileActs[i]->setData(files[i]);
        m_recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        m_recentFileActs[j]->setVisible(false);
}

void FileManager::updateRecentProjectActions()
{
    QStringList files = m_liteApp->settings()->value("LiteApp/recentProjectList").toStringList();

    int numRecentProjects = qMin(files.size(), (int)MaxRecentProjects);

    for (int i = 0; i < numRecentProjects; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        m_recentProjectActs[i]->setText(text);
        m_recentProjectActs[i]->setData(files[i]);
        m_recentProjectActs[i]->setVisible(true);
    }
    for (int j = numRecentProjects; j < MaxRecentProjects; ++j)
        m_recentProjectActs[j]->setVisible(false);
}

void FileManager::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
         openEditor(action->data().toString());
    }
}

void FileManager::openRecentProject()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
         openProject(action->data().toString());
    }
}

void FileManager::clearRecentFiles()
{
    m_liteApp->settings()->setValue("LiteApp/recentFileList",QStringList());
    updateRecentFileActions();
}

void FileManager::clearRecentProjects()
{
    m_liteApp->settings()->setValue("LiteApp/recentProjectList",QStringList());
    updateRecentProjectActions();
}


void FileManager::addRecentFile(const QString &_fileName)
{
    QString fileName = QDir::toNativeSeparators(_fileName);
    QStringList files = m_liteApp->settings()->value("LiteApp/recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles) {
        files.removeLast();
    }

    m_liteApp->settings()->setValue("LiteApp/recentFileList", files);
    updateRecentFileActions();
    emit recentFilesChanged();
}

void FileManager::addRecentProject(const QString &_fileName)
{
    QString fileName = QDir::toNativeSeparators(_fileName);
    QStringList files = m_liteApp->settings()->value("LiteApp/recentProjectList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentProjects) {
        files.removeLast();
    }

    m_liteApp->settings()->setValue("LiteApp/recentProjectList", files);
    updateRecentProjectActions();
    emit recentProjectsChanged();
}

void FileManager::removeRecentFile(const QString &_fileName)
{
    QString fileName = QDir::toNativeSeparators(_fileName);
    QStringList files = m_liteApp->settings()->value("LiteApp/recentFileList").toStringList();
    files.removeAll(fileName);
    m_liteApp->settings()->setValue("LiteApp/recentFileList", files);
    updateRecentFileActions();
    emit recentFilesChanged();
}

void FileManager::removeRecentProject(const QString &_fileName)
{
    QString fileName = QDir::toNativeSeparators(_fileName);
    QStringList files = m_liteApp->settings()->value("LiteApp/recentProjectList").toStringList();
    files.removeAll(fileName);
    m_liteApp->settings()->setValue("LiteApp/recentProjectList", files);
    updateRecentProjectActions();
    emit recentProjectsChanged();
}

void FileManager::updateFileState(IFile *file)
{
    if (!file) {
        return;
    }
    QString fileName = file->fileName();
    if (fileName.isEmpty()) {
        return;
    }
    FileStateItem item = {QFileInfo(fileName).lastModified(),file};
    m_fileStateMap.insert(fileName,item);
}

void FileManager::editorAboutToClose(LiteApi::IEditor *editor)
{
    IFile *file = editor->file();
    if (file) {
        removeFile(file);
    }
}

void FileManager::projectAboutToClose(LiteApi::IProject *project)
{
    IFile *file = project->file();
    if (file) {
        removeFile(file);
    }
}

void FileManager::editorSaved(LiteApi::IEditor *editor)
{
    IFile *file = editor->file();
    if (file) {
        updateFileState(file);
    }
}

void FileManager::fileChanged(QString fileName)
{
    const bool wasempty = m_changedFiles.isEmpty();
    if (!m_changedFiles.contains(fileName)) {
        m_changedFiles.append(fileName);
    }
    if (wasempty && !m_changedFiles.isEmpty()) {
        QTimer::singleShot(200, this, SLOT(checkForReload()));
    }
}

void FileManager::checkForReload()
{
    if (m_checkActivated) {
        return;
    }
    m_checkActivated = true;
    foreach (QString fileName, m_changedFiles) {
        if (!QFile::exists(fileName)) {
            //remove
            if (m_fileStateMap.contains(fileName)) {
                FileStateItem item = m_fileStateMap.value(fileName);
                LiteApi::IFile *file = item.file;
                QString text = QString(tr("%1\nThis file has been removed. Do you want save to file or close editor?")).arg(file->fileName());
                int ret = QMessageBox::question(m_liteApp->mainWindow(),tr("LiteIDE X"),text,QMessageBox::Save |QMessageBox::Close | QMessageBox::Cancel,QMessageBox::Save);
                if (ret == QMessageBox::Save) {
                    file->save(file->fileName());
                    m_fileWatcher->addPath(file->fileName());
                } else if (ret == QMessageBox::Close) {
                    foreach (LiteApi::IEditor *editor ,m_liteApp->editorManager()->editorList()) {
                        if (editor->file() == file) {
                            m_liteApp->editorManager()->closeEditor(editor);
                            break;
                        }
                    }
                }
            }
        } else {
            QDateTime modified = QFileInfo(fileName).lastModified();
            if (m_fileStateMap.contains(fileName)) {
                FileStateItem item = m_fileStateMap.value(fileName);
                if (item.modified >= modified) {
                    continue;
                } else {
                    item.file->reload(true);
                }
            }
        }
    }
    m_changedFiles.clear();
    m_checkActivated = false;
}


