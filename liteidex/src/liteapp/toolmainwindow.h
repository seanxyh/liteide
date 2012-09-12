#ifndef TOOLMAINWINDOW_H
#define TOOLMAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QPointer>
#include <QAction>

class ActionGroup;
class QSplitter;
class RotationToolButton;
class ToolDockWidget;

class ActionGroup : public QObject
{
    Q_OBJECT
public:
    ActionGroup(QObject *parent);
    QList<QAction *> actions() const;
    void addAction(QAction *act);
    void removeAction(QAction *act);
    QAction * checkedAction () const;
protected slots:
    void actionChanged();
protected:
    QList<QAction *> m_actions;
    QPointer<QAction> current;
};

class ActionToolBar : public QObject
{
    Q_OBJECT
public:
    ActionToolBar(QObject *parent, Qt::DockWidgetArea area);
    ToolDockWidget *dock(bool split) const;
    void addAction(QAction *action, const QString &title, bool split);
    void removeAction(QAction *action, bool split);
    void setHideToolBar(bool b);
signals:
    void moveActionTo(Qt::DockWidgetArea,QAction*,bool);
protected slots:
    void dock1Visible(bool);
    void dock2Visible(bool);
public:
    Qt::DockWidgetArea area;
    QToolBar *toolBar;
    QAction  *spacerAct;
    ToolDockWidget *dock1;
    ToolDockWidget *dock2;
    QMap<QAction*,QWidget*> m_actionWidgetMap;
    bool bHideToolBar;
};

struct ActionState
{
    QWidget *widget;
    Qt::DockWidgetArea area;
    bool    split;
    QString id;
    QString  title;
};

struct InitToolSate
{
    Qt::DockWidgetArea area;
    bool               split;
};

class ToolMainWindow : public QMainWindow
{
    Q_OBJECT    
public:
    ToolMainWindow(QWidget *parent = 0);
    ~ToolMainWindow();
    void removeAllToolWindows();
    QAction *addToolWindow(Qt::DockWidgetArea area, QWidget *widget, const QString &id, const QString &title, bool split = false);
    void removeToolWindow(QAction *action);
    QAction *findToolWindow(QWidget *wiget);
    QByteArray saveToolState(int version = 0) const;
    bool loadInitToolState(const QByteArray &state, int version = 0);
public slots:
    void hideAllToolWindows();
    void restoreToolWindows();
    void lockToolWindows(bool b);
    void moveToolWindow(Qt::DockWidgetArea area, QAction *action,bool split = false);    
protected slots:
    void toggledAction(bool);
protected:
    QMap<Qt::DockWidgetArea,ActionToolBar*> m_areaToolBar;
    QMap<QAction*,ActionState*> m_actStateMap;
    QMap<QString,InitToolSate> m_idStateMap;
    QStatusBar  *m_statusBar;
    QAction     *m_dockLockAct;
    QList<QAction*> m_hideActionList;
};

#endif // TOOLMAINWINDOW_H
