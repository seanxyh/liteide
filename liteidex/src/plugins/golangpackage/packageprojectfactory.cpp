#include "packageprojectfactory.h"
#include <packageproject.h>

PackageProjectFactory::PackageProjectFactory(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IProjectFactory(parent),
    m_liteApp(app)
{
    m_mimeTypes.append("text/x-gopackage");
    m_mimeTypes.append("text/x-gocommand");
}

QStringList PackageProjectFactory::mimeTypes() const
{
    return m_mimeTypes;
}

LiteApi::IProject *PackageProjectFactory::open(const QString &fileName, const QString &mimeType)
{
    if (!m_mimeTypes.contains(mimeType)) {
        return 0;
    }
    PackageProject *project = new PackageProject(m_liteApp);
    project->setPath(fileName);
    project->reload();
    return project;
}

bool PackageProjectFactory::findProjectInfo(const QString &fileName, const QString &mimetype, QMap<QString,QString>& projectInfo, QMap<QString,QString>& targetInfo) const
{
    return false;
}
