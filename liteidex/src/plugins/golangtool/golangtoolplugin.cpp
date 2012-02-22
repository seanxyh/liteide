#include "golangtoolplugin.h"

GolangToolPlugin::GolangToolPlugin()
{
    m_info->setId("plugin/GolangTool");
    m_info->setName("GolangTool");
    m_info->setAnchor("visualfc");
    m_info->setInfo("Golang Tool Plugin");
}

bool GolangToolPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }
    return true;
}

Q_EXPORT_PLUGIN(GolangToolPlugin)
