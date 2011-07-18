#include "sqlitemdelegate.h"
#include <QDebug>

SqlItemDelegate::SqlItemDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget *SqlItemDelegate::createEditor(QWidget *parent,
                      const QStyleOptionViewItem &option,
                      const QModelIndex &index) const
{
    QVariant var = index.data(Qt::EditRole);
    if (var.type() == QVariant::ByteArray) {
        return 0;
    }
    return QItemDelegate::createEditor(parent,option,index);
}
