#ifndef TOOLDOCKWIDGET_H
#define TOOLDOCKWIDGET_H

#include <QDockWidget>
#include <QToolBar>
#include <QComboBox>
#include <QPointer>
#include <QAction>

class QLabel;

class ToolDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit ToolDockWidget(QWidget *parent = 0);
    QAction *addWidget(QWidget *widget);
    void setWindowTitle(const QString &text);
    QList<QAction *> actions() const;
    void addAction(QAction *act, const QString &title);
    void removeAction(QAction *act);
    QAction * checkedAction () const;
    void setToolMenu(QMenu *menu);
    void createMenu(Qt::DockWidgetArea area, bool split);
signals:
    void moveActionTo(Qt::DockWidgetArea, QAction*, bool);
protected slots:
    void moveAction();
    void splitAction();
    void unsplitAction();
    void moveActionSplit();
    void actionChanged();
    void activeComboBoxIndex(int);
protected:
    Qt::DockWidgetArea area;
    QToolBar *m_toolbar;
    QLabel   *m_titleLabel;
    QComboBox *m_comboBox;
    QAction  *m_closeAct;
    QList<QAction*> m_actions;
    QPointer<QAction> current;
};

#endif // TOOLDOCKWIDGET_H
