#include "sqltablemodel.h"
#include <QColor>
#include <QDebug>
#include <QSqlRecord>

SqlTableModel::SqlTableModel(QObject *parent, QSqlDatabase db) :
    QSqlTableModel(parent,db)
{
    m_blobColor = QColor(Qt::darkYellow);
}

QVariant SqlTableModel::data(const QModelIndex &idx, int role) const
{
    QVariant var = QSqlTableModel::data(idx,Qt::DisplayRole);
    if (var.isNull()) {
        if (role == Qt::BackgroundColorRole)
            return QVariant(m_blobColor);
        if (role == Qt::ToolTipRole)
            return QVariant(tr("NULL value"));
        if (role == Qt::DisplayRole)
            return QVariant(tr("{null}"));
        if (role == Qt::EditRole)
            return QSqlTableModel::data(idx, Qt::DisplayRole);
        if (role == Qt::UserRole+1)
            return QVariant("{blob}");

    } else if (var.type() == QVariant::ByteArray) {
        if (role == Qt::BackgroundColorRole)
            return QVariant(m_blobColor);
        if (role == Qt::ToolTipRole)
            return QVariant(tr("BLOB value"));
        if (role == Qt::DisplayRole)
            return QVariant(tr("{blob}"));
        if (role == Qt::EditRole)
            return QSqlTableModel::data(idx, Qt::DisplayRole);
        if (role == Qt::UserRole+1)
            return QVariant("{blob}");

    }

    return QSqlTableModel::data(idx,role);
}
