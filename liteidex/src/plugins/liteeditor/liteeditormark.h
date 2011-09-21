#ifndef LITEEDITORMARK_H
#define LITEEDITORMARK_H

#include "liteapi/liteapi.h"
#include "liteeditorapi/liteeditorapi.h"

class LiteEditorMarkTypeManager : public LiteApi::IEditorMarkTypeManager
{
    Q_OBJECT
public:
    LiteEditorMarkTypeManager(QObject *parent = 0);
    virtual void registerMark(int type, int priority, const QIcon &icon);
    virtual QList<int> markTypeList() const;
    virtual QIcon markIcon(int type) const;
    virtual int markPriority(int typ) const;
protected:
    QMap<int,int>   m_typePrioMap;
    QMap<int,QIcon> m_typeIconMap;
};

class LiteEditorMark : public LiteApi::IEditorMark
{
    Q_OBJECT
public:
    explicit LiteEditorMark(LiteApi::IEditorMarkTypeManager *manager, QObject *parent = 0);
    virtual void addMark(int number, int type);
    virtual void removeMark(int number, int type);
    virtual QList<int> markNumberList() const;
    virtual QList<int> markTypeList(int number) const;
    virtual void paint(QPainter *painter, int number, int x, int y, int w, int h) const;
protected:
    LiteApi::IEditorMarkTypeManager *m_manager;
    QMap<int,QList<int> > m_numberMarkTypesMap;    
};

#endif // LITEEDITORMARK_H
