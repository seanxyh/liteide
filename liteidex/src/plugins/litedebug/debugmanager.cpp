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
// Module: debugmanager.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-8-12
// $Id: debugmanager.cpp,v 1.0 2011-8-12 visualfc Exp $

#include "debugmanager.h"

DebugManager::DebugManager(QObject *parent) :
    IDebugManager(parent),
    m_currentDebug(0)
{
}

DebugManager::~DebugManager()
{
    qDeleteAll(m_debugList);
}

void DebugManager::addDebug(IDebug *debug)
{
    m_debugList.append(debug);
}

void DebugManager::removeDebug(IDebug *debug)
{
    m_debugList.removeOne(debug);
}

IDebug *DebugManager::findDebug(const QString &mimeType)
{
    foreach(IDebug *debug, m_debugList) {
        if (debug->mimeType() == mimeType) {
            return debug;
        }
    }
    return 0;
}

QList<IDebug*> DebugManager::debugList() const
{
    return m_debugList;
}

void DebugManager::setCurrentDebug(IDebug *debug)
{
    m_currentDebug = debug;
    emit currentDebugChanged(m_currentDebug);
}

IDebug *DebugManager::currentDebug()
{
    return m_currentDebug;
}
