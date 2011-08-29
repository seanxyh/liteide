#ifndef LITEFINDPLUGIN_H
#define LITEFINDPLUGIN_H

#include "litefind_global.h"
#include "liteapi/liteapi.h"
#include <QtPlugin>

class LiteFindPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPlugin)
public:
    LiteFindPlugin();
    virtual bool initWithApp(LiteApi::IApplication *app);
};

#endif // LITEFINDPLUGIN_H
