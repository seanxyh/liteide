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
// Module: qsqldbfile.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-25
// $Id: qsqldbfile.cpp,v 1.0 2011-7-25 visualfc Exp $

#include "qsqldbfile.h"
#include <QDebug>

QSqlDbFile::QSqlDbFile(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IFile(parent),
    m_liteApp(app)
{
}

QSqlDbFile::~QSqlDbFile()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool QSqlDbFile::open(const QString &/*fileName*/, const QString &/*mimeType*/)
{
    return false;
}

bool QSqlDbFile::reload(bool /*externalModify*/)
{
    return false;
}

bool QSqlDbFile::save(const QString &/*fileName*/)
{
    return false;
}

bool QSqlDbFile::isReadOnly() const
{
    return false;
}

QString QSqlDbFile::fileName() const
{
    return m_db.databaseName();
}

QString QSqlDbFile::mimeType() const
{
    return "database/sql";
}

QSqlDatabase &QSqlDbFile::db()
{
    return m_db;
}
