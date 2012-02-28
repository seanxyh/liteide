#ifndef GOPATHBROWSER_H
#define GOPATHBROWSER_H

#include <QWidget>
#include "liteapi/liteapi.h"
#include "liteenvapi/liteenvapi.h"
#include <QModelIndex>
#include <QFileInfo>
#include <QDir>

class QTreeView;
class GopathModel;
class GopathBrowser : public QObject
{
    Q_OBJECT
    
public:
    explicit GopathBrowser(LiteApi::IApplication *app,QObject *parent = 0);
    ~GopathBrowser();
    QWidget *widget() const;
    void setPathList(const QStringList &pathList);
    void addPathList(const QString &path);
    QStringList pathList() const;
    QStringList systemGopathList() const;
    void setStartIndex(const QModelIndex &index);
    QString startPath() const;
public slots:
    void pathIndexChanged(const QModelIndex & index);
    void openPathIndex(const QModelIndex &index);
    void reloadEnv();
    void currentEditorChanged(LiteApi::IEditor*);
    void treeViewContextMenuRequested(const QPoint &pos);
    void openEditor();
    void newFile();
    void newFileWizard();
    void renameFile();
    void removeFile();
    void newFolder();
    void renameFolder();
    void removeFolder();
    void openShell();
    void openExplorer();
signals:
    void startPathChanged(const QString& path);
protected:
    QFileInfo contextFileInfo() const;
    QDir contextDir() const;
    static QString getShellCmd(LiteApi::IApplication *app);
    static QStringList getShellArgs(LiteApi::IApplication *app);
private:
    LiteApi::IApplication *m_liteApp;
    QWidget *m_widget;
    QTreeView *m_pathTree;
    GopathModel *m_model;
    QStringList m_pathList;
    QFileInfo m_contextInfo;
    QMenu   *m_fileMenu;
    QMenu   *m_folderMenu;
    QAction *m_openEditorAct;
    QAction *m_newFileAct;
    QAction *m_newFileWizardAct;
    QAction *m_removeFileAct;
    QAction *m_renameFileAct;
    QAction *m_newFolderAct;
    QAction *m_removeFolderAct;
    QAction *m_renameFolderAct;
    QAction *m_openShellAct;
    QAction *m_openExplorerAct;
};

#endif // GOPATHBROWSER_H
