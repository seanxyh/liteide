#ifndef EDITORREPLACEDIALOG_H
#define EDITORREPLACEDIALOG_H

#include <QDialog>
#include "liteapi.h"

namespace Ui {
    class EditorReplaceDialog;
}

class EditorReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditorReplaceDialog(LiteApi::IApplication *app, QWidget *parent = 0);
    ~EditorReplaceDialog();

private:
    Ui::EditorReplaceDialog *ui;
    LiteApi::IApplication *m_liteApp;
};

#endif // EDITORREPLACEDIALOG_H
