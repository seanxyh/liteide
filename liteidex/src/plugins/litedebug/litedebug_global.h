#ifndef LITEDEBUG_GLOBAL_H
#define LITEDEBUG_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LITEDEBUG_LIBRARY)
#  define LITEDEBUGSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LITEDEBUGSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LITEDEBUG_GLOBAL_H
