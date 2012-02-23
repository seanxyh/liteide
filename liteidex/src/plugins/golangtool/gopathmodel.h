#ifndef GOPATHMODEL_H
#define GOPATHMODEL_H

#include <QAbstractItemModel>
#include <QStringList>
#include <QIcon>
#include <QFileInfo>

class PathNode
{
public:
    PathNode();
    PathNode(const QString &path, PathNode *parent);
    ~PathNode();
    PathNode* parent();
    PathNode* child(int row);
    int childCount();
    int row() const;
    QList<PathNode*>* children();
    QString path() const;
    QString text() const;
    QFileInfo fileInfo() const;
    void clear();
protected:
    PathNode *m_parent;
    QList<PathNode*> *m_children;
    QString m_path;
    QString m_text;
};

class QFileIconProvider;

class GopathModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit GopathModel(QObject *parent = 0);
    ~GopathModel();
    void setPathList(const QStringList &pathList);
    QString filePath(const QModelIndex &index) const;
    PathNode *nodeFromIndex(const QModelIndex &index) const;    
    void setStartIndex(const QModelIndex &index);
    QModelIndex startIndex() const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual QModelIndex index(int row, int column,const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;    
signals:
    
public slots:

protected:
    QStringList m_pathList;
    PathNode *m_rootNode;
    QModelIndex m_startIndex;
    QFileIconProvider *m_iconProvider;
};

#endif // GOPATHMODEL_H
