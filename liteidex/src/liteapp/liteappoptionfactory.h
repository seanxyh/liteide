#ifndef LITEAPPOPTIONFACTORY_H
#define LITEAPPOPTIONFACTORY_H

#include "liteapi.h"

class LiteAppOptionFactory : public LiteApi::IOptionFactory
{
public:
    LiteAppOptionFactory(LiteApi::IApplication *app, QObject *parent);
    virtual QStringList mimeTypes() const;
    virtual LiteApi::IOption *create(const QString &mimeType);
protected:
    LiteApi::IApplication *m_liteApp;
};

#endif // LITEAPPOPTIONFACTORY_H
