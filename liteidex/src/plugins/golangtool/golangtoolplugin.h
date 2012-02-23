#ifndef GOLANGTOOLPLUGIN_H
#define GOLANGTOOLPLUGIN_H

#include "golangtool_global.h"
#include "liteapi/liteapi.h"
#include <QtPlugin>

class GolangToolPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPlugin)
public:
    GolangToolPlugin();
    virtual bool initWithApp(LiteApi::IApplication *app);
    virtual QStringList dependPluginList() const;
};

#endif // GOLANGTOOLPLUGIN_H
