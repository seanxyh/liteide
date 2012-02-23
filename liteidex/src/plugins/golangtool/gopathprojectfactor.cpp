#include "gopathprojectfactor.h"
#include "gopathproject.h"
#include "gopathbrowser.h"
#include <QDebug>

GopathProjectFactor::GopathProjectFactor(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IProjectFactory(parent),
    m_liteApp(app)
{
    m_mimeTypes << "text/x-gopath";
    m_browser = new GopathBrowser(app,this);

    connect(m_liteApp->projectManager(),SIGNAL(projectAboutToClose(LiteApi::IProject*)),this,SLOT(projectAboutToClose(LiteApi::IProject*)));
}

QStringList GopathProjectFactor::mimeTypes() const
{
    return m_mimeTypes;
}

LiteApi::IProject *GopathProjectFactor::open(const QString &fileName, const QString &mimeType)
{
    if (m_mimeTypes.contains(mimeType)) {        
        return new GopathProject(m_browser->widget());
    }
    return 0;
}

bool GopathProjectFactor::targetInfo(const QString &fileName, const QString &mimetype, QString &target, QString &targetPath, QString &workPath) const
{
    return false;
}

void GopathProjectFactor::projectAboutToClose(LiteApi::IProject* project)
{
}
