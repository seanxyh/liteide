#include "gopathmodel.h"
#include <QFileInfo>
#include <QDir>
#include <QIcon>
#include <QFont>
#include <QFileIconProvider>
#include <QDebug>

PathNode::PathNode() : m_parent(0), m_children(0)
{

}

PathNode::PathNode(const QString &path, PathNode *parent) : m_parent(parent), m_children(0), m_path(path)
{
    QFileInfo info(path);
    if (parent && parent->parent() == 0) {
        m_text = info.filePath();
    } else {
        m_text = info.fileName();
    }
}

PathNode::~PathNode()
{
    if (m_children) {
        qDeleteAll(m_children->begin(),m_children->end());
        delete m_children;
    }
}

QList<PathNode*>* PathNode::children()
{
    if (m_children == 0) {
        m_children = new QList<PathNode*>();
        if (!m_path.isEmpty()) {
            QFileInfo info(m_path);
            if (info.isDir()) {
                QDir dir(m_path);
                foreach(QFileInfo childInfo, dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot,QDir::DirsFirst)) {
                    m_children->append(new PathNode(childInfo.filePath(),this));
                }
            }
        }
    }
    return m_children;
}

PathNode* PathNode::parent()
{
    return m_parent;
}

PathNode* PathNode::child(int row)
{
    return children()->at(row);
}

int PathNode::childCount()
{
    return children()->count();
}

int PathNode::row() const
{
    if (m_parent) {
        return m_parent->children()->indexOf(const_cast<PathNode*>(this));
    }
    return 0;
}

QString PathNode::path() const
{
    return m_path;
}

QString PathNode::text() const
{
    return m_text;
}

QFileInfo PathNode::fileInfo() const
{
    return QFileInfo(m_path);
}

void PathNode::clear()
{
    if (m_children) {
        qDeleteAll(m_children->begin(),m_children->end());
        m_children->clear();
    }
}


GopathModel::GopathModel(QObject *parent) :
    QAbstractItemModel(parent),
    m_rootNode(new PathNode),
    m_iconProvider(new QFileIconProvider)
{
}

GopathModel::~GopathModel()
{
    delete m_rootNode;
}

PathNode *GopathModel::nodeFromIndex(const QModelIndex &index) const
{
    if (index.isValid()) {
        return static_cast<PathNode*>(index.internalPointer());
    }
    return m_rootNode;
}

void GopathModel::setStartIndex(const QModelIndex &index)
{
    m_startIndex = index;
}

QModelIndex GopathModel::startIndex() const
{
    return m_startIndex;
}

QString GopathModel::filePath(const QModelIndex &index) const
{
    return nodeFromIndex(index)->path();
}

void GopathModel::setPathList(const QStringList &pathList)
{
    m_rootNode->clear();
    foreach(QString path, pathList) {
        m_rootNode->children()->append(new PathNode(path,m_rootNode));
    }
    reset();
}

int GopathModel::rowCount(const QModelIndex &parent) const
{
    PathNode *node = nodeFromIndex(parent);
    return node->childCount();
}

int GopathModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QModelIndex GopathModel::parent(const QModelIndex &child) const
{
    PathNode *node = nodeFromIndex(child);
    PathNode *parent = node->parent();
    if (parent == m_rootNode) {
        return QModelIndex();
    }
    return createIndex(parent->row(),0,parent);
}

QModelIndex GopathModel::index(int row, int column,const QModelIndex &parent) const
{
    if (!hasIndex(row,column,parent))
        return QModelIndex();
    PathNode *node = nodeFromIndex(parent);
    return createIndex(row,column,node->child(row));
}

QVariant GopathModel::data(const QModelIndex &index, int role) const
{
    PathNode *node = nodeFromIndex(index);
    if (!node) {
        return QVariant();
    }
    switch(role) {
    case Qt::DisplayRole:
        return node->text();
    case Qt::DecorationRole:
        return m_iconProvider->icon(node->fileInfo());
    case Qt::FontRole: {
        QFont font;
        if (index == m_startIndex) {
            font.setBold(true);
        }
        return font;
    }
    }
    return QVariant();
}
