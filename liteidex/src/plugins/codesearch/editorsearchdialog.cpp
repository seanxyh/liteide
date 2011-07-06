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
    ui->wrapAroundCheckBox->setChecked(true);

    connect(ui->closePushButton,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->findNextPushButton,SIGNAL(clicked()),this,SLOT(onFindNext()));
    connect(ui->findPrevPushButton,SIGNAL(clicked()),this,SLOT(onFindPrev()));
}

EditorSearchDialog::~EditorSearchDialog()
{
    delete ui;
}

void EditorSearchDialog::setFindText(const QString &text)
{
    ui->findComboBox->setEditText(text);
}

void EditorSearchDialog::onFindNext()
{
    accept();
    findNext();
}

void EditorSearchDialog::onFindPrev()
{
    accept();
    findPrev();
}

void EditorSearchDialog::findPrev()
{
    QString text = ui->findComboBox->currentText();
    if (text.isEmpty()) {
        return;
    }
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
    if (!ed) {
        return;
    }

    QTextCursor find = findEditor(ed,ed->textCursor(),true);
    if (find.isNull() && ui->wrapAroundCheckBox->isChecked()) {
        ed->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
        find = findEditor(ed,ed->textCursor(),true);
    }
    if (!find.isNull()) {
        ed->setTextCursor(find);
    }
}

void EditorSearchDialog::findNext()
{
    QString text = ui->findComboBox->currentText();
    if (text.isEmpty()) {
        return;
    }
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
    if (!ed) {
        return;
    }
    QTextCursor find = findEditor(ed,ed->textCursor(),false);
    if (find.isNull() && ui->wrapAroundCheckBox->isChecked()) {
        ed->moveCursor(QTextCursor::Start,QTextCursor::MoveAnchor);
        find = findEditor(ed,ed->textCursor(),false);
    }
    if (!find.isNull()) {
        ed->setTextCursor(find);
    }
}

QTextCursor EditorSearchDialog::findEditor(QPlainTextEdit *ed, const QTextCursor &cursor, bool findBackward)
{
    QString text = ui->findComboBox->currentText();
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
    return find;
}
