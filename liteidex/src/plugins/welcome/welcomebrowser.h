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
// Module: welcomebrowser.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: welcomebrowser.h,v 1.0 2011-5-12 visualfc Exp $

#ifndef WELCOMEBROWSER_H
#define WELCOMEBROWSER_H

#include "browsereditor/browsereditorimpl.h"
#include <QStringListModel>

namespace Ui {
    class WelcomeWidget;
}

class DocumentBrowser;
class QStandardItemModel;
class WelcomeBrowser : public BrowserEditorImpl
{
    Q_OBJECT
public:
    WelcomeBrowser(LiteApi::IApplication *app);
    virtual ~WelcomeBrowser();
    virtual QWidget *widget();
public slots:
    void openLiteDoument(QModelIndex);
    void openRecentProjectList(QModelIndex);
    void openRecentFileList(QModelIndex);
    void recentProjectsChanged();
    void recentFilesChanged();
protected:
    LiteApi::IApplication *m_liteApp;
    QWidget *m_widget;
    Ui::WelcomeWidget *ui;
    QStringListModel    *m_recentProjectsModel;
    QStringListModel    *m_recentFilesModel;
    QStandardItemModel  *m_docModel;
    DocumentBrowser     *m_docBrowser;
    QAction             *m_browserAct;
};

#endif // WELCOMEBROWSER_H
