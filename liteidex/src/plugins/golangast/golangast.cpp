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
// Module: golangast.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: golangast.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "golangast.h"
#include "litefindobj.h"

#include <QDockWidget>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QTimer>
#include <QDebug>

//lite_memory_check_begin
#if defined(WIN32) && defined(_MSC_VER) &&  defined(_DEBUG)
     #define _CRTDBG_MAP_ALLOC
     #include <stdlib.h>
     #include <crtdbg.h>
     #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
     #define new DEBUG_NEW
#endif
//lite_memory_check_end

class GolangAstItem : public QStandardItem
{
public:
    void setTagName(const QString &tagName)
    {
        m_tagName = tagName;
    }
    QString tagName() const
    {
        return m_tagName;
    }
    void setFileName(const QString &fileName)
    {
        m_fileName = fileName;
    }
    QString fileName() const
    {
        return m_fileName;
    }
    void setLine(int line)
    {
        m_line = line;
    }
    void setCol(int col)
    {
        m_col = col;
    }
    int line() const
    {
        return m_line;
    }
    int col() const
    {
        return m_col;
    }
protected:
    QString m_tagName;
    QString m_fileName;
    int     m_line;
    int     m_col;
};

GolangAst::GolangAst(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app)
{
    m_widget = new QWidget;
    m_tree = new SymbolTreeView;
    m_model = new QStandardItemModel(this);
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSourceModel(m_model);

    m_tree->setModel(proxyModel);
    m_tree->setExpandsOnDoubleClick(false);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->addWidget(m_tree);
    m_widget->setLayout(mainLayout);
    m_process = new QProcess(this);

    m_timer = new QTimer(this);
    m_bEnable = false;
    m_bVisible = true;
    QDockWidget *widget = m_liteApp->dockManager()->addDock(m_widget,tr("GoAstView"),Qt::LeftDockWidgetArea);
    connect(widget,SIGNAL(visibilityChanged(bool)),this,SLOT(visibilityChanged(bool)));
    connect(m_liteApp->projectManager(),SIGNAL(currentProjectChanged(LiteApi::IProject*)),this,SLOT(projectChanged(LiteApi::IProject*)));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(editorChanged(LiteApi::IEditor*)));
    connect(m_liteApp->editorManager(),SIGNAL(editorSaved(LiteApi::IEditor*)),this,SLOT(editorSaved(LiteApi::IEditor*)));
    connect(m_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finishedProcess(int,QProcess::ExitStatus)));
    connect(m_timer,SIGNAL(timeout()),this,SLOT(updateAstNow()));
    connect(m_tree,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClockedTree(QModelIndex)));

    m_liteApp->extension()->addObject("GoAstView.Widget",m_widget);
}

GolangAst::~GolangAst()
{
    delete m_process;
    m_liteApp->dockManager()->removeDock(m_widget);
    delete m_widget;
}

void GolangAst::setEnable(bool b)
{
    if (m_bEnable == b) {
        return;
    }
    m_bEnable = b;
    if (b && m_bVisible) {
        // m_liteApp->dockManager()->showDock(m_widget);
        projectChanged(m_liteApp->projectManager()->currentProject());
        editorChanged(m_liteApp->editorManager()->currentEditor());
        updateAst();
    } else {
        m_model->clear();
        // m_liteApp->dockManager()->hideDock(m_widget);
    }
}

void GolangAst::visibilityChanged(bool b)
{
    if (m_bEnable) {
        m_bVisible = b;
        if (b) {
            updateAst();
        }
    }
}

void GolangAst::projectChanged(LiteApi::IProject *project)
{
    m_updateFiles.clear();
    m_model->clear();
    if (project) {
        foreach(QString file, project->fileList()) {
            if (QFileInfo(file).suffix() == "go") {
                m_updateFiles.append(file);
            }
        }
        updateAst();
    } else {
        LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
        if (editor) {
            editorChanged(editor);
        }
    }
}

void GolangAst::editorChanged(LiteApi::IEditor *editor)
{
    if (!m_liteApp->projectManager()->currentProject()) {
        m_updateFiles.clear();
        m_model->clear();
        if (editor) {
            LiteApi::IFile *file = editor->file();
            if (file) {
                QString fileName = file->fileName();
                if (QFileInfo(fileName).suffix() == "go") {
                    m_updateFiles.append(fileName);
                }
            }
            updateAst();
        }
    }
}

void GolangAst::editorSaved(LiteApi::IEditor *editor)
{
    if (editor) {
        LiteApi::IFile *file = editor->file();
        if (file) {
            QString fileName = file->fileName();
            if (m_updateFiles.contains(fileName)) {
                updateAst();
            }
        }
    }
}

void GolangAst::updateAst()
{
    if (!m_bVisible) {
        return;
    }
    m_timer->stop();
    m_timer->start(1000);
}

void GolangAst::updateAstNow()
{
    m_timer->stop();
    if (m_updateFiles.isEmpty()) {
        return;
    }
#ifdef Q_OS_WIN
    QString goastview = "goastview.exe";
#else
    QString goastview = "goastview";
#endif
    QString cmd = m_liteApp->applicationPath();
    cmd += "/";
    cmd += goastview;
    QStringList args;
    args << "-files";
    args << m_updateFiles.join(" ");
    m_process->start(cmd,args);
}

