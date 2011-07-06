#ifndef EDITORREPLACEDIALOG_H
#define EDITORREPLACEDIALOG_H

#include <QDialog>
#include <QTextCursor>
#include "liteapi.h"

class QPlainTextEdit;
namespace Ui {
    class EditorReplaceDialog;
}

class EditorReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditorReplaceDialog(LiteApi::IApplication *app, QWidget *parent = 0);
    ~EditorReplaceDialog();
protected slots:
    void findNext();
    void findPrev();
    void replace();
    void replaceAll();
protected:
    bool replaceFind();
    QTextCursor findEditor(QPlainTextEdit *ed, const QTextCursor &cursor, bool findBackward);
private:
    LiteApi::IApplication *m_liteApp;
    Ui::EditorReplaceDialog *ui;
    QTextCursor m_find;
};

#endif // EDITORREPLACEDIALOG_H
