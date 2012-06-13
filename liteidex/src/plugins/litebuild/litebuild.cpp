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
// Module: litebuild.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-3-26
// $Id: litebuild.cpp,v 1.0 2011-5-12 visualfc Exp $

#include "litebuild.h"
#include "buildmanager.h"
#include "fileutil/fileutil.h"
#include "processex/processex.h"
#include "textoutput/textoutput.h"
#include "liteapi/litefindobj.h"
#include "buildconfigdialog.h"

#include <QToolBar>
#include <QComboBox>
#include <QAction>
#include <QDir>
#include <QFileInfo>
#include <QTextBlock>
#include <QTextCodec>
#include <QProcessEnvironment>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QLabel>
#include <QTime>
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


enum {
    ID_CMD = 0,
    ID_ARGS = 1,
    ID_CODEC = 2,
    ID_MIMETYPE = 3,
    ID_TASKLIST = 4,
    ID_EDITOR = 5
};


class LiteOutput : public TextOutput
{
public:
    LiteOutput(QWidget *parent = 0) : TextOutput(true,parent)
    {
        m_stopAct = new QAction(tr("Stop"),this);
        m_stopAct->setIcon(QIcon("icon:litebuild/images/stopaction.png"));
        m_stopAct->setToolTip("Stop Action");
        m_toolBar->insertAction(m_clearAct,m_stopAct);
        m_toolBar->insertSeparator(m_clearAct);        
    }
    void setInfo(const QString &info)
    {
        m_infoLabel->setText(info);
    }
public:
    QAction *m_stopAct;
};

LiteBuild::LiteBuild(LiteApi::IApplication *app, QObject *parent) :
    LiteApi::ILiteBuild(parent),
    m_liteApp(app),
    m_manager(new BuildManager(this)),
    m_build(0),
    m_envManager(0)
{
    if (m_manager->initWithApp(m_liteApp)) {
        m_manager->load(m_liteApp->resourcePath()+"/litebuild");
        m_liteApp->extension()->addObject("LiteApi.IBuildManager",m_manager);
    }    
    m_toolBar = m_liteApp->actionManager()->insertToolBar("toolbar/litebuild",tr("Build ToolBar"));
    m_liteApp->actionManager()->insertViewMenu(LiteApi::ViewMenuToolBarPos,m_toolBar->toggleViewAction());

    m_liteideModel = new QStandardItemModel(0,2,this);
    m_liteideModel->setHeaderData(0,Qt::Horizontal,tr("Name"));
    m_liteideModel->setHeaderData(1,Qt::Horizontal,tr("Value"));

    m_configModel = new QStandardItemModel(0,2,this);
    m_configModel->setHeaderData(0,Qt::Horizontal,tr("Name"));
    m_configModel->setHeaderData(1,Qt::Horizontal,tr("Value"));

    m_customModel = new QStandardItemModel(0,2,this);
    m_customModel->setHeaderData(0,Qt::Horizontal,tr("Name"));
    m_customModel->setHeaderData(1,Qt::Horizontal,tr("Value"));

    m_configAct = new QAction(QIcon("icon:litebuild/images/config.png"),tr("BuildConfig"),this);
    m_toolBar->addAction(m_configAct);

    m_process = new ProcessEx(this);
    m_output = new LiteOutput;
    m_output->setMaxLine(1024);

    connect(m_output,SIGNAL(hideOutput()),m_liteApp->outputManager(),SLOT(setCurrentOutput()));
    connect(m_output->m_stopAct,SIGNAL(triggered()),this,SLOT(stopAction()));

    m_liteApp->outputManager()->addOutuput(m_output,tr("Build Output"));

    connect(m_liteApp,SIGNAL(loaded()),this,SLOT(appLoaded()));
    connect(m_liteApp->projectManager(),SIGNAL(currentProjectChanged(LiteApi::IProject*)),this,SLOT(currentProjectChanged(LiteApi::IProject*)));
    connect(m_liteApp->editorManager(),SIGNAL(editorCreated(LiteApi::IEditor*)),this,SLOT(editorCreated(LiteApi::IEditor*)));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));
    connect(m_process,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(extOutput(QByteArray,bool)));
    connect(m_process,SIGNAL(extFinish(bool,int,QString)),this,SLOT(extFinish(bool,int,QString)));
    connect(m_output,SIGNAL(dbclickEvent(QTextCursor)),this,SLOT(dbclickBuildOutput(QTextCursor)));
    connect(m_output,SIGNAL(enterText(QString)),this,SLOT(enterTextBuildOutput(QString)));
    connect(m_configAct,SIGNAL(triggered()),this,SLOT(config()));

    m_liteAppInfo.insert("LITEAPPDIR",m_liteApp->applicationPath());

    m_liteApp->extension()->addObject("LiteApi.ILiteBuild",this);
}