void GolangAst::finishedProcess(int code,QProcess::ExitStatus status)
{
    if (code == 0 && status == QProcess::NormalExit) {
        updateModel(m_process->readAllStandardOutput());
    } else {
        qDebug() << m_process->readAllStandardError();
    }
}

static QStringList stringListFromIndex(const QModelIndex &index)
{
    QStringList list;
    if (!index.isValid())
        return list;
    list.append(stringListFromIndex(index.parent()));
    list.append(index.data().toString());
    return list;
}

static QModelIndex indexFromStringList(QSortFilterProxyModel *model, QStringList &list, const QModelIndex & parent = QModelIndex())
{
    if (list.isEmpty())
        return QModelIndex();
    QString text = list.front();
    for (int i = 0; i < model->rowCount(parent); i++) {
        QModelIndex child = model->index(i,0,parent);
        if (child.data().toString() == text) {
            list.pop_front();
            if (list.isEmpty()) {
                return child;
            } else {
                QModelIndex next = indexFromStringList(model,list,child);
                if (next.isValid())
                    return next;
                else
                    return child;
            }
        }
    }
    return QModelIndex();
}


// level,tag,name,index,x,y
void GolangAst::updateModel(const QByteArray &data)
{
    //save state
    QList<QStringList> expands;
    QSetIterator<QModelIndex> i(m_tree->expandIndexs());
    while (i.hasNext()) {
        QStringList path = stringListFromIndex(i.next());
        expands.append(path);
    }
    QStringList topState = stringListFromIndex(m_tree->topViewIndex());
    QStringList curState = stringListFromIndex(m_tree->currentIndex());

    m_model->clear();

    QList<QByteArray> array = data.split('\n');
    QMap<int,QStandardItem*> items;
    QStringList indexFiles;
    bool ok = false;
    QMap<QString,GolangAstItem*> level1NameItemMap;
    foreach (QByteArray line, array) {
        QList<QByteArray> info = line.split(',');
        if (info.size() == 2 && info.at(0) == "@") {
            indexFiles.append(info.at(1));
        }
        if (info.size() < 3) {
            continue;
        }
        int level = info[0].toInt(&ok);
        if (!ok) {
            continue;
        }
        QString tag = info[1];
        QString name = info[2];
        if (name.isEmpty() || tag.isEmpty()) {
            continue;
        }
        if (level == 0) {
            level1NameItemMap.clear();
        }
        GolangAstItem *item = 0;
        if (level == 1) {
            item = level1NameItemMap.value(name);
            if (item != 0) {
                items[level] = item;
                continue;
            }
        }
        item = new GolangAstItem;
        if (level == 1) {
            level1NameItemMap.insert(name,item);
        }
        item->setText(name);
        if (name.at(0).isLower()) {
            item->setIcon(icons_p.iconFromTag(tag));
        } else {
            item->setIcon(icons.iconFromTag(tag));
        }
        if (info.size() >= 6) {
            int index = info[3].toInt(&ok);
            if (ok && index >= 0 && index < indexFiles.size()) {
                item->setFileName(indexFiles.at(index));
            }
            int line = info[4].toInt(&ok);
            if (ok) {
                item->setLine(line);
            }
            int col = info[5].toInt(&ok);
            if (ok) {
                item->setCol(col);
            }
        }
        QStandardItem *parent = items.value(level-1,0);
        if (parent ) {
            parent->appendRow(item);
        } else {
            m_model->appendRow(item);
        }
        items[level] = item;
    }

    //load state
    m_tree->expandToDepth(0);

    QListIterator<QStringList> ie(expands);
    while (ie.hasNext()) {
        QStringList expandPath = ie.next();
        QModelIndex expandIndex = indexFromStringList(proxyModel,expandPath);
        if (expandIndex.isValid()) {
            m_tree->setExpanded(expandIndex,true);
        }
    }

    QModelIndex curIndex = indexFromStringList(proxyModel,curState);
    if (curIndex.isValid()) {
        m_tree->setCurrentIndex(curIndex);
    }

    QModelIndex topIndex = indexFromStringList(proxyModel,topState);
    if (topIndex.isValid()) {
        m_tree->scrollTo(topIndex, QTreeView::PositionAtTop);
    }
}

void GolangAst::doubleClockedTree(QModelIndex proxyIndex)
{
    QModelIndex index = proxyModel->mapToSource(proxyIndex);
    if (!index.isValid()) {
        return;
    }
    GolangAstItem *item = (GolangAstItem*)m_model->itemFromIndex(index);
    if (item == NULL)
        return;
    QString fileName = item->fileName();
    if (fileName.isEmpty()) {
        if (m_tree->isExpanded(proxyIndex)) {
            m_tree->collapse(proxyIndex);
        } else {
            m_tree->expand(proxyIndex);
        }
        return;
    }
    LiteApi::IEditor *editor = m_liteApp->editorManager()->loadEditor(fileName);
    if (!editor) {
        return;
    }
    m_liteApp->editorManager()->setCurrentEditor(editor);
    editor->widget()->setFocus();
    LiteApi::ITextEditor *textEditor = LiteApi::findExtensionObject<LiteApi::ITextEditor*>(editor,"LiteApi.ITextEditor");
    if (!textEditor) {
        return;
    }
    textEditor->gotoLine(item->line(),item->col());
}
