#include "sqlitefile.h"
#include <QDebug>
#include <QSqlError>

SQLiteFile::SQLiteFile(LiteApi::IApplication *app, QObject *parent) :
    QSqlDbFile(app,parent)
{
}

bool SQLiteFile::open(const QString &fileName, const QString &mimeType)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE",fileName);
    m_db.setDatabaseName(fileName);
    return m_db.open();
}

QString SQLiteFile::mimeType() const
{
    return "database/sqlite";
}
