#ifndef FINDEDITOR_H
#define FINDEDITOR_H

#include "liteapi/liteapi.h"
#include <QTextCursor>
#include <QLabel>

class QLineEdit;
class QCheckBox;

struct FindState {
    QString findText;
    bool    useRegexp;
    bool    matchWord;
    bool    matchCase;
    bool    wrapAround;
    bool    backWard;
    bool isValid() {
        return !findText.isEmpty();
    }
};

class FindEditor : public QObject
{
    Q_OBJECT
public:
    explicit FindEditor(LiteApi::IApplication *app, QObject *parent = 0);
    virtual ~FindEditor();
    virtual QWidget *widget();
    virtual void setReady();
    QString findText() const;
    void genFindState(FindState *state, bool backWard);
public slots:
    void findNext();
    void findPrev();
public:
    QTextCursor findEditor(QTextDocument *ed, const QTextCursor &cursor, FindState *state);
    template <typename T>
    void findHelper(T *ed, FindState *state)
    {
        QTextCursor cursor = ed->textCursor();
        QTextCursor find = findEditor(ed->document(),cursor,state);
        if (find.isNull() && state->wrapAround) {
            if (state->backWard) {
                cursor.movePosition(QTextCursor::End,QTextCursor::MoveAnchor);
            } else {
                cursor.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
            }
            find = findEditor(ed->document(),cursor,state);
        }
        if (!find.isNull()) {
            ed->setTextCursor(find);
            m_status->setText(QString("Ln:%1 Col:%2").
                                  arg(find.blockNumber()+1).
                                  arg(find.columnNumber()));
        } else {
            m_status->setText(tr("Not find"));
        }
    }
protected:
    LiteApi::IApplication *m_liteApp;
    QWidget     *m_widget;
    QLineEdit   *m_findEdit;
    QCheckBox   *m_matchWordCheckBox;
    QCheckBox   *m_matchCaseCheckBox;
    QCheckBox   *m_useRegexCheckBox;
    QCheckBox   *m_wrapAroundCheckBox;
    QLabel      *m_status;
};



#endif // FINDEDITOR_H
