#ifndef LITEIMG_H
#define LITEIMG_H

#include <QIcon>

namespace LiteApi {

class Resource
{
public:
    Resource();
    void setResourcePath(const QString &path);
    QString resourcePath() const;
    QIcon icon(const QString &name) const;
protected:
    QString m_resourcePath;
    QString m_imagePath;
};

} //namespace LiteApi

LiteApi::Resource& liteRes();

#endif // LITEIMG_H
