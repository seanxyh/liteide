#ifndef GDBDEBUGERPLUGIN_H
#define GDBDEBUGERPLUGIN_H

#include "gdbdebuger_global.h"
#include "liteapi/liteapi.h"
#include <QtPlugin>

class GdbDebugerPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPlugin)
public:
    GdbDebugerPlugin();
    virtual bool initWithApp(LiteApi::IApplication *app);
    virtual QStringList dependPluginList() const;
};

#endif // GDBDEBUGERPLUGIN_H
