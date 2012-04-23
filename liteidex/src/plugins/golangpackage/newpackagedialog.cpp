#include "newpackagedialog.h"
#include "ui_newpackagedialog.h"

NewPackageDialog::NewPackageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewPackageDialog)
{
    ui->setupUi(this);
}

NewPackageDialog::~NewPackageDialog()
{
    delete ui;
}
