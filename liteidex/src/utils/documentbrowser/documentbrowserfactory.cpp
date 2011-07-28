#include "documentbrowserfactory.h"
#include "documentbrowser.h"
#include <QFile>
#include <QTextBrowser>
#include <QFileInfo>

DocumentBrowserFactory::DocumentBrowserFactory(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IEditorFactory(parent),
    m_liteApp(app)
{
    m_mimeTypes << "liteide/x-browser";
}

QStringList DocumentBrowserFactory::mimeTypes() const
{
    return m_mimeTypes;
}

LiteApi::IEditor *DocumentBrowserFactory::open(const QString &fileName, const QString &mimeType)
{
    if (!m_mimeTypes.contains(mimeType)) {
        return 0;
    }
    DocumentBrowser *browser = new DocumentBrowser(m_liteApp,0);
    if (browser->open(fileName,mimeType)) {
        return browser;
    }
    return 0;
}
