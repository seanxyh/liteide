#ifndef TABLEVIEWWIDGET_H
#define TABLEVIEWWIDGET_H

#include <QWidget>
#include <QSqlDatabase>

namespace Ui {
    class TableViewWidget;
}

class SqlTableModel;
class SqlItemDelegate;
class TableViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TableViewWidget(QSqlDatabase &db, QWidget *parent = 0);
    ~TableViewWidget();
    void setTable(const QString &table);
private:
    Ui::TableViewWidget *ui;
    SqlTableModel      *m_model;
    SqlItemDelegate    *m_delegate;
};

#endif // TABLEVIEWWIDGET_H
