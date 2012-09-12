#include "tooldockwidget.h"
#include <QAction>
#include <QIcon>
#include <QLabel>
#include <QHBoxLayout>
#include <QVariant>
#include <QMenu>
#include <QToolButton>
#include <QDebug>

ToolDockWidget::ToolDockWidget(QWidget *parent) :
    QDockWidget(parent), current(0)
{
    m_comboBox = new QComboBox;
    m_comboBox->setMinimumContentsLength(2);
    m_comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_toolbar = new QToolBar(this);
    m_toolbar->setContentsMargins(0, 0, 0, 0);
    m_toolbar->setIconSize(QSize(16,16));
    m_toolbar->addWidget(m_comboBox);

    m_closeAct = new QAction(tr("Close"), m_toolbar);
    m_closeAct->setToolTip(tr("Close"));
    m_closeAct->setIcon(QIcon("icon:images/closetool.png"));
    m_toolbar->addAction(m_closeAct);
    connect(m_closeAct,SIGNAL(triggered()),this,SLOT(close()));
    connect(m_comboBox,SIGNAL(activated(int)),this,SLOT(activeComboBoxIndex(int)));

    this->setTitleBarWidget(m_toolbar);


    m_toolbar->setStyleSheet("QToolBar {border:1 ; background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #EEEEEE, stop: 1 #ababab); color : #EEEEEE}");
}

void ToolDockWidget::createMenu(Qt::DockWidgetArea area, bool split)
{
    QMenu *moveMenu = new QMenu(tr("Move To"),this);
    if (area != Qt::TopDockWidgetArea) {
        QAction *act = new QAction(tr("Top"),this);
        act->setData(Qt::TopDockWidgetArea);
        moveMenu->addAction(act);
        connect(act,SIGNAL(triggered()),this,SLOT(moveAction()));
        QAction *act1 = new QAction(tr("Top : Split"),this);
        act1->setData(Qt::TopDockWidgetArea);
        moveMenu->addAction(act1);
        connect(act1,SIGNAL(triggered()),this,SLOT(moveActionSplit()));
    }
    if (area != Qt::BottomDockWidgetArea) {
        QAction *act = new QAction(tr("Bottom"),this);
        act->setData(Qt::BottomDockWidgetArea);
        moveMenu->addAction(act);
        connect(act,SIGNAL(triggered()),this,SLOT(moveAction()));
        QAction *act1 = new QAction(tr("Bottom : Split"),this);
        act1->setData(Qt::BottomDockWidgetArea);
        moveMenu->addAction(act1);
        connect(act1,SIGNAL(triggered()),this,SLOT(moveActionSplit()));
    }
    if (area != Qt::LeftDockWidgetArea) {
        QAction *act = new QAction(tr("Left"),this);
        act->setData(Qt::LeftDockWidgetArea);
        moveMenu->addAction(act);
        connect(act,SIGNAL(triggered()),this,SLOT(moveAction()));
        QAction *act1 = new QAction(tr("Left : Split"),this);
        act1->setData(Qt::LeftDockWidgetArea);
        moveMenu->addAction(act1);
        connect(act1,SIGNAL(triggered()),this,SLOT(moveActionSplit()));
    }
    if (area != Qt::RightDockWidgetArea) {
        QAction *act = new QAction(tr("Right"),this);
        act->setData(Qt::RightDockWidgetArea);
        moveMenu->addAction(act);
        connect(act,SIGNAL(triggered()),this,SLOT(moveAction()));
        QAction *act1 = new QAction(tr("Right : Split"),this);
        act1->setData(Qt::RightDockWidgetArea);
        moveMenu->addAction(act1);
        connect(act1,SIGNAL(triggered()),this,SLOT(moveActionSplit()));
    }

    QMenu *menu = new QMenu;
    if (split) {
        QAction *unsplitAct = new QAction(tr("UnSplit"),this);
        unsplitAct->setData(area);
        connect(unsplitAct,SIGNAL(triggered()),this,SLOT(unsplitAction()));
        menu->addAction(unsplitAct);
    } else {
        QAction *splitAct = new QAction(tr("Split"),this);
        splitAct->setData(area);
        connect(splitAct,SIGNAL(triggered()),this,SLOT(splitAction()));
        menu->addAction(splitAct);
    }
    menu->addAction(moveMenu->menuAction());

    QToolButton *btn = new QToolButton;
    btn->setPopupMode(QToolButton::InstantPopup);
    btn->setIcon(QIcon("icon:images/movemenu.png"));
    btn->setMenu(menu);

    m_toolbar->insertWidget(m_closeAct,btn);
}

