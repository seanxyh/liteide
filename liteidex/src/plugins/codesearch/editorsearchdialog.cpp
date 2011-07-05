#include "editorsearchdialog.h"
#include "ui_editorsearchdialog.h"

#include "liteapi/litefindobj.h"
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QTextDocument>

EditorSearchDialog::EditorSearchDialog(LiteApi::IApplication *app, QWidget *parent) :
    QDialog(parent),
    m_liteApp(app),
    ui(new Ui::EditorSearchDialog)
{
    ui->setupUi(this);
    connect(ui->cancelPushButton,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->findNextPushButton,SIGNAL(clicked()),this,SLOT(findNext()));
    connect(ui->findPrevPushButton,SIGNAL(clicked()),this,SLOT(findPrev()));
}

EditorSearchDialog::~EditorSearchDialog()
{
    delete ui;
}

void EditorSearchDialog::findPrev()
{
    QString text = ui->findComboBox->currentText();
    if (text.isEmpty()) {
        return;
    }
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (editor) {
        findEditor(editor,true);
    }
}

void EditorSearchDialog::findNext()
{
    QString text = ui->findComboBox->currentText();
    if (text.isEmpty()) {
        return;
    }
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (editor) {
        findEditor(editor,false);
    }
}

void EditorSearchDialog::findEditor(LiteApi::IEditor *editor, bool findBackward)
{
    QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
    if (!ed) {
        return;
    }
    QTextCursor cursor = ed->textCursor();
    QString text = ui->findComboBox->currentText().trimmed();
    QTextDocument::FindFlags flags = 0;
    if (findBackward) {
        flags |= QTextDocument::FindBackward;
    }
    if (ui->matchCaseCheckBox->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (ui->matchWordCheckBox->isChecked()) {
        flags |= QTextDocument::FindWholeWords;
    }
    QTextCursor find;
    if (ui->useRegexCheckBox->isChecked()) {
        find = ed->document()->find(QRegExp(text),cursor,flags);
    } else {
        find = ed->document()->find(text,cursor,flags);
    }
    QString info;
    if (!find.isNull()) {
        ed->setTextCursor(find);
        info = QString(tr("Find %1:%2")).arg(find.blockNumber()+1).arg(find.columnNumber());
    } else {
        if (findBackward) {
            info = tr("Not find backward editor to begin!");
        } else {
            info = tr("Not find editor to end!");
        }
    }
    ui->infoLineEdit->setText(info);
}
