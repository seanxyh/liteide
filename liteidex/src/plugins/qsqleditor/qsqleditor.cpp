#include "qsqleditor.h"

QSqlEditor::QSqlEditor(LiteApi::IApplication *app) :
    m_liteApp(app),
    m_file(0),
    m_widget(0),
    m_bReadOnly(false)
{
    m_widget = new QWidget;
}

QSqlEditor::~QSqlEditor()
{
    if (m_file) {
        delete m_file;
    }
    if (m_widget) {
        delete m_widget;
    }
}

QWidget *QSqlEditor::widget()
{
    return m_widget;
}

QString QSqlEditor::displayName() const
{
    if (m_file) {
        return m_file->fileName();
    }
    return QString("");
}

bool QSqlEditor::open(const QString &fileName,const QString &mimeType)
{
    return false;
}

bool QSqlEditor::save()
{
    return false;
}

void QSqlEditor::setReadOnly(bool b)
{
    m_bReadOnly = b;
}

bool QSqlEditor::isReadOnly() const
{
    return m_bReadOnly;
}

bool QSqlEditor::isModified() const
{
    return false;
}

LiteApi::IFile *QSqlEditor::file()
{
    return m_file;
}

void QSqlEditor::setFile(QSqlDbFile *file)
{
    m_file = file;
}
