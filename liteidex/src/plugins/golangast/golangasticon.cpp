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
// Module: golangasticon.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: golangasticon.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "golangasticon.h"

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

GolangAstIcon::GolangAstIcon() :
    iconPackage(QIcon(":/images/package.png")),
    iconType(QIcon(":/images/type.png")),
    iconStruct(QIcon(":/images/struct.png")),
    iconInterface(QIcon(":/images/interface.png")),
    iconFunc(QIcon(":/images/func.png")),
    iconFuncs(QIcon(":/images/funcs.png")),
    iconVar(QIcon(":/images/var.png")),
    iconVars(QIcon(":/images/vars.png")),
    iconConst(QIcon(":/images/const.png")),
    iconConsts(QIcon(":/images/consts.png")),
    iconMethod(QIcon(":/images/method.png")),
    iconGofile(QIcon(":/images/gofile.png")),
    iconMakefile(QIcon(":/images/makefile.png")),
    iconProfile(QIcon(":/images/project.png"))
{

}
QIcon GolangAstIcon::iconFromTag(const QString &tag) const
{
    if (tag == "p")
        return iconPackage;
    else if (tag == "t")
        return iconType;
    else if (tag == "i")
        return iconInterface;
    else if (tag == "s")
        return iconStruct;
    else if (tag == "v")
        return iconVar;
    else if (tag == "c")
        return iconConst;
    else if (tag == "v_")
        return iconVars;
    else if (tag == "c_")
        return iconConsts;
    else if (tag == "f")
        return iconFunc;
    else if (tag == "f_")
        return iconFuncs;
    else if (tag == "m")
        return iconMethod;
    else if (tag == "a")
        return iconFunc;
    return QIcon();
}

GolangAstIconPriv::GolangAstIconPriv() :
    iconPackage(QIcon(":/images/package.png")),
    iconType(QIcon(":/images/type_p.png")),
    iconStruct(QIcon(":/images/struct_p.png")),
    iconInterface(QIcon(":/images/interface_p.png")),
    iconFunc(QIcon(":/images/func_p.png")),
    iconFuncs(QIcon(":/images/funcs.png")),
    iconVar(QIcon(":/images/var_p.png")),
    iconVars(QIcon(":/images/vars.png")),
    iconConst(QIcon(":/images/const_p.png")),
    iconConsts(QIcon(":/images/consts.png")),
    iconMethod(QIcon(":/images/method_p.png"))
{

}
QIcon GolangAstIconPriv::iconFromTag(const QString &tag) const
{
    if (tag == "p")
        return iconPackage;
    else if (tag == "t")
        return iconType;
    else if (tag == "i")
        return iconInterface;
    else if (tag == "s")
        return iconStruct;
    else if (tag == "v")
        return iconVar;
    else if (tag == "c")
        return iconConst;
    else if (tag == "v_")
        return iconVars;
    else if (tag == "c_")
        return iconConsts;
    else if (tag == "f")
        return iconFunc;
    else if (tag == "f_")
        return iconFuncs;
    else if (tag == "m")
        return iconMethod;
    else if (tag == "a")
        return iconFunc;
    return QIcon();
}

