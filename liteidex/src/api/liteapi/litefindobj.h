#ifndef LITEFINDOBJ_H
#define LITEFINDOBJ_H

#include "liteobj.h"

namespace LiteApi {

template <typename T>
inline T findExtensionObject(IObject *obj, const QString & meta)
{
    IExtension *ext = obj->extension();
    if (!ext) {
        return 0;
    }
    QObject *t = ext->findObject(meta);
    if (!t) {
        return 0;
    }
    return static_cast<T>(t);
}


template <typename T>
inline T findExtensionObject(IExtension *ext, const QString & meta)
{
    QObject *t = ext->findObject(meta);
    if (!t) {
        return 0;
    }
    return static_cast<T>(t);
}

} //namespace LiteApi

#endif