LiteBuild::~LiteBuild()
{
    m_liteApp->actionManager()->removeToolBar(m_toolBar);
    delete m_output;
}

QString LiteBuild::actionValue(const QString &value,QMap<QString,QString> &liteEnv,const QProcessEnvironment &env)
{
    QString v = value;
    QMapIterator<QString,QString> i(liteEnv);
    while(i.hasNext()) {
        i.next();
        v.replace("$("+i.key()+")",i.value());
    }
    QRegExp rx("\\$\\((\\w+)\\)");
    int pos = 0;
    QStringList list;
    while ((pos = rx.indexIn(v, pos)) != -1) {
         list << rx.cap(1);
         pos += rx.matchedLength();
    }

    foreach (QString str, list) {
         if (env.contains(str)) {
            v.replace("$("+str+")",env.value(str));
        }
    }
    return v;
}

QString LiteBuild::buildFilePath() const
{
    return m_buildFilePath;
}

QString LiteBuild::targetFilePath() const
{
    QString path = m_targetInfo.value("TARGETPATH");
    if (path.isEmpty()) {
        return path;
    }
    return FileUtil::lookPath(path,m_envManager->currentEnvironment(),true);
}

LiteApi::IBuildManager *LiteBuild::buildManager() const
{
    return m_manager;
}

void LiteBuild::appLoaded()
{
    currentProjectChanged(m_liteApp->projectManager()->currentProject());
    m_envManager = LiteApi::findExtensionObject<LiteApi::IEnvManager*>(m_liteApp,"LiteApi.IEnvManager");
    if (m_envManager) {
        connect(m_envManager,SIGNAL(currentEnvChanged(LiteApi::IEnv*)),this,SLOT(currentEnvChanged(LiteApi::IEnv*)));
        currentEnvChanged(m_envManager->currentEnv());
    }
}

void LiteBuild::config()
{
    if (!m_build) {
        return;
    }
    BuildConfigDialog dlg;
    dlg.setBuild(m_build->id(),m_buildFilePath);
    dlg.setModel(m_liteideModel,m_configModel,m_customModel);
    if (dlg.exec() == QDialog::Accepted) {
        QString key;
        if (!m_buildFilePath.isEmpty()) {
            key = "litebuild-custom/"+m_buildFilePath;
        }
        for (int i = 0; i < m_customModel->rowCount(); i++) {
            QStandardItem *name = m_customModel->item(i,0);
            QStandardItem *value = m_customModel->item(i,1);
            //m_customMap.insert(name->text(),value->text());
            if (!key.isEmpty()) {
                m_liteApp->settings()->setValue(key+"#"+name->text(),value->text());
            }
        }
    }
}

void LiteBuild::currentEnvChanged(LiteApi::IEnv*)
{
    m_process->setEnvironment(m_envManager->currentEnvironment().toStringList());
}

void LiteBuild::loadProjectInfo(const QString &filePath)
{    
    m_projectInfo.clear();
    if (filePath.isEmpty()) {
        return;
    }
    QFileInfo info(filePath);
    /*
PROJECT_NAME
PROJECT_PATH
PROJECT_DIR
PROJECT_DIRNAME
PROJECT_TARGETNAME
PROJECT_TARGETATH
    */
    if (info.isDir()) {
        m_projectInfo.insert("PROJECT_NAME",info.fileName());
        m_projectInfo.insert("PROJECT_PATH",info.filePath());
        m_projectInfo.insert("PROJECT_DIR",info.filePath());
        m_projectInfo.insert("PROJECT_DIRNAME",info.fileName());
    } else {
        m_projectInfo.insert("PROJECT_NAME",info.fileName());
        m_projectInfo.insert("PROJECT_PATH",info.filePath());
        m_projectInfo.insert("PROJECT_DIR",info.path());
        m_projectInfo.insert("PROJECT_DIRNAME",QFileInfo(info.path()).fileName());
    }
}

