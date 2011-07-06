#ifndef EDITORSEARCHDIALOG_H
#define EDITORSEARCHDIALOG_H

#include <QDialog>
#include <QTextCursor>
#include "liteapi.h"

class QPlainTextEdit;

namespace Ui {
    class EditorSearchDialog;
}

class EditorSearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditorSearchDialog(LiteApi::IApplication *app, QWidget *parent = 0);
    ~EditorSearchDialog();
    void setFindText(const QString &text);
public slots:
    void findNext();
    void findPrev();
protected slots:
    void onFindNext();
    void onFindPrev();
protected:
    QTextCursor findEditor(QPlainTextEdit*,const QTextCursor &cursor,bool findBackward);
private:
    Ui::EditorSearchDialog *ui;
    LiteApi::IApplication *m_liteApp;
};

#endif // EDITORSEARCHDIALOG_H
