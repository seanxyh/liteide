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
public slots:
    void pathIndexChanged(const QModelIndex & index);
    void reload();
signals:
    void startPathChanged(const QString& path);
private:
    LiteApi::IApplication *m_liteApp;
    QWidget *m_widget;
    QTreeView *m_pathTree;
    LiteApi::IEnvManager *m_envManager;
    GopathModel *m_model;
    QStringList m_gopath;
};

#endif // GOPATHBROWSER_H
