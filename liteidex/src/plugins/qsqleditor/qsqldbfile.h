#ifndef QSQLDBFILE_H
#define QSQLDBFILE_H

#include "liteapi.h"
#include <QSqlDatabase>

class QSqlDbFile : public LiteApi::IFile
{
public:
    QSqlDbFile(LiteApi::IApplication *app, QObject *parent);
    ~QSqlDbFile();
public:
    virtual bool open(const QString &fileName, const QString &mimeType);
    virtual bool reload(bool externalModify);
    virtual bool save(const QString &fileName);
    virtual bool isReadOnly() const;
    virtual QString fileName() const;
    virtual QString mimeType() const;
public:
    QSqlDatabase & db();
protected:
    LiteApi::IApplication *m_liteApp;
    QSqlDatabase    m_db;
};

#endif // QSQLDBFILE_H