void ToolDockWidget::moveAction()
{
    QAction *action = static_cast<QAction*>(sender());
    if (!action) {
        return;
    }
    Qt::DockWidgetArea area = (Qt::DockWidgetArea)action->data().toInt();
    emit moveActionTo(area,current,false);
}

void ToolDockWidget::moveActionSplit()
{
    QAction *action = static_cast<QAction*>(sender());
    if (!action) {
        return;
    }
    Qt::DockWidgetArea area = (Qt::DockWidgetArea)action->data().toInt();
    emit moveActionTo(area,current,true);
}

void ToolDockWidget::splitAction()
{
    QAction *action = static_cast<QAction*>(sender());
    if (!action) {
        return;
    }
    Qt::DockWidgetArea area = (Qt::DockWidgetArea)action->data().toInt();
    emit moveActionTo(area,current,true);
}

void ToolDockWidget::unsplitAction()
{
    QAction *action = static_cast<QAction*>(sender());
    if (!action) {
        return;
    }
    Qt::DockWidgetArea area = (Qt::DockWidgetArea)action->data().toInt();
    emit moveActionTo(area,current,false);
}


void ToolDockWidget::setWindowTitle(const QString &text)
{
    QDockWidget::setWindowTitle(text);
}

QAction *ToolDockWidget::addWidget(QWidget *widget)
{
   return m_toolbar->insertWidget(m_closeAct,widget);
}

void ToolDockWidget::setToolMenu(QMenu *menu)
{
    QToolButton *btn = new QToolButton;
    btn->setPopupMode(QToolButton::InstantPopup);
    btn->setMenu(menu);

    m_toolbar->insertWidget(m_closeAct,btn);
}

QList<QAction *> ToolDockWidget::actions() const
{
    return m_actions;
}

void ToolDockWidget::removeAction(QAction *action)
{
    if (m_actions.removeAll(action)) {
        if (action == current)
            current = 0;
        int index = m_comboBox->findData((int)action);
        if (index >= 0) {
            m_comboBox->removeItem(index);
        }
        QObject::disconnect(action, SIGNAL(changed()), this, SLOT(actionChanged()));
    }
}

QAction * ToolDockWidget::checkedAction () const
{
    return current;
}

void ToolDockWidget::actionChanged()
{
    QAction *action = qobject_cast<QAction*>(sender());
    Q_ASSERT_X(action != 0, "ActionGroup::actionChanged", "internal error");
    if (action->isChecked()) {
        if (action != current) {
            if(current)
                current->setChecked(false);
            current = action;
            int index = m_comboBox->findData((int)action);
            if (index >= 0) {
                m_comboBox->setCurrentIndex(index);
            }
        }
    } else if (action == current) {
        current = 0;
    }
}

void ToolDockWidget::activeComboBoxIndex(int index)
{
    if (index < 0 || index >= m_comboBox->count()) {
        return;
    }
    bool ok;
    QAction *action =(QAction*)m_comboBox->itemData(index).toInt(&ok);
    if (action && !action->isChecked()) {
        action->setChecked(true);
    }
}

void ToolDockWidget::addAction(QAction *action, const QString &title)
{
    if(!m_actions.contains(action)) {
        m_actions.append(action);
        m_comboBox->addItem(title,QVariant((int)action));
        QObject::connect(action, SIGNAL(changed()), this, SLOT(actionChanged()));
    }
    if (current && current->isChecked()) {
        current->setChecked(false);
    }
    if (action->isChecked()) {
        current = action;
    }
}


