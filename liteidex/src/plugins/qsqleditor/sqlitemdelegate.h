#ifndef SQLITEMDELEGATE_H
#define SQLITEMDELEGATE_H

#include <QItemDelegate>

class SqlItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit SqlItemDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

};

#endif // SQLITEMDELEGATE_H
