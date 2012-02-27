#include "gopathprojectfactor.h"
#include "gopathproject.h"
#include "gopathbrowser.h"
#include "importgopathdialog.h"
#include <QAction>
#include <QFileDialog>
#include <QDebug>

GopathProjectFactor::GopathProjectFactor(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IProjectFactory(parent),
    m_liteApp(app)
{
    m_mimeTypes << "text/x-gopath";
    m_browser = new GopathBrowser(app,this);

    QAction *act = new QAction(tr("<GOPATH> Project"),this);
    connect(act,SIGNAL(triggered()),this,SLOT(importGopath()));
    m_liteApp->projectManager()->addImportAction(act);
}

GopathProjectFactor::~GopathProjectFactor()
{
    if (m_browser) {
        delete m_browser;
    }
}

void GopathProjectFactor::importGopath()
{
    ImportGopathDialog *dlg = new ImportGopathDialog(m_liteApp->mainWindow());
    dlg->setSysPathList(m_browser->systemGopathList());
    dlg->setPathList(m_browser->pathList());
    if (dlg->exec() == QDialog::Accepted) {
        m_browser->setPathList(dlg->pathList());
        GopathProject *project =  new GopathProject(m_browser);
        m_liteApp->projectManager()->setCurrentProject(project);
    }
    delete dlg;
}

QStringList GopathProjectFactor::mimeTypes() const
{
    return m_mimeTypes;
}

LiteApi::IProject *GopathProjectFactor::open(const QString &fileName, const QString &mimeType)
{
    if (m_mimeTypes.contains(mimeType)) {        
        GopathProject *project =  new GopathProject(m_browser);
        project->browser()->setPathList(QStringList() << fileName);
        return project;
    }
    return 0;
}

bool GopathProjectFactor::targetInfo(const QString &fileName, const QString &mimetype, QString &target, QString &targetPath, QString &workPath) const
{
    return false;
}
