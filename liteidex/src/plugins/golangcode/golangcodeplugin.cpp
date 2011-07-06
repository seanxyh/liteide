#include "golangcodeplugin.h"
#include "litefindobj.h"
#include "liteeditorapi/liteeditorapi.h"
#include "fileutil/fileutil.h"
#include "golangcode.h"
#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

GolangCodePlugin::GolangCodePlugin()
{
    m_info->setId("plugin/GolangCode");
    m_info->setName("GolangCode");
    m_info->setAnchor("visualfc");
    m_info->setInfo("Gocode Helper Plugin");
}

bool GolangCodePlugin::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IPlugin::initWithApp(app)) {
        return false;
    }

    LiteApi::IBuildManager *manager = LiteApi::findExtensionObject<LiteApi::IBuildManager*>(m_liteApp,"LiteApi.IBuildManager");
    if (manager) {
        connect(manager,SIGNAL(buildChanged(LiteApi::IBuild*)),this,SLOT(buildChanged(LiteApi::IBuild*)));
    }

    m_code = new GolangCode(app,this);
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));
    return true;
}

void GolangCodePlugin::buildChanged(LiteApi::IBuild *build)
{
    m_code->setBuild(build);
}

void GolangCodePlugin::currentEditorChanged(LiteApi::IEditor *editor)
{
    QString mimeType;
    if (editor) {
        LiteApi::IFile *file = editor->file();
        if (file) {
            mimeType = file->mimeType();
        }
    }
    if (mimeType == "text/x-gosrc") {
        LiteApi::ICompleter *completer = LiteApi::findExtensionObject<LiteApi::ICompleter*>(editor,"LiteApi.ICompleter");
        m_code->setCompleter(completer);
    } else {
        m_code->setCompleter(0);
    }
}

Q_EXPORT_PLUGIN(GolangCodePlugin)