LiteApi::IBuild *LiteBuild::findProjectBuild(LiteApi::IProject *project)
{
    if (!project) {
        return 0;
    }
    LiteApi::IBuild *build = m_manager->findBuild(project->mimeType());
    return build;
}

void LiteBuild::reloadProject()
{
    LiteApi::IProject *project = (LiteApi::IProject*)sender();
    if (project) {
        loadProjectInfo(project->filePath());
        m_targetInfo = project->targetInfo();
    }
}

void LiteBuild::currentProjectChanged(LiteApi::IProject *project)
{
    m_buildFilePath.clear();
    m_projectInfo.clear();
    m_targetInfo.clear();
    if (project) {
        connect(project,SIGNAL(reloaded()),this,SLOT(reloadProject()));
        loadProjectInfo(project->filePath());
        m_targetInfo = project->targetInfo();
        m_buildFilePath = project->filePath();
        LiteApi::IBuild *build = findProjectBuild(project);
        setProjectBuild(build);
    } else {
        LiteApi::IBuild *build = findProjectBuildByEditor(m_liteApp->editorManager()->currentEditor());
        setProjectBuild(build);
    }
}

QMap<QString,QString> LiteBuild::liteideEnvMap() const
{
    QMap<QString,QString> env = m_liteAppInfo;
    QMapIterator<QString,QString> p(m_projectInfo);
    while(p.hasNext()) {
        p.next();
        env.insert(p.key(),p.value());
    }
    QMapIterator<QString,QString> e(m_editorInfo);
    while(e.hasNext()) {
        e.next();
        env.insert(e.key(),e.value());
    }
    QMapIterator<QString,QString> t(m_targetInfo);
    while(t.hasNext()) {
        t.next();
        env.insert(t.key(),t.value());
    }
    return env;
}

QMap<QString,QString> LiteBuild::buildEnvMap(LiteApi::IBuild *build, const QString &buildFilePath) const
{
    QMap<QString,QString> env = liteideEnvMap();
    if (!build) {
        return env;
    }
    QString customkey;
    if (!buildFilePath.isEmpty()) {
        customkey = "litebuild-custom/"+buildFilePath;
    }
    QString configkey = "litebuild-config/"+build->id();
    foreach(LiteApi::BuildConfig *cf, build->configList()) {
        QString name = cf->name();
        QString value = cf->value();
        if (!configkey.isEmpty()) {
            value = m_liteApp->settings()->value(configkey+"#"+name,value).toString();
        }
        QMapIterator<QString,QString> m(env);
        while(m.hasNext()) {
            m.next();
            value.replace("$("+m.key()+")",m.value());
        }
        env.insert(name,value);
    }
    foreach(LiteApi::BuildCustom *cf, build->customList()) {
        QString name = cf->name();
        QString value = cf->value();
        if (!customkey.isEmpty()) {
            value = m_liteApp->settings()->value(customkey+"#"+name,value).toString();
        }
        QMapIterator<QString,QString> m(env);
        while(m.hasNext()) {
            m.next();
            value.replace("$("+m.key()+")",m.value());
        }
        env.insert(name,value);
    }
    return env;
}

QMap<QString,QString> LiteBuild::buildEnvMap() const
{
    return buildEnvMap(m_build,m_buildFilePath);
    /*
    LiteApi::IBuild *build = m_build;
    QString buildFilePath = m_buildFilePath;
    if (!build) {
        LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
        if (editor && !editor->filePath().isEmpty()) {
            build = m_manager->findBuild(editor->mimeType());
            buildFilePath = QFileInfo(editor->filePath()).path();
        }
    }
    return buildEnvMap(build,buildFilePath);
    */
    /*
    QMap<QString,QString> env = liteideEnvMap();
    QMapIterator<QString,QString> i(m_configMap);
    while(i.hasNext()) {
        i.next();
        QString k = i.key();
        QString v = i.value();
        QMapIterator<QString,QString> m(env);
        while(m.hasNext()) {
            m.next();
            v.replace("$("+m.key()+")",m.value());
        }
        env.insert(k,v);
    }
    QMapIterator<QString,QString> c(m_customMap);
    while(c.hasNext()) {
        c.next();
        QString k = c.key();
        QString v = c.value();
        QMapIterator<QString,QString> m(env);
        while(m.hasNext()) {
            m.next();
            v.replace("$("+m.key()+")",m.value());
        }
        env.insert(k,v);
    }    
    QMapIterator<QString,QString> p(m_projectInfo);
    while(p.hasNext()) {
        p.next();
        env.insert(p.key(),p.value());
    }
    QMapIterator<QString,QString> e(m_editorInfo);
    while(e.hasNext()) {
        e.next();
        env.insert(e.key(),e.value());
    }
    QMapIterator<QString,QString> t(m_targetInfo);
    while(t.hasNext()) {
        t.next();
        env.insert(t.key(),t.value());
    }
    return env;
    */
}

