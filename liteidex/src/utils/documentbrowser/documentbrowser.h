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

#include "liteapi/liteapi.h"
#include "docbrowserapi/docbrowserapi.h"
#include <QStack>
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
    bool findText(bool findBackward);
    QUrl resolveUrl(const QUrl &url) const;
public:
    virtual void setSearchPaths(const QStringList &paths);
    virtual void setUrlHtml(const QUrl &url,const QString &html);
    virtual QToolBar *toolBar();
    virtual QStatusBar *statusBar();
public slots:
    virtual void backward();
    virtual void forward();
public slots:
    void anchorClicked(QUrl);
    void highlighted(QUrl);
    void activatedFindText(QString);
    void findNext();
    void findPrev();
    void activatedUrl(QString);
protected:
    struct HistoryEntry {
        HistoryEntry()
            : hpos(0), vpos(0)
        {}
        QUrl url;
        int hpos;
        int vpos;
    };
    HistoryEntry createHistoryEntry() const;
    void restoreHistoryEntry(const HistoryEntry &entry);
    void setUrlHtml(const QUrl &url,const QString &data,bool html);
    LiteApi::IApplication   *m_liteApp;
    Extension     *m_extension;
    QWidget *m_widget;
    QTextBrowser *m_textBrowser;
    QToolBar     *m_toolBar;
    QAction      *m_backwardAct;
    QAction      *m_forwardAct;
    QComboBox    *m_urlComboBox;
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
    QUrl        m_url;
    QStack<HistoryEntry> m_backwardStack;
    QStack<HistoryEntry> m_forwardStack;
};

#endif // DOCUMENTBROWSER_H
