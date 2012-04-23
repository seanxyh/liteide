#ifndef NEWPACKAGEDIALOG_H
#define NEWPACKAGEDIALOG_H

#include <QDialog>

namespace Ui {
class NewPackageDialog;
}

class NewPackageDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewPackageDialog(QWidget *parent = 0);
    ~NewPackageDialog();
    
private:
    Ui::NewPackageDialog *ui;
};

#endif // NEWPACKAGEDIALOG_H
