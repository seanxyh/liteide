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

class LiteOutput : public TextOutput
{
public:
    LiteOutput(QWidget *parent = 0) : TextOutput(true,parent)
    {
        m_stopAct = new QAction(tr("Stop"),this);
        m_stopAct->setIcon(QIcon(":/images/stopaction.png"));
        m_stopAct->setToolTip("Stop Action");
        m_toolBar->insertAction(m_clearAct,m_stopAct);
        m_toolBar->insertSeparator(m_clearAct);
    }
public:
    QAction *m_stopAct;
};

LiteBuild::LiteBuild(LiteApi::IApplication *app, QObject *parent) :
    QObject(parent),
    m_liteApp(app),
    m_manager(new BuildManager(this)),
    m_build(0),
    m_envManager(0)
{
    if (m_manager->initWithApp(m_liteApp)) {
        m_manager->load(m_liteApp->resourcePath()+"/build");
        m_liteApp->extension()->addObject("LiteApi.IBuildManager",m_manager);
    }
    m_toolBar = new QToolBar;
    m_toolBar->setObjectName("BuildToolBar");

    m_configModel = new QStandardItemModel(0,2,this);
    m_configModel->setHeaderData(0,Qt::Horizontal,tr("Name"));
    m_configModel->setHeaderData(1,Qt::Horizontal,tr("Value"));

    m_configAct = new QAction(QIcon(":/images/config.png"),tr("Config"),this);
    m_toolBar->addAction(m_configAct);
    m_toolBar->addSeparator();

    m_liteApp->actionManager()->addToolBar(m_toolBar);

    m_process = new ProcessEx(this);
    m_output = new LiteOutput;
    connect(m_output,SIGNAL(hideOutput()),m_liteApp->outputManager(),SLOT(setCurrentOutput()));
    connect(m_output->m_stopAct,SIGNAL(triggered()),this,SLOT(stopAction()));

    m_liteApp->outputManager()->addOutuput(m_output,tr("LiteBuild"));

    connect(m_liteApp,SIGNAL(loaded()),this,SLOT(appLoaded()));
    connect(m_liteApp->projectManager(),SIGNAL(currentProjectChanged(LiteApi::IProject*)),this,SLOT(currentProjectChanged(LiteApi::IProject*)));
    connect(m_liteApp->editorManager(),SIGNAL(currentEditorChanged(LiteApi::IEditor*)),this,SLOT(currentEditorChanged(LiteApi::IEditor*)));
    connect(m_process,SIGNAL(extOutput(QByteArray,bool)),this,SLOT(extOutput(QByteArray,bool)));
    connect(m_process,SIGNAL(extFinish(bool,int,QString)),this,SLOT(extFinish(bool,int,QString)));
    connect(m_output,SIGNAL(dbclickEvent()),this,SLOT(dbclickBuildOutput()));
    connect(m_output,SIGNAL(enterText(QString)),this,SLOT(enterTextBuildOutput(QString)));
    connect(m_configAct,SIGNAL(triggered()),this,SLOT(config()));

    m_liteApp->outputManager()->showOutput(m_output);
}

LiteBuild::~LiteBuild()
{
    m_liteApp->actionManager()->removeToolBar(m_toolBar);
    delete m_toolBar;
    delete m_output;
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
    BuildConfigDialog dlg;
    dlg.setModel(m_configModel);
    if (dlg.exec() == QDialog::Accepted) {
        for (int i = 0; i < m_configModel->rowCount(); i++) {
            QStandardItem *name = m_configModel->item(i,0);
            QStandardItem *value = m_configModel->item(i,1);
            m_liteEnv.insert(name->text(),value->text());
        }
    }
}

void LiteBuild::currentEnvChanged(LiteApi::IEnv*)
{
    m_process->setEnvironment(m_envManager->currentEnvironment().toStringList());
}

void LiteBuild::resetLiteEnv(LiteApi::IProject *project)
{
    QString projectPath,projectDir,projectName;
    QString workDir,targetPath,targetDir,targetName;
    if (project) {
        projectPath = project->fileName();
        projectDir = QFileInfo(projectPath).absolutePath();
        projectName = QFileInfo(projectPath).fileName();
        workDir = project->workPath();
        targetPath = project->targetPath();
        targetName = project->target();
        targetDir = QFileInfo(targetPath).absolutePath();
    }
    m_liteEnv.insert("${LITEAPPDIR}",m_liteApp->applicationPath());
    m_liteEnv.insert("${PROJECTPATH}",projectPath);
    m_liteEnv.insert("${PROJECTDIR}",projectDir);
    m_liteEnv.insert("${PROJECTNAME}",projectName);
    m_liteEnv.insert("${WORKDIR}",workDir);
    m_liteEnv.insert("${TARGETPATH}",targetPath);
    m_liteEnv.insert("${TARGETDIR}",targetDir);
    m_liteEnv.insert("${TARGETNAME}",targetName);
}

