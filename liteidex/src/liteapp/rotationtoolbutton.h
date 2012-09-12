#ifndef ROTATIONTOOLBUTTON_H
#define ROTATIONTOOLBUTTON_H

#include <QToolButton>

class RotationToolButton : public QToolButton
{
    Q_OBJECT
public:
    enum Rotation {
        NoRotation  = 0,
        UpsideDown  = 180,
        Clockwise  = 90,
        CounterClockwise = 270
    };
    explicit RotationToolButton(QWidget *parent = 0);
    void setRotation(Rotation rotation);
    Rotation rotation() const;
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
protected:
    void paintEvent(QPaintEvent *);
protected:
    Rotation  rot;
};

#endif // ROTATIONTOOLBUTTON_H
