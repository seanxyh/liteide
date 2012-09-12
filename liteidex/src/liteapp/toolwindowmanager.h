#ifndef TOOLWINDOWMANAGER_H
#define TOOLWINDOWMANAGER_H

#include "liteapi/liteapi.h"

using namespace LiteApi;

class ToolWindowManager : public IToolWindowManager
{
public:
    virtual QAction *addToolWindow(Qt::DockWidgetArea area, QWidget *widget, const QString &id, const QString &title, bool split);
    virtual void moveToolWindow(Qt::DockWidgetArea area,QAction *action, bool split);
    virtual QAction *findToolWindow(QWidget *widget);
};

#endif // TOOLWINDOWMANAGER_H