void LiteBuild::setProjectBuild(LiteApi::IBuild *build)
{
    if (m_build == build) {
        if (build) {
            m_output->setInfo(QString("{build id=\"%1\" file=\"%2\"}").
                              arg(build->id()).
                              arg(m_buildFilePath));
        }
        return;
    }
    //update buildconfig
    if (build) {
        m_configModel->removeRows(0,m_configModel->rowCount());
        m_customModel->removeRows(0,m_customModel->rowCount());
        QString customkey;
        if (!m_buildFilePath.isEmpty()) {
            customkey = "litebuild-custom/"+m_buildFilePath;
        }
        QString configkey = "litebuild-config/"+build->id();
        foreach(LiteApi::BuildCustom *cf, build->customList()) {
            QString name = cf->name();
            QString value = cf->value();
            if (!customkey.isEmpty()) {
                value = m_liteApp->settings()->value(customkey+"#"+name,value).toString();
            }
            m_customModel->appendRow(QList<QStandardItem*>()
                                     << new QStandardItem(name)
                                     << new QStandardItem(value));
            //m_customMap.insert(name,value);
        }
        foreach(LiteApi::BuildConfig *cf, build->configList()) {
            QString name = cf->name();
            QString value = cf->value();
            if (!configkey.isEmpty()) {
                value = m_liteApp->settings()->value(configkey+"#"+name,value).toString();
            }
            m_configModel->appendRow(QList<QStandardItem*>()
                                     << new QStandardItem(name)
                                     << new QStandardItem(value));
            //m_configMap.insert(name,value);
        }
        m_output->setInfo(QString("{build id=\"%1\" file=\"%2\"}").
                          arg(build->id()).
                          arg(m_buildFilePath));
    } else {
        m_output->setInfo(QString());
    }

    m_build = build;
    m_manager->setCurrentBuild(build);

    m_outputRegex.clear();

    foreach(QAction *act, m_actions) {
        m_toolBar->removeAction(act);
        delete act;
    }
    m_actions.clear();

    if (!m_build) {
        return;
    }

    m_liteideModel->removeRows(0,m_liteideModel->rowCount());
    QMapIterator<QString,QString> i(this->liteideEnvMap());
    while (i.hasNext()) {
        i.next();
        m_liteideModel->appendRow(QList<QStandardItem*>()
                                 << new QStandardItem(i.key())
                                 << new QStandardItem(i.value()));
    }
    foreach(LiteApi::BuildAction *ba,m_build->actionList()) {
        if (ba->isSeparator()) {
            m_actions.append(m_toolBar->addSeparator());
            continue;
        }
        QAction *act = m_toolBar->addAction(ba->id());
        act->setProperty("mimetype",build->mimeType());
        act->setProperty("id",ba->id());
        if (!ba->key().isEmpty()) {
            QList<QKeySequence> list;
            foreach(QString key, ba->key().split(";")) {
                list.append(QKeySequence(key));
            }
            act->setShortcuts(list);
            act->setToolTip(QString("%1 (%2)").arg(ba->id()).arg(ba->key()));
        }
        if (!ba->img().isEmpty()) {
            QIcon icon(ba->img());
            if (!icon.isNull()) {
                act->setIcon(icon);
            }
        }
        connect(act,SIGNAL(triggered()),this,SLOT(buildAction()));
        m_actions.append(act);
    }
}