void LiteBuild::reloadProject()
{
    LiteApi::IProject *project = (LiteApi::IProject*)sender();
    resetLiteEnv(project);
}

void LiteBuild::currentProjectChanged(LiteApi::IProject *project)
{
    LiteApi::IBuild *build = 0;
    resetLiteEnv(project);
    if (project) {
        connect(project,SIGNAL(reloaded()),this,SLOT(reloadProject()));
        build =  m_manager->findBuild(project->mimeType());
        if (build) {
            m_liteApp->actionManager()->showToolBar(m_toolBar);
            setCurrentBuild(build);
        } else {
            m_liteApp->actionManager()->hideToolBar(m_toolBar);
        }
    } else {
        currentEditorChanged(m_liteApp->editorManager()->currentEditor());
    }
}

void LiteBuild::setCurrentBuild(LiteApi::IBuild *build)
{
    if (m_build == build) {
        return;
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

    m_configModel->removeRows(0,m_configModel->rowCount());
    foreach(LiteApi::BuildConfig *cf, m_build->configList()) {
        m_configModel->appendRow(QList<QStandardItem*>()
                                 << new QStandardItem(cf->name())
                                 << new QStandardItem(cf->value()));
        m_liteEnv.insert(cf->name(),cf->value());
    }

    foreach(LiteApi::BuildAction *ba,m_build->actionList()) {
        QAction *act = m_toolBar->addAction(ba->id());
        if (!ba->key().isEmpty()) {
            act->setShortcut(QKeySequence(ba->key()));
            act->setToolTip(QString("%1(%2)").arg(ba->id()).arg(ba->key()));
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

void LiteBuild::currentEditorChanged(LiteApi::IEditor *editor)
{
    if (m_liteApp->projectManager()->currentProject()) {
        return;
    }
    QString editorPath,editorDir,editorName;
    QString workDir,targetPath,targetDir,targetName;

    LiteApi::IBuild *build = 0;
    if (editor) {
        QString fileName = editor->fileName();
        if (!fileName.isEmpty()) {
            editorPath = fileName;
            editorDir = QFileInfo(editorPath).absolutePath();
            editorName = QFileInfo(editorPath).fileName();
            build = m_manager->findBuild(editor->mimeType());
        }
        workDir = editorDir;
        targetDir = editorDir;
        targetName = QFileInfo(editorPath).baseName();
        targetPath = targetDir+"/"+targetName;
    }
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
                        break;
                    }
                }
                dir.cdUp();
            }
        }
    }
    if (projectBuild != 0) {
        build = projectBuild;
        QString projectDir = QFileInfo(projectPath).absolutePath();
        QString projectName = QFileInfo(projectPath).fileName();
        workDir = projectDir;
        m_liteApp->fileManager()->targetInfo(projectPath,targetName,targetPath,workDir);
        m_liteEnv.insert("${LITEAPPDIR}",m_liteApp->applicationPath());
        m_liteEnv.insert("${PROJECTPATH}",projectPath);
        m_liteEnv.insert("${PROJECTDIR}",projectDir);
        m_liteEnv.insert("${PROJECTNAME}",projectName);
        m_liteEnv.insert("${WORKDIR}",workDir);
        m_liteEnv.insert("${TARGETPATH}",targetPath);
        m_liteEnv.insert("${TARGETDIR}",targetDir);
        m_liteEnv.insert("${TARGETNAME}",targetName);
    } else {
        m_liteEnv.insert("${LITEAPPDIR}",m_liteApp->applicationPath());
        m_liteEnv.insert("${EDITORPATH}",editorPath);
        m_liteEnv.insert("${EDITORDIR}",editorDir);
        m_liteEnv.insert("${EDITORNAME}",editorName);
        m_liteEnv.insert("${WORKDIR}",workDir);
        m_liteEnv.insert("${TARGETPATH}",targetPath);
        m_liteEnv.insert("${TARGETDIR}",targetDir);
        m_liteEnv.insert("${TARGETNAME}",targetName);
    }
    if (build) {
        m_liteApp->actionManager()->showToolBar(m_toolBar);
        setCurrentBuild(build);
    } else {
        m_liteApp->actionManager()->hideToolBar(m_toolBar);
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
    int len = data.length();
    if (len >= 1 && data.at(len-1) == '\n') {
        int right = 1;
        if (len >= 2 && data.at(len-2) == '\r') {
            right++;
        }
        if (codec) {
            m_output->append(codec->toUnicode(data.left(len-right)));
        } else {
            m_output->append(data.left(len-right));
        }
    } else {
        if (codec) {
            m_output->append(codec->toUnicode(data));
        } else {
            m_output->append(data);
        }
    }
}

