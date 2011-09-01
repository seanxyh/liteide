#ifndef BUILDCONFIGDIALOG_H
#define BUILDCONFIGDIALOG_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
    class BuildConfigDialog;
}

class QAbstractItemModel;
class BuildConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BuildConfigDialog(QWidget *parent = 0);
    ~BuildConfigDialog();
    void setModel(QAbstractItemModel * model);
public slots:
    void editTabView(QModelIndex);
private:
    Ui::BuildConfigDialog *ui;
};

#endif // BUILDCONFIGDIALOG_H
