#ifndef SQLTABLEMODEL_H
#define SQLTABLEMODEL_H

#include <QSqlTableModel>
#include <QColor>

class SqlTableModel : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit SqlTableModel(QObject *parent = 0,QSqlDatabase db = QSqlDatabase());
    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const;
signals:

public slots:
protected:
    QColor m_blobColor;
};

#endif // SQLTABLEMODEL_H