void LiteBuild::loadEditorInfo(const QString &filePath)
{
    m_editorInfo.clear();
    if (filePath.isEmpty()) {
        return;
    }
    //m_editorInfo = editor->editorInfo();
    QFileInfo info(filePath);
    /*
EDITOR_BASENAME
EDITOR_NAME
EDITOR_SUFFIX
EDITOR_PATH
EDITOR_DIR
EDITOR_DIRNAME
EDITOR_TARGETNAME
EDITOR_TARGETATH
    */
    m_editorInfo.insert("EDITOR_BASENAME",info.baseName());
    m_editorInfo.insert("EDITOR_NAME",info.fileName());
    m_editorInfo.insert("EDITOR_SUFFIX",info.suffix());
    m_editorInfo.insert("EDITOR_PATH",info.filePath());
    m_editorInfo.insert("EDITOR_DIR",info.path());
    m_editorInfo.insert("EDITOR_DIRNAME",QFileInfo(info.path()).fileName());
    QString target = info.fileName();
    if (!info.suffix().isEmpty()) {
        target = target.left(target.length()-info.suffix().length()-1);
    }
#ifdef Q_OS_WIN
    m_editorInfo.insert("EDITOR_TARGETNAME",target+".exe");
    m_editorInfo.insert("EDITOR_TARGETPATH",QFileInfo(info.path(),target+".exe").filePath());
#else
    m_editorInfo.insert("EDITOR_TARGETNAME",target);
    m_editorInfo.insert("EDITOR_TARGETPATH",QFileInfo(info.path(),target).filePath());
#endif
}

LiteApi::IBuild *LiteBuild::findProjectBuildByEditor(IEditor *editor)
{
    m_buildFilePath.clear();
    m_projectInfo.clear();
    m_targetInfo.clear();

    if (!editor) {
        return 0;
    }
    QString filePath = editor->filePath();
    if (filePath.isEmpty()) {
        return 0;
    }
    QString workDir = QFileInfo(filePath).path();
    LiteApi::IBuild *build = m_manager->findBuild(editor->mimeType());
    LiteApi::IBuild *projectBuild = 0;
    QString projectPath;
    if (build != 0) {
        foreach (LiteApi::BuildLookup *lookup,build->lookupList()) {
            QDir dir(workDir);
            for (int i = 0; i <= lookup->top(); i++) {
                QFileInfoList infos = dir.entryInfoList(QStringList() << lookup->file(),QDir::Files);
                if (infos.size() >= 1) {
                    projectBuild = m_manager->findBuild(lookup->mimeType());
                    if (projectBuild != 0) {
                        projectPath = infos.at(0).filePath();
                        m_buildFilePath = projectPath;
                        break;
                    }
                }
                dir.cdUp();
            }
        }
    }
    if (!projectBuild) {
        return 0;
    }
    loadProjectInfo(projectPath);
    QMap<QString,QString> targetInfo;
    if (m_liteApp->fileManager()->findProjectTargetInfo(projectPath,targetInfo)) {
        m_targetInfo = targetInfo;
    }
    return projectBuild;
}

void LiteBuild::editorCreated(LiteApi::IEditor *editor)
{
    if (!editor) {
        return;
    }
    IBuild *build = m_manager->findBuild(editor->mimeType());
    if (!build) {
        return;
    }
    QToolBar *toolBar = LiteApi::findExtensionObject<QToolBar*>(editor,"LiteApi.QToolBar");
    if (!toolBar) {
        return;
    }
    QList<QAction*> actionList;
    foreach(LiteApi::BuildAction *ba,build->actionList()) {
        QAction *act = new QAction(ba->id(),this);
        act->setProperty("mimetype",build->mimeType());
        act->setProperty("id",ba->id());
        act->setProperty("editor",editor->filePath());
        if (ba->isSeparator()) {
            act->setSeparator(true);
        } else {
            if (!ba->key().isEmpty()) {
                act->setShortcut(QKeySequence(ba->key()));
                act->setToolTip(QString("%1 (%2)").arg(ba->id()).arg(ba->key()));
            }
            if (!ba->img().isEmpty()) {
                QIcon icon(ba->img());
                if (!icon.isNull()) {
                    act->setIcon(icon);
                }
            }
        }
        connect(act,SIGNAL(triggered()),this,SLOT(buildAction()));
        actionList.append(act);
    }
    toolBar->addSeparator();
    toolBar->addActions(actionList);
}

void LiteBuild::currentEditorChanged(LiteApi::IEditor *editor)
{
    m_editorInfo.clear();
    if (!editor) {
        return;
    }
    loadEditorInfo(editor->filePath());
    if (!m_liteApp->projectManager()->currentProject()) {
        LiteApi::IBuild *build = findProjectBuildByEditor(editor);
        setProjectBuild(build);
    }
}

