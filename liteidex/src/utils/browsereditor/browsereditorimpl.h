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
// Module: browsereditorimpl.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: browsereditorimpl.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef BROWSEREDITORIMPL_H
#define BROWSEREDITORIMPL_H

#include "liteapi.h"

class BrowserEditorImpl : public LiteApi::IEditor
{
public:
    BrowserEditorImpl(QObject *parent = 0);
    virtual QWidget *widget();
    virtual QString displayName() const;
    virtual QIcon icon() const;

    virtual bool open(const QString &fileName, const QString &mimeType);
    virtual bool save();
    virtual void setReadOnly(bool b);
    virtual bool isReadOnly() const;
    virtual bool isModified() const;
    virtual LiteApi::IFile *file();

    virtual void setDisplayName(const QString &name);
protected:
    QString m_displayName;
};

#endif //BROWSEREDITORIMPL_H
