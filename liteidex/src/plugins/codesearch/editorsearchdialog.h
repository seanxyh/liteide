#ifndef EDITORSEARCHDIALOG_H
#define EDITORSEARCHDIALOG_H

#include <QDialog>
#include "liteapi.h"

namespace Ui {
    class EditorSearchDialog;
}

class EditorSearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditorSearchDialog(LiteApi::IApplication *app, QWidget *parent = 0);
    ~EditorSearchDialog();
protected slots:
    void findNext();
    void findPrev();
    void findEditor(LiteApi::IEditor*,bool findBackward);
private:
    Ui::EditorSearchDialog *ui;
    LiteApi::IApplication *m_liteApp;
};

#endif // EDITORSEARCHDIALOG_H
