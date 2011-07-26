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
// Module: qsqlfilefactory.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-25
// $Id: qsqlfilefactory.cpp,v 1.0 2011-7-25 visualfc Exp $

#include "qsqlfilefactory.h"
#include "sqlitefile.h"
#include "qsqleditor.h"

#include <QDebug>

QSqlFileFactory::QSqlFileFactory(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::IFileFactory(parent),
    m_liteApp(app)
{
}

QStringList QSqlFileFactory::mimeTypes() const
{
    return QStringList() << "database/sqlite";
}

LiteApi::IFile *QSqlFileFactory::open(const QString &fileName, const QString &mimeType)
{
    QSqlDbFile *file = 0;
    if (mimeType == "database/sqlite") {
        file = new SQLiteFile(m_liteApp);
        if (!file->open(fileName,mimeType)) {
            delete file;
            return 0;
        }
    }
    if (file) {
        QSqlEditor *editor = new QSqlEditor(m_liteApp);
        editor->setFile(file);
        m_liteApp->editorManager()->addAutoReleaseEditor(editor);
    }
    return file;
}

bool QSqlFileFactory::targetInfo(const QString &/*fileName*/, const QString &/*mimetype*/, QString &/*target*/, QString &/*targetPath*/, QString &/*workPath*/) const
{
    return false;
}
