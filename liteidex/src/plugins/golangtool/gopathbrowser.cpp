#include "gopathbrowser.h"
#include "ui_gopathbrowser.h"
#include "gopathmodel.h"
#include "liteapi/litefindobj.h"
#include <QDebug>

GopathBrowser::GopathBrowser(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app)
{
    m_widget = new QWidget;
    m_pathTree = new QTreeView;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_pathTree);
    m_widget->setLayout(layout);
    m_model = new GopathModel(this);
    m_pathTree->setModel(m_model);

    connect(m_pathTree->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(pathIndexChanged(QModelIndex)));

    m_envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
    if (m_envManager) {
        connect(m_envManager,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(reload()));
        reload();
    }
}

GopathBrowser::~GopathBrowser()
{
    delete m_widget;
}

QWidget *GopathBrowser::widget() const
{
    return m_pathTree;
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
            m_pathTree->update(oldIndex);
            m_pathTree->update(newIndex);
        }
    }
}
