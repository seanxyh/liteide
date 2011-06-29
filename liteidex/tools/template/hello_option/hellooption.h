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
// Module: hellooption.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-6-27
// $Id: hellooption.h,v 1.0 2011-6-27 visualfc Exp $

#ifndef HELLOOPTION_H
#define HELLOOPTION_H

#include "liteapi.h"

namespace Ui {
    class HelloOption;
}

class HelloOption : public LiteApi::IOption
{
    Q_OBJECT

public:
    explicit HelloOption(LiteApi::IApplication *app, QObject *parent = 0);
    ~HelloOption();
    virtual QWidget *widget();
    virtual QString displayName() const;
    virtual QString mimeType() const;
    virtual void apply();
private:
    LiteApi::IApplication   *m_liteApp;
    QWidget           *m_widget;
    Ui::HelloOption *ui;
};

#endif // HELLOOPTION_H
