#include "liteeditormark.h"

LiteEditorMark::LiteEditorMark(QObject *parent) :
    QObject(parent)
{
}

void LiteEditorMark::paint(QPainter *painter, const QRect &rect) const
{
    m_icon.paint(painter, rect, Qt::AlignCenter);
}

void LiteEditorMark::setIcon(const QIcon &icon)
{
    m_icon = icon;
}

void LiteEditorMark::setName(const QString &name)
{
    m_name = name;
}

QString LiteEditorMark::name() const
{
    return m_name;
}
