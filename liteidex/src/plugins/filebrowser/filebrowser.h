#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include "liteapi.h"
#include <QModelIndex>

class QComboBox;
class QDirModel;
class QTreeView;
class QFileSystemModel;
class QSortFilterProxyModel;


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
protected:
    LiteApi::IApplication   *m_liteApp;
    QWidget                 *m_widget;
    QTreeView               *m_treeView;
    QFileSystemModel        *m_fileModel;
    QSortFilterProxyModel   *m_proxyModel;
    QComboBox               *m_filterCombo;
    QToolBar                *m_toolBar;
    QAction                 *m_syncAct;
};

#endif // FILEBROWSER_H
