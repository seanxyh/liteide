#ifndef LITEDEBUGPLUGIN_H
#define LITEDEBUGPLUGIN_H

#include "litedebug_global.h"
#include "liteapi/liteapi.h"
#include <QtPlugin>

class LiteDebugPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPlugin)
public:
    LiteDebugPlugin();
    virtual bool initWithApp(LiteApi::IApplication *app);
};

#endif // LITEDEBUGPLUGIN_H
