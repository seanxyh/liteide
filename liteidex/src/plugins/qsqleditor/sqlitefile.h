#ifndef SQLITEFILE
#define SQLITEFILE

#include "qsqldbfile.h"

class SQLiteFile : public QSqlDbFile
{
    Q_OBJECT
public:
    SQLiteFile(LiteApi::IApplication *app, QObject *parent = 0);
public:
    virtual bool open(const QString &fileName, const QString &mimeType);
    virtual QString mimeType() const;
protected:
    LiteApi::IApplication *m_liteApp;
};

#endif // SQLITEFILE
