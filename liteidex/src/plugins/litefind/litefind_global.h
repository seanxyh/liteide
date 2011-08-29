#ifndef LITEFIND_GLOBAL_H
#define LITEFIND_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LITEFIND_LIBRARY)
#  define LITEFINDSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LITEFINDSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LITEFIND_GLOBAL_H
