#include "katehighlighter.h"
#include "generichighlighter/highlighter.h"
#include "generichighlighter/highlightdefinition.h"
#include "generichighlighter/highlightdefinitionhandler.h"
#include "generichighlighter/highlighterexception.h"
#include "generichighlighter/highlightdefinitionmetadata.h"
#include "generichighlighter/manager2.h"
#include "texteditorconstants.h"

#include <QDir>
#include <QFileInfo>
#include <QXmlStreamReader>

using namespace TextEditor::Internal;
using namespace TextEditor::Constants;
using namespace TextEditor;

KateHighlighter::KateHighlighter(QObject *parent) :
    QObject(parent)
{
}


void KateHighlighter::loadPath(const QString &definitionsPaths)
{
    Manager2::instance()->loadPath(QStringList(definitionsPaths));
}

QStringList KateHighlighter::mimeTypes() const
{
    return Manager2::instance()->mimeTypes();
}

QString KateHighlighter::mimeTypeName(const QString &mimeType) const
{
    QString id = Manager2::instance()->definitionIdByMimeType(mimeType);
    QSharedPointer<HighlightDefinitionMetaData> data = Manager2::instance()->definitionMetaData(id);
    if (data) {
        return data->name();
    }
    return QString();
}

QStringList KateHighlighter::mimeTypePatterns(const QString &mimeType) const
{
    QString id = Manager2::instance()->definitionIdByMimeType(mimeType);
    QSharedPointer<HighlightDefinitionMetaData> data = Manager2::instance()->definitionMetaData(id);
    if (data) {
        return data->patterns();
    }
    return QStringList();
}

void KateHighlighter::reset(TextEditor::SyntaxHighlighter *h)
{
    Highlighter *highlighter = static_cast<Highlighter*>(h);
    if (!highlighter) {
        return;
    }
   QTextCharFormat fmt;
    fmt.setForeground(Qt::lightGray);
    highlighter->configureFormat(Highlighter::VisualWhitespace, fmt);

    fmt.setForeground(Qt::darkBlue);
    fmt.setFontWeight(QFont::Bold);
    highlighter->configureFormat(Highlighter::Keyword, fmt);

    fmt.setFontWeight(QFont::Normal);
    fmt.setForeground(Qt::darkBlue);//Qt::darkMagenta);
    highlighter->configureFormat(Highlighter::DataType, fmt);

    fmt.setForeground(Qt::blue);
    highlighter->configureFormat(Highlighter::Function,fmt);

    fmt.setForeground(Qt::darkGreen);
    highlighter->configureFormat(Highlighter::Comment, fmt);

    fmt.setForeground(Qt::darkMagenta);
    // Using C_NUMBER for all kinds of numbers.
    highlighter->configureFormat(Highlighter::Decimal, fmt);
    highlighter->configureFormat(Highlighter::BaseN, fmt);
    highlighter->configureFormat(Highlighter::Float, fmt);
    fmt.setForeground(Qt::darkGreen);

    // Using C_STRING for strings and chars.
    highlighter->configureFormat(Highlighter::Char, fmt);
    highlighter->configureFormat(Highlighter::String, fmt);
    fmt.setForeground(Qt::red);
    highlighter->configureFormat(Highlighter::Error,fmt);
    //highlighter->rehighlight();
}

TextEditor::SyntaxHighlighter *KateHighlighter::create(QTextDocument *doc, const QString &mimeType)
{
    Highlighter *h = new Highlighter(doc);
     reset(h);
     /*
    QTextCharFormat fmt;
    fmt.setForeground(Qt::red);

    h->configureFormat(Highlighter::Keyword,fmt);
    fmt.setForeground(Qt::darkBlue);
    h->configureFormat(Highlighter::String,fmt);
    */
    QString id = Manager2::instance()->definitionIdByMimeType(mimeType);
    QSharedPointer<HighlightDefinition> def = Manager2::instance()->definition(id);
    if (def) {
        h->setDefaultContext(def->initialContext());
    }
    return h;
}
