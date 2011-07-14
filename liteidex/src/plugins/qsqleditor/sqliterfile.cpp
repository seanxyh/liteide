#include "sqlitefile.h"
#include <QDebug>
#include <QSqlError>

SQLiteFile::SQLiteFile(LiteApi::IApplication *app, QObject *parent) :
    QSqlDbFile(app,parent)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

bool SQLiteFile::open(const QString &fileName, const QString &mimeType)
{
    m_db.setDatabaseName(fileName);
    return m_db.open();
}

QString SQLiteFile::mimeType() const
{
    return "database/sqlite";
}
