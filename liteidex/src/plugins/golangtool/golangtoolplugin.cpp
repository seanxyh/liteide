#include "golangtoolplugin.h"
#include "gopathbrowser.h"

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
    new GopathBrowser(app,this);
    return true;
}

QStringList GolangToolPlugin::dependPluginList() const
{
    return QStringList() << "plugin/liteenv";
}


Q_EXPORT_PLUGIN(GolangToolPlugin)
