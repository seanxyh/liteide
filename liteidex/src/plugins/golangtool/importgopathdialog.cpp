#include "importgopathdialog.h"
#include "ui_importgopathdialog.h"

#include <QFileDialog>

ImportGopathDialog::ImportGopathDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportGopathDialog)
{
    ui->setupUi(this);
    connect(ui->browserButton,SIGNAL(clicked()),this,SLOT(browser()));
    connect(ui->clearButton,SIGNAL(clicked()),ui->pathTextEdit,SLOT(clear()));
}

ImportGopathDialog::~ImportGopathDialog()
{
    delete ui;
}

void ImportGopathDialog::setSysPathList(const QStringList &pathList)
{
    foreach (QString path, pathList) {
        ui->sysPathTextEdit->appendPlainText(path);
    }
}

void ImportGopathDialog::setPathList(const QStringList &pathList)
{
    foreach (QString path, pathList) {
        ui->pathTextEdit->appendPlainText(path);
    }
}

QStringList ImportGopathDialog::pathList() const
{
    return ui->pathTextEdit->toPlainText().split("\n",QString::SkipEmptyParts);
}

void ImportGopathDialog::browser()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Load GOPATH Directory"),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        ui->pathTextEdit->appendPlainText(dir);
    }
}
