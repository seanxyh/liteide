#include "litedebugplugin.h"

LiteDebugPlugin::LiteDebugPlugin()
{
    m_info->setId("plugin/LiteDebug");
    m_info->setName("LiteDebug");
    m_info->setAnchor("visualfc");
    m_info->setInfo("LiteIDE Debug Plugin");
}

bool LiteDebugPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }
    return true;
}

Q_EXPORT_PLUGIN(LiteDebugPlugin)
