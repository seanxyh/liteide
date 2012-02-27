#ifndef IMPORTGOPATHDIALOG_H
#define IMPORTGOPATHDIALOG_H

#include <QDialog>

namespace Ui {
class ImportGopathDialog;
}

class ImportGopathDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ImportGopathDialog(QWidget *parent = 0);
    ~ImportGopathDialog();
    void setSysPathList(const QStringList &pathList);
    void setPathList(const QStringList &pathList);
    QStringList pathList() const;
public slots:
    void browser();
private:
    Ui::ImportGopathDialog *ui;
};

#endif // IMPORTGOPATHDIALOG_H
