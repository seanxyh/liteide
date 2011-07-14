#include "qsqldbfile.h"
#include <QDebug>

QSqlDbFile::QSqlDbFile(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IFile(parent),
    m_liteApp(app)
{
}

QSqlDbFile::~QSqlDbFile()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool QSqlDbFile::open(const QString &fileName, const QString &mimeType)
{
    return false;
}

bool QSqlDbFile::reload(bool externalModify)
{
    return false;
}

bool QSqlDbFile::save(const QString &fileName)
{
    return false;
}

bool QSqlDbFile::isReadOnly() const
{
    return false;
}

QString QSqlDbFile::fileName() const
{
    return m_db.databaseName();
}

QString QSqlDbFile::mimeType() const
{
    return "database/sql";
}

QSqlDatabase &QSqlDbFile::db()
{
    return m_db;
}
