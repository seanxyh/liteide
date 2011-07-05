#include "editorreplacedialog.h"
#include "ui_editorreplacedialog.h"

EditorReplaceDialog::EditorReplaceDialog(LiteApi::IApplication *app,QWidget *parent) :
    QDialog(parent),
    m_liteApp(app),
    ui(new Ui::EditorReplaceDialog)
{
    ui->setupUi(this);

    connect(ui->cancelPushButton,SIGNAL(clicked()),this,SLOT(reject()));
}

EditorReplaceDialog::~EditorReplaceDialog()
{
    delete ui;
}