void LiteBuild::extFinish(bool error,int exitCode, QString msg)
{
    m_output->plainTextEdit()->setReadOnly(true);
    if (error) {
        m_output->appendTag1(QString("<error msg=\"%1\" />").arg(msg));
    } else {
        m_output->appendTag1(QString("<exit code=\"%1\" msg=\"%2\"/>").arg(exitCode).arg(msg));
    }
    m_output->appendTag0(QString("</action>"));
    m_output->moveToEnd();

    if (!error && exitCode == 0) {
        QStringList task = m_process->userData(3).toStringList();
        if (!task.isEmpty()) {
            QString id = task.takeFirst();
            m_process->setUserData(3,task);
            execAction(id);
        }
    } else {
        m_process->setUserData(3,QStringList());
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
    QAction *act = (QAction*)sender();
    if (!act) {
        return;
    }
    QString id = act->text();
    LiteApi::BuildAction *ba = m_build->findAction(id);
    if (!ba) {
        return;
    }
    if (m_process->isRuning()) {
        return;
    }

    m_liteApp->outputManager()->setCurrentOutput(m_output);
    m_output->updateExistsTextColor();

    if (ba->task().isEmpty()) {
        execAction(act->text());
    } else {
        QStringList task = ba->task();
        QString id = task.takeFirst();
        m_process->setUserData(3,task);
        execAction(id);
    }
}

void LiteBuild::execAction(const QString &id)
{
    if (!m_build) {
        return;
    }

    if (m_process->isRuning()) {
        return;
    }

    LiteApi::BuildAction *ba = m_build->findAction(id);
    if (!ba) {
        return;
    }
    QString codec = ba->codec();
    if (!ba->regex().isEmpty()) {
        m_outputRegex = ba->regex();
    }
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
    }

    QString workDir = m_liteEnv.value("${WORKDIR}");

    QString cmd;
    if (m_envManager) {
        cmd = m_build->actionCommand(ba,m_liteEnv,m_envManager->currentEnvironment());
    } else {
        cmd = m_build->actionCommand(ba,m_liteEnv,QProcessEnvironment::systemEnvironment());
    }

    QString args = m_build->actionArgs(ba,m_liteEnv);

    QStringList arguments =  args.split(" ",QString::SkipEmptyParts);

    if (m_output->plainTextEdit()->document()->lineCount() > 1024) {
        m_output->clear();
    }
    if (!ba->output()) {
        bool b = QProcess::startDetached(cmd,arguments,workDir);
        m_output->plainTextEdit()->setReadOnly(true);
        m_output->appendTag0(QString("<action id=\"%1\" cmd=\"%2\" args=\"%3\">")
                             .arg(id).arg(ba->cmd()).arg(ba->args()));
        m_output->appendTag1(QString("<run=\"%1 %2\" workdir=\"%3\"/>").
                             arg(cmd).arg(args).arg(workDir));
        m_output->append(QString("Start process %1").arg(b?"success":"false"));
        m_output->appendTag0(QString("</action>"));
        m_output->moveToEnd();
        return;
    } else {
        m_output->plainTextEdit()->setReadOnly(false);
        m_process->setUserData(0,cmd);
        m_process->setUserData(1,args);
        m_process->setUserData(2,codec);
        m_process->setWorkingDirectory(workDir);
        m_output->appendTag0(QString("<action id=\"%1\" cmd=\"%2\" args=\"%3\">")
                             .arg(id).arg(ba->cmd()).arg(ba->args()));
        m_output->appendTag1(QString("<start=\"%1 %2\" workdir=\"%3\"/>").
                             arg(cmd).arg(args).arg(workDir));
        m_output->moveToEnd();
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

void LiteBuild::dbclickBuildOutput()
{
    QTextCursor cur = m_output->plainTextEdit()->textCursor();
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

    cur.select(QTextCursor::LineUnderCursor);
    m_output->plainTextEdit()->setTextCursor(cur);

    LiteApi::IProject *project = m_liteApp->projectManager()->currentProject();
    if (project) {
        fileName = project->fileNameToFullPath(fileName);
    } else {
        QString workDir = m_liteEnv.value("${WORKDIR}");
        QDir dir(workDir);
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

    LiteApi::IEditor *editor = m_liteApp->fileManager()->openEditor(fileName);
    if (editor) {
        editor->widget()->setFocus();
        LiteApi::ITextEditor *textEditor = LiteApi::findExtensionObject<LiteApi::ITextEditor*>(editor,"LiteApi.ITextEditor");
        if (textEditor) {
            textEditor->gotoLine(line,0);
        }
    }
}
