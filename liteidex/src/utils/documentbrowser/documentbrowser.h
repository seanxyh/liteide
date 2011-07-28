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
// Module: documentbrowser.h
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-7
// $Id: documentbrowser.h,v 1.0 2011-7-26 visualfc Exp $

#ifndef DOCUMENTBROWSER_H
#define DOCUMENTBROWSER_H

#include "liteapi.h"
#include "docbrowserapi/docbrowserapi.h"
#include <QUrl>

class QTextBrowser;
class QComboBox;
class QToolButton;
class QCheckBox;
class Extension;
class DocumentBrowser : public LiteApi::IDocumentBrowser
{
    Q_OBJECT
public:
    explicit DocumentBrowser(LiteApi::IApplication *app, QObject *parent);
    ~DocumentBrowser();
public:
    virtual LiteApi::IExtension *extension();
    virtual QWidget *widget();
    virtual bool open(const QString &fileName,const QString &mimeType);
    virtual QString name() const;
    virtual QString fileName() const;
    virtual QString mimeType() const;
    void setName(const QString &t);
    void setFileName(const QString &t);
    virtual QTextBrowser *browser();
    bool findText(bool findBackward);
public slots:
    void highlighted(QUrl);
    void activatedFindText(QString);
    void findNext();
    void findPrev();
protected:
    LiteApi::IApplication   *m_liteApp;
    Extension     *m_extension;
    QWidget *m_widget;
    QTextBrowser *m_textBrowser;
    QStatusBar   *m_statusBar;
    QComboBox    *m_findComboBox;
    QCheckBox    *m_matchCaseCheckBox;
    QCheckBox    *m_matchWordCheckBox;
    QCheckBox    *m_useRegexCheckBox;
    QAction     *m_findNextAct;
    QAction     *m_findPrevAct;
    QString     m_name;
    QString     m_fileName;
    QString     m_mimeType;
};

#endif // DOCUMENTBROWSER_H
