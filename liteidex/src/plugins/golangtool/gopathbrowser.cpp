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

    m_pathList = m_liteApp->settings()->value("golangtool/gopath").toStringList();

    connect(m_pathTree->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(pathIndexChanged(QModelIndex)));
    connect(m_pathTree,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(openPathIndex(QModelIndex)));
    LiteApi::IEnvManager* envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
    connect(envManager,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(reloadEnv()));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));
}

GopathBrowser::~GopathBrowser()
{    
    m_liteApp->settings()->setValue("golangtool/gopath",m_pathList);
    delete m_widget;
}

QWidget *GopathBrowser::widget() const
{
    return m_widget;
}

void GopathBrowser::addPathList(const QString &path)
{
    QStringList pathList = m_pathList;
    pathList.append(path);
    this->setPathList(pathList);
}

void GopathBrowser::setPathList(const QStringList &pathList)
{
    m_pathList = pathList;
    QStringList allPathList = systemGopathList()+m_pathList;
    m_model->setPathList(allPathList);
    m_pathTree->setCurrentIndex(m_model->startIndex());
    m_pathTree->expand(m_model->startIndex());
    LiteApi::IEnvManager* envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
    LiteApi::IEnv *env = envManager->currentEnv();
    if (env) {
#ifdef Q_OS_WIN
    env->environment().insert("LITEIDE_GOPATH",allPathList.join(";"));
#else
    env->environment().insert("LITEIDE_GOPATH",allPathList.join(":"));
#endif
    }
    currentEditorChanged(m_liteApp->editorManager()->currentEditor());
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

void GopathBrowser::setStartIndex(const QModelIndex &index)
{
    QModelIndex oldIndex = m_model->startIndex();
    if (oldIndex != index) {
        m_model->setStartIndex(index);
        m_pathTree->update(oldIndex);
        m_pathTree->update(index);
        emit startPathChanged(m_model->filePath(index));
    }
}

QString GopathBrowser::startPath() const
{
    QModelIndex index = m_model->startIndex();
    return m_model->filePath(index);
}

void GopathBrowser::pathIndexChanged(const QModelIndex & index)
{
    PathNode *node = m_model->nodeFromIndex(index);
    if (node) {
        QFileInfo info = node->fileInfo();
        QModelIndex newIndex = index;
        if (info.isDir()) {
            newIndex = index;
        } else {
            newIndex = index.parent();
        }
        this->setStartIndex(newIndex);
    }
}

void GopathBrowser::openPathIndex(const QModelIndex &index)
{
    PathNode *node = m_model->nodeFromIndex(index);
    if (node && node->isFile()) {
        m_liteApp->fileManager()->openEditor(node->path(),true);
    }
}

void GopathBrowser::currentEditorChanged(LiteApi::IEditor* editor)
{
    if (editor && !editor->filePath().isEmpty()) {
        QModelIndex index = m_model->findPath(editor->filePath());
        if (index.isValid()) {
            m_pathTree->setCurrentIndex(index);
            setStartIndex(index.parent());
        }
    }
}
