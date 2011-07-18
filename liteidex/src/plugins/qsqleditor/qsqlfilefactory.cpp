#include "qsqlfilefactory.h"
#include "sqlitefile.h"
#include "qsqleditor.h"

#include <QDebug>

QSqlFileFactory::QSqlFileFactory(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IFileFactory(parent),
    m_liteApp(app)
{
}

QStringList QSqlFileFactory::mimeTypes() const
{
    return QStringList() << "database/sqlite";
}

LiteApi::IFile *QSqlFileFactory::open(const QString &fileName, const QString &mimeType)
{
    QSqlDbFile *file = 0;
    if (mimeType == "database/sqlite") {
        file = new SQLiteFile(m_liteApp);
        if (!file->open(fileName,mimeType)) {
            delete file;
            return 0;
        }
    }
    if (file) {
        QSqlEditor *editor = new QSqlEditor(m_liteApp);
        editor->setFile(file);
        m_liteApp->editorManager()->addAutoReleaseEditor(editor);
    }
    return file;
}

bool QSqlFileFactory::targetInfo(const QString &fileName, const QString &mimetype, QString &target, QString &targetPath, QString &workPath) const
{
    return false;
}
