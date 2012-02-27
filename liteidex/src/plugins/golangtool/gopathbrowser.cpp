#include "gopathbrowser.h"
#include "gopathmodel.h"
#include "liteapi/litefindobj.h"
#include <QTreeView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDebug>

GopathBrowser::GopathBrowser(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app)
{
    m_widget = new QWidget;

    m_pathTree = new QTreeView;
    m_pathTree->setHeaderHidden(true);
    m_model = new GopathModel(this);
    m_pathTree->setModel(m_model);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(m_pathTree);
    m_widget->setLayout(layout);

    connect(m_pathTree->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(pathIndexChanged(QModelIndex)));
    LiteApi::IEnvManager* envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
    connect(envManager,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(reloadEnv()));
}

GopathBrowser::~GopathBrowser()
{
    delete m_widget;
}

QWidget *GopathBrowser::widget() const
{
    return m_widget;
}

void GopathBrowser::setPathList(const QStringList &pathList)
{
    m_pathList = pathList;
    QStringList allPathList = systemGopathList()+m_pathList;
    m_model->setPathList(allPathList);
    int row = 0;
    if (m_model->rowCount() > 0) {
        row = m_model->rowCount()-1;
    }
    m_pathTree->expand(m_model->index(row,0));
    m_model->setStartIndex(m_model->index(row,0));
    LiteApi::IEnvManager* envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
#ifdef Q_OS_WIN
    envManager->currentEnvironment().insert("GOPATH",allPathList.join(";"));
#else
    envManager->currentEnvironment().insert("GOPATH",allPathList.join(":"));
#endif
}

QStringList GopathBrowser::pathList() const
{
    return m_pathList;
}

QStringList GopathBrowser::systemGopathList() const
{
    LiteApi::IEnvManager* envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
    QString gopath= envManager->currentEnvironment().value("GOPATH");
    if (gopath.isEmpty()) {
        gopath = envManager->currentEnvironment().value("GOROOT");
    }
    QStringList list;
 #ifdef Q_OS_WIN
     list = gopath.split(";",QString::SkipEmptyParts);
 #else
     list = gopath.split(":",QString::SkipEmptyParts);
 #endif
     return list;
}

void GopathBrowser::reloadEnv()
{
    this->setPathList(m_pathList);
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
            emit startPathChanged(m_model->filePath(newIndex));
        }
    }
}
