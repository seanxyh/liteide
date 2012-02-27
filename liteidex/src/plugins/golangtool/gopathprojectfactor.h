#ifndef GOPATHPROJECTFACTOR_H
#define GOPATHPROJECTFACTOR_H

#include "liteapi/liteapi.h"

class GopathBrowser;
class GopathProjectFactor : public LiteApi::IProjectFactory
{
    Q_OBJECT
public:
    explicit GopathProjectFactor(LiteApi::IApplication *app, QObject *parent = 0);
    ~GopathProjectFactor();
    virtual QStringList mimeTypes() const;
    virtual LiteApi::IProject *open(const QString &fileName, const QString &mimeType);
    virtual bool targetInfo(const QString &fileName, const QString &mimetype, QString &target, QString &targetPath, QString &workPath) const;    
public slots:
    void importGopath();
protected:
    LiteApi::IApplication *m_liteApp;
    GopathBrowser *m_browser;
    QStringList m_mimeTypes;
};

#endif // GOPATHPROJECTFACTOR_H
