#include "liteeditormark.h"


LiteEditorMarkTypeManager::LiteEditorMarkTypeManager(QObject *parent) :
    LiteApi::IEditorMarkTypeManager(parent)
{
}

void LiteEditorMarkTypeManager::registerMark(int type, int priority, const QIcon &icon)
{
    m_typeIconMap.insert(type,icon);
    m_typePrioMap.insert(type,priority);
}

QList<int> LiteEditorMarkTypeManager::markTypeList() const
{
    return m_typeIconMap.keys();
}

QIcon LiteEditorMarkTypeManager::markIcon(int type) const
{
    return m_typeIconMap.value(type);
}

int LiteEditorMarkTypeManager::markPriority(int type) const
{
    return m_typePrioMap.value(type);
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

void LiteEditorMark::addMark(int number, int type)
{
    QMap<int, QList<int> >::iterator it = m_numberMarkTypesMap.find(number);
    if (it == m_numberMarkTypesMap.end()) {
        m_numberMarkTypesMap.insert(number,QList<int>() << type);
    } else {
        if (!it.value().contains(type)) {
            it.value().append(type);
            qSort(it.value());
        }
    }
}

void LiteEditorMark::removeMark(int number, int type)
{
    QMap<int, QList<int> >::iterator it = m_numberMarkTypesMap.find(number);
    if (it != m_numberMarkTypesMap.end()) {
        it.value().removeOne(type);
    }
}

QList<int> LiteEditorMark::markNumberList() const
{
    return m_numberMarkTypesMap.keys();
}

QList<int> LiteEditorMark::markTypeList(int number) const
{
    return m_numberMarkTypesMap.value(number);
}

void LiteEditorMark::paint(QPainter *painter, int number, int x, int y, int w, int h) const
{
    QMap<int, QList<int> >::const_iterator it = m_numberMarkTypesMap.find(number);
    if (it != m_numberMarkTypesMap.end()) {
        int offset = x;
        foreach(int type, it.value()) {
            const QIcon &icon = m_manager->markIcon(type);
            icon.paint(painter,offset,y,w,h);
            offset += 2;
        }
    }
}

