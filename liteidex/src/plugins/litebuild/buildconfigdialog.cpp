#include "buildconfigdialog.h"
#include "ui_buildconfigdialog.h"

#include <QAbstractItemModel>
#include <QDebug>

BuildConfigDialog::BuildConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BuildConfigDialog)
{
    ui->setupUi(this);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->verticalHeader()->hide();
    connect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(editTabView(QModelIndex)));
}

BuildConfigDialog::~BuildConfigDialog()
{
    delete ui;
}

void BuildConfigDialog::editTabView(QModelIndex index)
{
    if (!index.isValid()) {
        return;
    }
    if (index.column() == 1) {
        ui->tableView->edit(index);
    }
}

void BuildConfigDialog::setModel(QAbstractItemModel * model)
{
    ui->tableView->setModel(model);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->adjustSize();

    int total = 0;
    for (int i = 0; i < model->columnCount(); ++i) {
        total += ui->tableView->columnWidth(i);
    }
    int width = ui->tableView->size().width();
    if (total < width)
    {
        int extra = (width - total)/ model->columnCount();
        for (int i = 0; i < model->columnCount(); ++i)
            ui->tableView->setColumnWidth(i, ui->tableView->columnWidth(i) + extra);
    }
}