void LiteBuild::extOutput(const QByteArray &data, bool /*bError*/)
{
    if (data.isEmpty()) {
        return;
    }
    m_liteApp->outputManager()->setCurrentOutput(m_output);
    QString codecName = m_process->userData(2).toString();
    QTextCodec *codec = QTextCodec::codecForLocale();
    if (!codecName.isEmpty()) {
        codec = QTextCodec::codecForName(codecName.toAscii());
    }
    m_output->append(codec->toUnicode(data));
}

void LiteBuild::extFinish(bool error,int exitCode, QString msg)
{
    m_output->setReadOnly(true);

    if (error) {
        m_output->appendTag1(QString("> error %1.\n").arg(msg));
    } else {
        m_output->appendTag1(QString("> exit code %1, %2.\n").arg(exitCode).arg(msg));
    }
    m_output->appendTag0(QString("</action>\n"));

    if (!error && exitCode == 0) {
        QStringList task = m_process->userData(ID_TASKLIST).toStringList();
        if (!task.isEmpty()) {
            QString id = task.takeFirst();
            QString mime = m_process->userData(ID_MIMETYPE).toString();
            m_process->setUserData(ID_TASKLIST,task);
            execAction(mime,id);
        }
    } else {
        m_process->setUserData(ID_TASKLIST,QStringList());
    }
}

void LiteBuild::stopAction()
{
    if (m_process->isRuning()) {
        m_process->waitForFinished(100);
        m_process->kill();
    }
}

void LiteBuild::buildAction()
{
    if (m_process->isRuning()) {
        return;
    }

    QAction *act = (QAction*)sender();
    if (!act) {
        return;
    }
    QString mime = act->property("mimetype").toString();
    QString id = act->property("id").toString();
    QString editor = act->property("editor").toString();

    LiteApi::IBuild *build = m_manager->findBuild(mime);
    if (!build) {
        return;
    }
    LiteApi::BuildAction *ba = build->findAction(id);
    if (!ba) {
        return;
    }

    m_liteApp->outputManager()->setCurrentOutput(m_output);
    m_output->updateExistsTextColor();
    m_process->setUserData(ID_MIMETYPE,mime);
    m_process->setUserData(ID_EDITOR,editor);
    if (ba->task().isEmpty()) {
        execAction(mime,id);
    } else {
        QStringList task = ba->task();
        QString id = task.takeFirst();
        m_process->setUserData(ID_TASKLIST,task);
        execAction(mime,id);
    }
}

