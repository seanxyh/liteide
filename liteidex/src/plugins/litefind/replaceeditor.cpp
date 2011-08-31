#include "replaceeditor.h"
#include "liteapi/litefindobj.h"
#include <QGridLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QDebug>

ReplaceEditor::ReplaceEditor(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_widget(new QWidget)
{
    m_findEdit = new QLineEdit;
    m_replaceEdit = new QLineEdit;
    QPushButton *findNext = new QPushButton(tr("Find Next"));
    QPushButton *replace = new QPushButton(tr("Replace"));
    QPushButton *replaceAll = new QPushButton(tr("Replace All"));

    m_matchWordCheckBox = new QCheckBox(tr("Match whole word only"));
    m_matchCaseCheckBox = new QCheckBox(tr("Match case"));
    m_useRegexCheckBox = new QCheckBox(tr("Regular expression"));
    m_wrapAroundCheckBox = new QCheckBox(tr("Wrap around"));

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
    layout->addWidget(new QLabel(tr("Replace With:")),1,0);
    layout->addWidget(m_replaceEdit,1,1);
    layout->addWidget(replace,1,2);
    layout->addWidget(replaceAll,1,3);
    layout->addWidget(new QLabel(tr("Find Option:")),3,0);
    layout->addLayout(optLayout,3,1,1,3);

    m_widget->setLayout(layout);

    connect(m_findEdit,SIGNAL(returnPressed()),this,SLOT(findNext()));
    connect(findNext,SIGNAL(clicked()),this,SLOT(findNext()));
    connect(replace,SIGNAL(clicked()),this,SLOT(replace()));
    connect(replaceAll,SIGNAL(clicked()),this,SLOT(replaceAll()));
}

ReplaceEditor::~ReplaceEditor()
{
    if (m_widget) {
        delete m_widget;
    }
}

QWidget *ReplaceEditor::widget()
{
    return m_widget;
}

void ReplaceEditor::setReady(const QString &findText)
{
    m_widget->setFocus();
    if (!findText.isEmpty()) {
        m_findEdit->setText(findText);
    }
    if (m_findEdit->text().isEmpty()) {
        m_findEdit->setFocus();
    } else {
        m_replaceEdit->setFocus();
    }
}

void ReplaceEditor::genFindState(ReplaceState *state, bool backWard)
{
    state->findText = m_findEdit->text();
    state->replaceText = m_replaceEdit->text();
    state->matchWord = m_matchWordCheckBox->isChecked();
    state->matchCase = m_matchCaseCheckBox->isChecked();
    state->useRegexp = m_useRegexCheckBox->isChecked();
    state->wrapAround = m_wrapAroundCheckBox->isChecked();
    state->backWard = backWard;
}

QTextCursor ReplaceEditor::findEditor(QTextDocument *doc, const QTextCursor &cursor, ReplaceState *state)
{
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
        find = doc->find(QRegExp(state->findText),cursor,flags);
    } else {
        find = doc->find(state->findText,cursor,flags);
    }
    return find;
}


void ReplaceEditor::findNext()
{
    ReplaceState state;
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
        QTextEdit *ed = LiteApi::findExtensionObject<QTextEdit*>(editor,"LiteApi.QTextEdit");
        if (ed) {
            findHelper(ed,&state);
        }
    }
}

void ReplaceEditor::replace()
{
    ReplaceState state;
    genFindState(&state,false);
    if (!state.isValid()) {
        return;
    }

    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
    if (ed && !ed->isReadOnly()) {
        replaceHelper(ed,&state,1);
    } else {
        QTextEdit *ed = LiteApi::findExtensionObject<QTextEdit*>(editor,"LiteApi.QTextEdit");
        if (ed && !ed->isReadOnly()) {
            replaceHelper(ed,&state,1);
        }
    }
}

void ReplaceEditor::replaceAll()
{
    ReplaceState state;
    genFindState(&state,false);
    if (!state.isValid()) {
        return;
    }

    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (!editor) {
        return;
    }
    QPlainTextEdit *ed = LiteApi::findExtensionObject<QPlainTextEdit*>(editor,"LiteApi.QPlainTextEdit");
    if (ed && !ed->isReadOnly()) {
        replaceHelper(ed,&state,-1);
    } else {
        QTextEdit *ed = LiteApi::findExtensionObject<QTextEdit*>(editor,"LiteApi.QTextEdit");
        if (ed && !ed->isReadOnly()) {
            replaceHelper(ed,&state,-1);
        }
    }
}
