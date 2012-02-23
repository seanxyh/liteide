#ifndef GOPATHBROWSER_H
#define GOPATHBROWSER_H

#include <QWidget>
#include "liteapi/liteapi.h"
#include "liteenvapi/liteenvapi.h"
#include <QModelIndex>

namespace Ui {
class GopathBrowser;
}

class GopathModel;
class GopathBrowser : public QObject
{
    Q_OBJECT
    
public:
    explicit GopathBrowser(LiteApi::IApplication *app,QObject *parent = 0);
    ~GopathBrowser();
public slots:
    void pathIndexChanged(const QModelIndex & index);
    void reload();
private:
    LiteApi::IApplication *m_liteApp;
    QWidget *m_widget;
    Ui::GopathBrowser *ui;
    LiteApi::IEnvManager *m_envManager;
    GopathModel *m_model;
    QStringList m_gopath;
};

#endif // GOPATHBROWSER_H
