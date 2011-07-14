#ifndef QSQLEDITOR_H
#define QSQLEDITOR_H

#include "liteapi.h"
#include "qsqldbfile.h"

class QSqlEditor : public LiteApi::IEditor
{
    Q_OBJECT
public:
    explicit QSqlEditor(LiteApi::IApplication *app);
    ~QSqlEditor();
public:
    virtual QWidget *widget();
    virtual QString displayName() const;
public:
    virtual bool open(const QString &fileName,const QString &mimeType);
    virtual bool save();
    virtual void setReadOnly(bool b);
    virtual bool isReadOnly() const;
    virtual bool isModified() const;
    virtual LiteApi::IFile *file();
public:
    void setFile(QSqlDbFile *file);
signals:

public slots:
protected:
    LiteApi::IApplication *m_liteApp;
    QSqlDbFile  *m_file;
    QWidget     *m_widget;
    bool    m_bReadOnly;
};

#endif // QSQLEDITOR_H
