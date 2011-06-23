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
signals:

public slots:
    void visibilityChanged(bool);
    void doubleClickedTreeView(QModelIndex);
    void activatedFilter(QString);
    void syncFileModel(bool);
    void currentEditorChanged(LiteApi::IEditor*);
    void treeViewContextMenuRequested(const QPoint &pos);
protected slots:
    void openEditor();
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
    QAction *m_openEditorAct;
    QAction *m_newFileAct;
    QAction *m_removeFileAct;
    QAction *m_renameFileAct;
    QAction *m_newFolderAct;
    QAction *m_removeFolderAct;
    QAction *m_renameFolderAct;
    QAction *m_openShellAct;
};

#endif // FILEBROWSER_H
