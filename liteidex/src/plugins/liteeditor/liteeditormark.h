#ifndef LITEEDITORMARK_H
#define LITEEDITORMARK_H

#include <QIcon>

class LiteEditorMark : public QObject
{
    Q_OBJECT
public:
    explicit LiteEditorMark(QObject *parent = 0);
    virtual void paint(QPainter *painter, const QRect &rect) const;
    virtual void setIcon(const QIcon &icon);
    virtual void setName(const QString &name);
    QString name() const;
protected:
    QIcon   m_icon;
    QString m_name;
};

#endif // LITEEDITORMARK_H
