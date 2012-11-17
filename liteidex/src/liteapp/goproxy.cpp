/**************************************************************************
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE Team. All rights reserved.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** In addition, as a special exception,  that plugins developed for LiteIDE,
** are allowed to remain closed sourced and can be distributed under any license .
** These rights are included in the file LGPL_EXCEPTION.txt in this package.
**
**************************************************************************/
// Module: goproxy.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2012-11-17
// $Id: goproxy.cpp,v 1.0 2012-11-17 visualfc Exp $

#include "goproxy.h"
#include <QDebug>
//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

typedef void (*DRV_CALLBACK)(char *id, char *reply, int len, void *ctx);
typedef int (*GODRV_CALL)(char* id,int id_size, char* args, int args_size, DRV_CALLBACK cb, void *ctx);

static GODRV_CALL godrv_call_fn = 0;

static int godrv_call(const QByteArray &id, const QByteArray &args, DRV_CALLBACK cb, void *ctx)
{
    if (godrv_call_fn == 0) {
        return -1;
    }
    return godrv_call_fn((char*)id.constData(),id.length(),(char*)args.constData(),args.length(),cb,ctx);
}

static void cdrv_callback(char *id, char *reply, int len, void *ctx)
{
    ((GoProxy*)(ctx))->callback(id,reply,len);
}

GoProxy::GoProxy(QObject *parent) :
    QObject(parent)
{
}

bool GoProxy::hasProxy()
{
    return godrv_call_fn != 0;
}

void GoProxy::call(const QByteArray &id, const QByteArray &args)
{    
    godrv_call(id,args,&cdrv_callback,this);
}

void GoProxy::callback(char *id, char *reply, int len)
{
    emit done(id,QByteArray(reply,len));
}

extern "C"
void LITEIDESHARED_EXPORT cdrv_init(void *fn)
{
    godrv_call_fn = (GODRV_CALL)fn;
}

extern "C"
void LITEIDESHARED_EXPORT cdrv_cb(DRV_CALLBACK cb, char *id, char *reply, int size, void* ctx)
{
    cb(id,reply,size,ctx);
}