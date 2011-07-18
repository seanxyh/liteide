#include "tableviewwidget.h"
#include "ui_tableviewwidget.h"

#include "sqltablemodel.h"
#include "sqlitemdelegate.h"
#include <QTableWidgetItem>

TableViewWidget::TableViewWidget(QSqlDatabase &db, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TableViewWidget)
{
    ui->setupUi(this);
    m_model = new SqlTableModel(this,db);
    m_delegate = new SqlItemDelegate(this);

    ui->tableView->setModel(m_model);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setItemDelegate(m_delegate);
}

TableViewWidget::~TableViewWidget()
{
    delete ui;
}

void TableViewWidget::setTable(const QString &table)
{
    m_model->setTable(table);
    m_model->select();

    if (m_model->columnCount() <= 0)
            return;

    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();

    int total = 0;
    for (int i = 0; i < m_model->columnCount(); ++i)
        total += ui->tableView->columnWidth(i);

    if (total < ui->tableView->viewport()->width())
    {
        int extra = (ui->tableView->viewport()->width() - total)
                / m_model->columnCount();
        for (int i = 0; i < m_model->columnCount(); ++i)
            ui->tableView->setColumnWidth(i, ui->tableView->columnWidth(i) + extra);
    }
}
