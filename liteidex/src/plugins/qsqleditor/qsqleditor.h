#ifndef QSQLEDITOR_H
#define QSQLEDITOR_H

#include "liteapi.h"
#include "qsqldbfile.h"
#include <QModelIndex>
#include <QtSql/qsql.h>

namespace Ui {
    class QSqlEditor;
}

class QStandardItemModel;

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
    void loadDatabase();
protected slots:
    void dbTreeContextMenuRequested(const QPoint& pt);
    void editorTable();
protected:
    void appendTableItems(QSql::TableType type);
    QString tableTypeNames(int type) const;
    QString tableTypeName(int type) const;
protected:
    LiteApi::IApplication *m_liteApp;
    QSqlDbFile  *m_file;
    QWidget     *m_widget;
    Ui::QSqlEditor  *ui;
    bool    m_bReadOnly;
    QStandardItemModel *m_dbModel;
    QMenu   *m_tableMenu;
    QAction *m_infoAct;
    QAction *m_editorAct;
    QModelIndex m_contextIndex;
};

#endif // QSQLEDITOR_H
