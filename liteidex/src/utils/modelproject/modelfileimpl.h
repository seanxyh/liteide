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
// Module: modelfileimpl.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: modelfileimpl.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef MODELFILEIMPL_H
#define MODELFILEIMPL_H

#include "liteapi.h"

class QStandardItemModel;
class ModelFileImpl : public LiteApi::IFile
{
    Q_OBJECT
public:
    ModelFileImpl(LiteApi::IApplication *app, QObject *parent);
    enum ITEM_TYPE{
        ItemRoot = 1,
        ItemFolder,
        ItemProFile,
        ItemFile,
    };
public:
    virtual bool open(const QString &fileName, const QString &mimeType);
    virtual bool reload(bool externalModify);
    virtual bool save(const QString &fileName);
    virtual QString fileName() const;
    virtual QString mimeType() const;
protected:
    virtual bool loadFile(const QString &fileName) = 0;
    virtual void updateModel() = 0;
public:
    virtual QString target() const = 0;
    QStandardItemModel *model() const;
    QString value(const QString &key) const;
    QStringList values(const QString &key) const;
    QStringList fileList() const;
    QString fileNameToFullPath(const QString &fileName);
protected:
    LiteApi::IApplication *m_liteApp;
    QStandardItemModel       *m_model;
    QString     m_rootPath;
    QMap<QString,QStringList>   m_context;
    QStringList m_fileList;
    QString     m_mimeType;
    QString     m_fileName;
};

#endif // MODELFILEIMPL_H
