#include "literes.h"
#include <QFileInfo>
#include <QDebug>

using namespace LiteApi;

Resource::Resource()
{
}

void Resource::setResourcePath(const QString &path)
{
    m_resourcePath = path;
    m_imagePath = path+"/images";
    qDebug() << m_imagePath;
}

QString Resource::resourcePath() const
{
    return m_resourcePath;
}

QIcon Resource::icon(const QString &name) const
{
    qDebug() << m_imagePath+"/"+name;
    return QIcon(m_imagePath+"/"+name);
}


LiteApi::Resource & liteRes()
{
    static LiteApi::Resource res;
    return res;
}
