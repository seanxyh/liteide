#include "toolwindowmanager.h"
#include "toolmainwindow.h"

QAction *ToolWindowManager::addToolWindow(Qt::DockWidgetArea area, QWidget *widget, const QString &id, const QString &title, bool split)
{
    return ((ToolMainWindow*)m_liteApp->mainWindow())->addToolWindow(area,widget,id,title,split);
}

void ToolWindowManager::moveToolWindow(Qt::DockWidgetArea area,QAction *action, bool split)
{
    ((ToolMainWindow*)m_liteApp->mainWindow())->moveToolWindow(area,action,split);
}

QAction *ToolWindowManager::findToolWindow(QWidget *widget)
{
    return ((ToolMainWindow*)m_liteApp->mainWindow())->findToolWindow(widget);
}
