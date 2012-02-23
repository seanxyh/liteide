#include "gopathbrowser.h"
#include "ui_gopathbrowser.h"
#include "gopathmodel.h"
#include "liteapi/litefindobj.h"
#include <QDebug>

GopathBrowser::GopathBrowser(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_widget(new QWidget),
    ui(new Ui::GopathBrowser)
{
    ui->setupUi(m_widget);
    m_model = new GopathModel(this);
    ui->pathTreeView->setModel(m_model);
    m_liteApp->dockManager()->addDock(m_widget,tr("GOPATH Browser"));
    /*
    m_widget->setStyleSheet("QTreeView::item:hover{background-color:rgb(0,255,0,50)}"
                            "QTreeView::item:selected{background-color:rgb(0,0,255,100)}"
                            "QTreeView::item:!active{background-color:transparent}");
    */
    //connect(ui->pathTreeView,SIGNAL(pressed(QModelIndex)),this,SLOT(pathTreePressed(QModelIndex)));
    connect(ui->pathTreeView->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(pathIndexChanged(QModelIndex)));

    m_envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
    if (m_envManager) {
        connect(m_envManager,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(reload()));
        reload();
    }
}

GopathBrowser::~GopathBrowser()
{
    delete ui;
    delete m_widget;
}

void GopathBrowser::reload()
{
   QString gopath= m_envManager->currentEnvironment().value("GOPATH");
   if (gopath.isEmpty()) {
       gopath = m_envManager->currentEnvironment().value("GOROOT");
   }
#ifdef Q_OS_WIN
    m_gopath = gopath.split(";",QString::SkipEmptyParts);
#else
    m_gopath = gopath.split(":",QString::SkipEmptyParts);
#endif
    qDebug() << m_gopath;
    m_model->setPathList(m_gopath);
}

void GopathBrowser::pathIndexChanged(const QModelIndex & index)
{
    PathNode *node = m_model->nodeFromIndex(index);
    if (node) {
        QFileInfo info = node->fileInfo();
        QModelIndex oldIndex = m_model->startIndex();
        QModelIndex newIndex = index;
        if (info.isDir()) {
            newIndex = index;
        } else {
            newIndex = index.parent();
        }
        if (newIndex != oldIndex) {
            m_model->setStartIndex(newIndex);
            ui->pathTreeView->update(oldIndex);
            ui->pathTreeView->update(newIndex);
        }
    }
}
