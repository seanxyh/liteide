#ifndef CDRV_H
#define CDRV_H

#include <QtCore/qglobal.h>

#if defined(LITEAPP_LIBRARY)
#  define LITEIDESHARED_EXPORT Q_DECL_EXPORT
#else
#  define LITEIDESHARED_EXPORT Q_DECL_IMPORT
#endif

typedef void (*DRV_CALLBACK)(char *id, char *reply, int len, int err, void *ctx);
typedef int (*GODRV_CALL)(char* id,int id_size, char* args, int args_size, DRV_CALLBACK cb, void *ctx);

extern "C"
int LITEIDESHARED_EXPORT cdrv_main(int argc, char **argv);

extern "C"
void LITEIDESHARED_EXPORT cdrv_init(void *fn);

extern "C"
void LITEIDESHARED_EXPORT cdrv_cb(DRV_CALLBACK cb, char *id, char *reply, int size, int err, void* ctx);


#endif // CDRV_H
