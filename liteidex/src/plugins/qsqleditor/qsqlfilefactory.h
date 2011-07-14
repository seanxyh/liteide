#ifndef QSQLFILEFACTORY_H
#define QSQLFILEFACTORY_H

#include "liteapi.h"

class QSqlFileFactory : public LiteApi::IFileFactory
{
public:
    QSqlFileFactory(LiteApi::IApplication *app, QObject *parent);

    virtual QStringList mimeTypes() const;
    virtual LiteApi::IFile *open(const QString &fileName, const QString &mimeType);
    virtual QString target(const QString &fileName, const QString &mimetype) const;
protected:
    LiteApi::IApplication *m_liteApp;
};

#endif // QSQLFILEFACTORY_H
