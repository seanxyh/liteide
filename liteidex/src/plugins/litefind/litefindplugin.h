#ifndef LITEFINDPLUGIN_H
#define LITEFINDPLUGIN_H

#include "litefind_global.h"
#include "liteapi/liteapi.h"
#include <QtPlugin>

class FindEditor;
class ReplaceEditor;
class LiteFindPlugin : public LiteApi::IPlugin
{
    Q_OBJECT
    Q_INTERFACES(LiteApi::IPlugin)
public:
    LiteFindPlugin();
    virtual bool initWithApp(LiteApi::IApplication *app);
public slots:
    void find(bool);
    void replace(bool);
protected:
    FindEditor *m_findEditor;
    ReplaceEditor *m_replaceEditor;
    QAction *m_findAct;
    QAction *m_findNextAct;
    QAction *m_findPrevAct;
    QAction *m_replaceAct;
};

#endif // LITEFINDPLUGIN_H
