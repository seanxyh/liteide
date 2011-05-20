/**************************************************************************
**
** This file is part of LiteIDE
**
** Copyright (c) 2011 LiteIDE team. All rights reserved.
**
** Contact: visualfc (visualfc@gmail.com)
**
** No Commercial Usage
**
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception,  that plugins developed for LiteIDE,
** are allowed to remain closed sourced and can be distributed under any license .
** These rights are included in the file LGPL_EXCEPTION.txt in this package.
**
**
** If you have questions regarding the use of this file, please contact
** visualfc at visualfc@gmail.com.
**
**************************************************************************/

//filename: golangcode_global.h
//anchor: visualfc <visualfc@gmail.com>
//date:	2011-3-26

#ifndef GOLANGCODE_GLOBAL_H
#define GOLANGCODE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GOLANGCODE_LIBRARY)
#  define GOLANGCODESHARED_EXPORT Q_DECL_EXPORT
#else
#  define GOLANGCODESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GOLANGCODE_GLOBAL_H
