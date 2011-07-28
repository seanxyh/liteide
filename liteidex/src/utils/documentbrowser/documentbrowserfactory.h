#ifndef DOCUMENTBROWSERFACTORY_H
#define DOCUMENTBROWSERFACTORY_H

#include "liteapi.h"

class DocumentBrowserFactory : public LiteApi::IEditorFactory
{
public:
    DocumentBrowserFactory(LiteApi::IApplication *app, QObject *parent);
    virtual QStringList mimeTypes() const;
    virtual LiteApi::IEditor *open(const QString &fileName, const QString &mimeType);
protected:
    LiteApi::IApplication *m_liteApp;
    QStringList     m_mimeTypes;
};

#endif // DOCUMENTBROWSERFACTORY_H
