#ifndef GOPATHBROWSER_H
#define GOPATHBROWSER_H

#include <QWidget>
#include "liteapi/liteapi.h"
#include "liteenvapi/liteenvapi.h"
#include <QModelIndex>

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
    QStringList pathList() const;
    QStringList systemGopathList() const;
public slots:
    void pathIndexChanged(const QModelIndex & index);
    void reloadEnv();
signals:
    void startPathChanged(const QString& path);
private:
    LiteApi::IApplication *m_liteApp;
    QWidget *m_widget;
    QTreeView *m_pathTree;
    GopathModel *m_model;
    QStringList m_pathList;
};

#endif // GOPATHBROWSER_H
