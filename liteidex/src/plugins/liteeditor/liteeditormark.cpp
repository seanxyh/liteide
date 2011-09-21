#include "liteeditormark.h"


LiteEditorMarkTypeManager::LiteEditorMarkTypeManager(QObject *parent) :
    LiteApi::IEditorMarkTypeManager(parent)
{
}

void LiteEditorMarkTypeManager::registerMark(int type, const QIcon &icon)
{
    m_typeIconMap.insert(type,icon);
}

QList<int> LiteEditorMarkTypeManager::markTypeList() const
{
    return m_typeIconMap.keys();
}

QIcon LiteEditorMarkTypeManager::markIcon(int type) const
{
    return m_typeIconMap.value(type);
}

LiteEditorMark::LiteEditorMark(LiteApi::IEditorMarkTypeManager *manager, QObject *parent) :
    LiteApi::IEditorMark(parent),
    m_manager(manager)
{
}
/*
void LiteEditorMark::paint(QPainter *painter, const QRect &rect) const
{
    m_icon.paint(painter, rect, Qt::AlignCenter);
}
*/

void LiteEditorMark::addMark(int line, int type)
{
    QMap<int, QList<int> >::iterator it = m_numberMarkTypesMap.find(line);
    if (it == m_numberMarkTypesMap.end()) {
        m_numberMarkTypesMap.insert(line,QList<int>() << type);
    } else {
        if (!it.value().contains(type)) {
            it.value().append(type);
            qSort(it.value());
        }
    }
}

void LiteEditorMark::removeMark(int line, int type)
{
    QMap<int, QList<int> >::iterator it = m_numberMarkTypesMap.find(line);
    if (it != m_numberMarkTypesMap.end()) {
        it.value().removeOne(type);
    }
}

QList<int> LiteEditorMark::markNumberList() const
{
    return m_numberMarkTypesMap.keys();
}

QList<int> LiteEditorMark::markTypeList(int line) const
{
    return m_numberMarkTypesMap.value(line);
}

void LiteEditorMark::paint(QPainter *painter, int blockNumber, int x, int y, int w, int h) const
{
    const int line = blockNumber + 1;
    QMap<int, QList<int> >::const_iterator it = m_numberMarkTypesMap.find(line);
    if (it != m_numberMarkTypesMap.end()) {
        int offset = x;
        foreach(int type, it.value()) {
            const QIcon &icon = m_manager->markIcon(type);
            icon.paint(painter,offset,y,w,h);
            offset += 2;
        }
    }
}

