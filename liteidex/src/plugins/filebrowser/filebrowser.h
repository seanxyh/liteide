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
// Module: filebrowser.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-6-21
// $Id: filebrowser.h,v 1.0 2011-6-21 visualfc Exp $

#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include "liteapi.h"
#include <QModelIndex>

class QComboBox;
class QTreeView;
class QFileSystemModel;
class QSortFilterProxyModel;
class QFileInfo;
class QMenu;

class FileBrowser : public QObject
{
    Q_OBJECT
public:
    explicit FileBrowser(LiteApi::IApplication *app,QObject *parent = 0);
    virtual ~FileBrowser();
public:
    static QString getShellCmd(LiteApi::IApplication *app);
    static QStringList getShellArgs(LiteApi::IApplication *app);
signals:

public slots:
    void visibilityChanged(bool);
    void doubleClickedTreeView(QModelIndex);
    void activatedFilter(QString);
    void syncFileModel(bool);
    void currentEditorChanged(LiteApi::IEditor*);
    void treeViewContextMenuRequested(const QPoint &pos);
protected slots:
    void openFile();
    void newFile();
    void renameFile();
    void removeFile();
    void newFolder();
    void renameFolder();
    void removeFolder();
    void openShell();
protected:
    void showTreeViewContextMenu(const QPoint &globalPos, const QFileInfo &info);
protected:
    LiteApi::IApplication   *m_liteApp;
    QSortFilterProxyModel   *m_proxyModel;
    QWidget                 *m_widget;
    QTreeView               *m_treeView;
    QFileSystemModel        *m_fileModel;
    QComboBox   *m_filterCombo;
    QToolBar    *m_toolBar;
    QAction *m_syncAct;
protected:
    QModelIndex m_contextIndex;
    QMenu   *m_fileMenu;
    QMenu   *m_folderMenu;
    QAction *m_openFileAct;
    QAction *m_newFileAct;
    QAction *m_removeFileAct;
    QAction *m_renameFileAct;
    QAction *m_newFolderAct;
    QAction *m_removeFolderAct;
    QAction *m_renameFolderAct;
    QAction *m_openShellAct;
};

#endif // FILEBROWSER_H
