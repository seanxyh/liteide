#ifndef GOPATHPROJECT_H
#define GOPATHPROJECT_H

#include "liteapi/liteapi.h"
#include "liteenvapi/liteenvapi.h"
#include <QModelIndex>

class QTreeView;
class GopathModel;
class GopathProject : public LiteApi::IProject
{
    Q_OBJECT
public:
    explicit GopathProject(QWidget *widget);
    ~GopathProject();
    virtual QWidget *widget();
    virtual QString name() const;
    virtual QString filePath() const;
    virtual QString mimeType() const;
    virtual QStringList fileNameList() const;
    virtual QStringList filePathList() const;
    virtual QString fileNameToFullPath(const QString &filePath);
    virtual QString target() const;
    virtual QString targetPath() const;
    virtual QString workPath() const;
    virtual void load();
public slots:
protected:
    QWidget *m_widget;
    QString m_path;
};

#endif // GOPATHPROJECT_H
