#include "litefindplugin.h"

LiteFindPlugin::LiteFindPlugin()
{
    m_info->setId("plugin/LiteFind");
    m_info->setName("LiteFind");
    m_info->setAnchor("visualfc");
    m_info->setInfo("LiteIDE Find Plugin");
}

bool LiteFindPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }
    return true;
}

Q_EXPORT_PLUGIN(LiteFindPlugin)
