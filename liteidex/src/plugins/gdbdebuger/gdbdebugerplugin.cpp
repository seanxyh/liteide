#include "gdbdebugerplugin.h"

GdbDebugerPlugin::GdbDebugerPlugin()
{
    m_info->setId("plugin/GdbDebuger");
    m_info->setName("GdbDebuger");
    m_info->setAnchor("visualfc");
    m_info->setInfo("Gdb Debuger Plugin");
}

bool GdbDebugerPlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }
    return true;
}

QStringList GdbDebugerPlugin::dependPluginList() const
{
    return QStringList() << "plugin/litedebug";
}

Q_EXPORT_PLUGIN(GdbDebugerPlugin)
