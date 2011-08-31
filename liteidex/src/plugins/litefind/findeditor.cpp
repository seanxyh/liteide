#include "findeditor.h"
#include "liteapi/litefindobj.h"

#include <QLineEdit>
#include <QTextCursor>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QTextBrowser>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

FindEditor::FindEditor(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_widget(new QWidget)
{
    m_findEdit = new QLineEdit;

    m_matchWordCheckBox = new QCheckBox(tr("Match word"));
    m_matchCaseCheckBox = new QCheckBox(tr("Match case"));
    m_useRegexCheckBox = new QCheckBox(tr("Regular expression"));
    m_wrapAroundCheckBox = new QCheckBox(tr("Wrap around"));

    QPushButton *findNext = new QPushButton(tr("Find Next"));
    QPushButton *findPrev = new QPushButton(tr("Find Prev"));
    QPushButton *switchReplace = new QPushButton(tr("Replace"));
    switchReplace->setToolTip(tr("Switch Replace"));
    QPushButton *hideFind = new QPushButton(tr("Hide"));
    hideFind->setToolTip(tr("Hide Find"));


    m_status = new QLabel(tr("Ready"));
    m_status->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QGridLayout *layout = new QGridLayout;
    layout->setMargin(0);

    QHBoxLayout *optLayout = new QHBoxLayout;
    optLayout->setMargin(0);

    optLayout->addWidget(m_matchWordCheckBox);
    optLayout->addWidget(m_matchCaseCheckBox);
    optLayout->addWidget(m_useRegexCheckBox);
    optLayout->addWidget(m_wrapAroundCheckBox);
    optLayout->addStretch();
    optLayout->addWidget(m_status);

    layout->addWidget(new QLabel(tr("Find What:")),0,0);
    layout->addWidget(m_findEdit,0,1);
    layout->addWidget(findNext,0,2);
    layout->addWidget(findPrev,0,3);
    layout->addWidget(switchReplace,0,4);
    layout->addWidget(hideFind,0,5);
    layout->addWidget(new QLabel(tr("Find Option:")),1,0);
    layout->addLayout(optLayout,1,1,1,5);

    m_widget->setLayout(layout);

    connect(m_findEdit,SIGNAL(returnPressed()),this,SLOT(findNext()));
    connect(findNext,SIGNAL(clicked()),this,SLOT(findNext()));
    connect(findPrev,SIGNAL(clicked()),this,SLOT(findPrev()));
    connect(hideFind,SIGNAL(clicked()),this,SIGNAL(hideFind()));
    connect(switchReplace,SIGNAL(clicked()),this,SIGNAL(swithReplace()));
}

FindEditor::~FindEditor()
{
    if (m_widget) {
        delete m_widget;
    }
}

QWidget *FindEditor::widget()
{
    return m_widget;
}

void FindEditor::setReady()
{
    m_widget->setFocus();
    m_status->setText(tr("Ready"));
    m_findEdit->setFocus();
}

QString FindEditor::findText() const
{
    return m_findEdit->text();
}

void FindEditor::genFindState(FindState *state, bool backWard)
{
    state->findText = m_findEdit->text();
    state->matchWord = m_matchWordCheckBox->isChecked();
    state->matchCase = m_matchCaseCheckBox->isChecked();
    state->useRegexp = m_useRegexCheckBox->isChecked();
    state->wrapAround = m_wrapAroundCheckBox->isChecked();
    state->backWard = backWard;
}

void FindEditor::findNext()
{
    FindState state;
    genFindState(&state,false);
    if (!state.isValid()) {
        return;
    }

    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
    if (ed) {
        findHelper(ed,&state);
    } else {
        QTextBrowser *ed = LiteApi::findExtensionObject<QTextBrowser*>(editor,"LiteApi.QTextBrowser");
        if (ed) {
            findHelper(ed,&state);
        }
    }
}

void FindEditor::findPrev()
{
    FindState state;
    genFindState(&state,true);
    if (!state.isValid()) {
        return;
    }

    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
    if (ed) {
        findHelper(ed,&state);
    } else {
        QTextBrowser *ed = LiteApi::findExtensionObject<QTextBrowser*>(editor,"LiteApi.QTextBrowser");
        if (ed) {
            findHelper(ed,&state);
        }
    }
}

QTextCursor FindEditor::findEditor(QTextDocument *doc, const QTextCursor &cursor, FindState *state)
{
    QString text = m_findEdit->text();
    QTextDocument::FindFlags flags = 0;
    if (state->backWard) {
        flags |= QTextDocument::FindBackward;
    }
    if (state->matchCase) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (state->matchWord) {
        flags |= QTextDocument::FindWholeWords;
    }
    QTextCursor find;
    if (state->useRegexp) {
        find = doc->find(QRegExp(text),cursor,flags);
    } else {
        find = doc->find(text,cursor,flags);
    }
    return find;
}

