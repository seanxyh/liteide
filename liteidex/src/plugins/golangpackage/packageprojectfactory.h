#ifndef PACKAGEPROJECTFACTORY_H
#define PACKAGEPROJECTFACTORY_H

#include "liteapi/liteapi.h"

class PackageProjectFactory : public LiteApi::IProjectFactory
{
public:
    PackageProjectFactory(LiteApi::IApplication *app, QObject *parnet);
    virtual QStringList mimeTypes() const;
    virtual LiteApi::IProject *open(const QString &fileName, const QString &mimeType);
    virtual bool findProjectInfo(const QString &fileName, const QString &mimetype, QMap<QString,QString>& projectInfo, QMap<QString,QString>& targetInfo) const;
public:
    LiteApi::IApplication *m_liteApp;
    QStringList m_mimeTypes;
};

#endif // PACKAGEPROJECTFACTORY_H
