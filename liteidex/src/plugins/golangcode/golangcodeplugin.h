#ifndef GOLANGCODEPLUGIN_H
#define GOLANGCODEPLUGIN_H

#include "golangcode_global.h"
#include "liteapi.h"
#include "litebuildapi/litebuildapi.h"
#include <QtPlugin>

class GolangCode;
class GolangCodePlugin : public LiteApi::IPlugin
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPlugin)
public:
    GolangCodePlugin();
    virtual bool initWithApp(LiteApi::IApplication *app);
protected slots:
    void currentEditorChanged(LiteApi::IEditor*);
    void buildChanged(LiteApi::IBuild*);
protected:
    GolangCode *m_code;
};

#endif // GOLANGCODEPLUGIN_H
