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
protected:
    LiteApi::IApplication   *m_liteApp;
    QWidget                 *m_widget;
    QTreeView               *m_treeView;
    QFileSystemModel        *m_fileModel;
    QSortFilterProxyModel   *m_proxyModel;
    QComboBox               *m_filterCombo;
};

#endif // FILEBROWSER_H
