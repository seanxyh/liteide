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

    m_liteApp->settings()->beginGroup("codesearch_find");
    ui->matchWordCheckBox->setChecked(m_liteApp->settings()->value("matchword",false).toBool());
    ui->matchCaseCheckBox->setChecked(m_liteApp->settings()->value("matchcase",false).toBool());
    ui->useRegexCheckBox->setChecked(m_liteApp->settings()->value("useregex",false).toBool());
    ui->wrapAroundCheckBox->setChecked(m_liteApp->settings()->value("wraparound",true).toBool());
    m_liteApp->settings()->endGroup();

    connect(ui->closePushButton,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->findNextPushButton,SIGNAL(clicked()),this,SLOT(onFindNext()));
    connect(ui->findPrevPushButton,SIGNAL(clicked()),this,SLOT(onFindPrev()));
}

EditorSearchDialog::~EditorSearchDialog()
{
    m_liteApp->settings()->beginGroup("codesearch_find");
    m_liteApp->settings()->setValue("matchword",ui->matchWordCheckBox->isChecked());
    m_liteApp->settings()->setValue("matchcase",ui->matchCaseCheckBox->isChecked());
    m_liteApp->settings()->setValue("useregex",ui->useRegexCheckBox->isChecked());
    m_liteApp->settings()->setValue("wraparound",ui->wrapAroundCheckBox->isChecked());
    m_liteApp->settings()->endGroup();

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