void LiteBuild::execAction(const QString &mime, const QString &id)
{
    if (m_process->isRuning()) {
        return;
    }

    LiteApi::IBuild *build = m_manager->findBuild(mime);
    if (!build) {
        return;
    }

    LiteApi::BuildAction *ba = build->findAction(id);
    if (!ba) {
        return;
    }
    QString codec = ba->codec();
    LiteApi::IEditor *editor = m_liteApp->editorManager()->currentEditor();
    if (ba->save() == "project") {
        if (editor && editor->isModified()) {
            m_liteApp->editorManager()->saveEditor();
        }
        m_liteApp->projectManager()->saveProject();
    } else if(ba->save() == "editor") {
        if (editor && editor->isModified()) {
            m_liteApp->editorManager()->saveEditor();
        }
    } else if (ba->save() == "all") {
        m_liteApp->editorManager()->saveAllEditors();
    }

    QString editorPath = m_process->userData(ID_EDITOR).toString();
    QString buildFilePath;
    if (!editorPath.isEmpty()) {
        buildFilePath = QFileInfo(editorPath).path();
    } else {
        buildFilePath = m_buildFilePath;
    }

    QMap<QString,QString> env = buildEnvMap(build,buildFilePath);

    QProcessEnvironment sysenv = LiteApi::getGoEnvironment(m_liteApp);
    QString cmd = this->actionValue(ba->cmd(),env,sysenv);
    QString args = this->actionValue(ba->args(),env,sysenv);

    m_workDir = this->actionValue(build->work(),env,sysenv);
    QString work = ba->work();
    if (!work.isEmpty()) {
        m_workDir = this->actionValue(work,env,sysenv);
    }

    if (!QFileInfo(cmd).exists()) {
        QString findCmd = FileUtil::lookPathInDir(cmd,sysenv,m_workDir);
        if (!findCmd.isEmpty()) {
            cmd = findCmd;
        }
    }

    if (cmd.indexOf("$(") >= 0 || args.indexOf("$(") >= 0 || m_workDir.isEmpty()) {
        m_output->appendTag1(QString("> error, can not execute action '%1'\n").arg(ba->id()));
        m_process->setUserData(3,QStringList());
        return;
    }


    if (!ba->regex().isEmpty()) {
        m_outputRegex = this->actionValue(ba->regex(),env,sysenv);
    }

    QStringList arguments =  args.split(" ",QString::SkipEmptyParts);
    if (ba->output() && ba->readline()) {
        m_output->setReadOnly(false);
    } else {
        m_output->setReadOnly(true);
    }
    m_process->setEnvironment(sysenv.toStringList());
    if (!ba->output()) {
        bool b = QProcess::startDetached(cmd,arguments,m_workDir);
        m_output->appendTag0(QString("<action id=\"%1\" cmd=\"%2\" args=\"%3\" work=\"%4\">\n")
                             .arg(id).arg(ba->cmd()).arg(ba->args()).arg(m_workDir));
        m_output->appendTag1(QString("> %1 %2\n").arg(cmd).arg(args));
        m_output->appendTag1(QString("> Start process %1\n").arg(b?"success":"false"));
        m_output->appendTag0(QString("</action>\n"));
    } else {
        m_process->setUserData(0,cmd);
        m_process->setUserData(1,args);
        m_process->setUserData(2,codec);

        m_process->setWorkingDirectory(m_workDir);        
        m_output->appendTag0(QString("<action id=\"%1\" cmd=\"%2\" args=\"%3\" work=\"%4\">\n")
                             .arg(id).arg(ba->cmd()).arg(ba->args()).arg(m_workDir));
        m_output->appendTag1(QString("> %1 %2\n").arg(cmd).arg(args));
        m_process->start(cmd,arguments);
    }
}

void LiteBuild::enterTextBuildOutput(QString text)
{
    if (!m_process->isRuning()) {
        return;
    }
    QTextCodec *codec = QTextCodec::codecForLocale();
    QString codecName = m_process->userData(2).toString();
    if (!codecName.isEmpty()) {
        codec = QTextCodec::codecForName(codecName.toAscii());
    }
    if (codec) {
        m_process->write(codec->fromUnicode(text));
    } else {
        m_process->write(text.toAscii());
    }
}

void LiteBuild::dbclickBuildOutput(const QTextCursor &cur)
{
    QRegExp rep(m_outputRegex);//"([\\w\\d:_\\\\/\\.]+):(\\d+)");

    int index = rep.indexIn(cur.block().text());
    if (index < 0)
        return;
    QStringList capList = rep.capturedTexts();

    if (capList.count() < 3)
        return;
    QString fileName = capList[1];
    QString fileLine = capList[2];

    bool ok = false;
    int line = fileLine.toInt(&ok);
    if (!ok)
        return;

    QDir dir(m_workDir);
    QString filePath = dir.filePath(fileName);
    if (QFile::exists(filePath)) {
        fileName = filePath;
    } else {
        foreach(QFileInfo info,dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot)) {
            QString filePath = info.absoluteDir().filePath(fileName);
            if (QFile::exists(filePath)) {
                fileName = filePath;
                break;
            }
        }
    }
    /*
    LiteApi::IProject *project = m_liteApp->projectManager()->currentProject();
    if (project) {
        fileName = project->fileNameToFullPath(fileName);
    } else {
        QDir dir(m_workDir);
        QString filePath = dir.filePath(fileName);
        if (QFile::exists(filePath)) {
            fileName = filePath;
        } else {
            foreach(QFileInfo info,dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot)) {
                QString filePath = info.absoluteDir().filePath(fileName);
                if (QFile::exists(filePath)) {
                    fileName = filePath;
                    break;
                }
            }
        }
    }
    */
    LiteApi::IEditor *editor = m_liteApp->fileManager()->openEditor(fileName);
    if (editor) {
        editor->widget()->setFocus();
        LiteApi::ITextEditor *textEditor = LiteApi::findExtensionObject<LiteApi::ITextEditor*>(editor,"LiteApi.ITextEditor");
        if (textEditor) {
            textEditor->gotoLine(line,0,true);
        }
    }
}
