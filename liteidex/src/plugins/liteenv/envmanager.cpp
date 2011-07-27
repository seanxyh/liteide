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
// Module: envmanager.cpp
// Creator: visualfc <visualfc@gmail.com>
// date: 2011-7-25
// $Id: envmanager.cpp,v 1.0 2011-7-25 visualfc Exp $

#include "envmanager.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QToolBar>
#include <QComboBox>
#include <QLabel>
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

Env::Env(QObject *parent) :
    LiteApi::IEnv(parent)
{
    m_env = QProcessEnvironment::systemEnvironment();
}

QString Env::id() const
{
    return m_id;
}

QProcessEnvironment Env::env() const
{
    return m_env;
}

void Env::reload()
{
    if (m_filePath.isEmpty()) {
        return;
    }
    QFile f(m_filePath);
    if (!f.open(QIODevice::ReadOnly)) {
        return;
    }
    m_env = loadEnv(&f);
    f.close();
}

QProcessEnvironment Env::loadEnv(QIODevice *dev)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#ifdef Q_OS_WIN
    QRegExp rx("\\%([\\w]+)\\%");
#else
    QRegExp rx("\\$([\\w]+)");
#endif
    while (!dev->atEnd()) {
        QString line = dev->readLine().trimmed();
        int pos = line.indexOf("=");
        if (pos == -1) {
            continue;
        }
        QString key = line.left(pos).trimmed();
        QString value = line.right(line.length()-pos-1).trimmed();
        pos = 0;
        int rep = 0;
        while (rep++ < 10) {
            pos = rx.indexIn(value,pos);
            if (pos == -1) {
                break;
            }
            QString v = env.value(rx.cap(1));
            if (!v.isEmpty()) {
                value.replace(pos,rx.cap(0).length(),v);
                pos = 0;
            } else {
                pos += rx.matchedLength();
            }
        }
        env.insert(key,value);
    }
    return env;
}

void Env::loadEnv(EnvManager *manager, const QString &filePath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly)) {
        return;
    }

    Env *env = new Env(manager);
    env->m_filePath = filePath;
    env->m_id = QFileInfo(filePath).baseName();
    env->m_env = loadEnv(&f);
    f.close();
    manager->addEnv(env);
}

EnvManager::EnvManager(QObject *parent)
    : LiteApi::IEnvManager(parent),
      m_curEnv(0)
{
}

EnvManager::~EnvManager()
{
    if (m_curEnv) {
        m_liteApp->settings()->setValue("LiteEnv/current",m_curEnv->id());
    }
}

void EnvManager::addEnv(LiteApi::IEnv *env)
{
    m_envList.append(env);
}

void EnvManager::removeEnv(LiteApi::IEnv *env)
{
    m_envList.removeAll(env);
}

LiteApi::IEnv *EnvManager::findEnv(const QString &id) const
{
    foreach (LiteApi::IEnv *env, m_envList) {
        if (env->id() == id) {
            return env;
        }
    }
    return NULL;
}

QList<LiteApi::IEnv*> EnvManager::envList() const
{
    return m_envList;
}

void EnvManager::setCurrentEnv(LiteApi::IEnv *env)
{
    if (m_curEnv == env) {
        return;
    }
    m_curEnv = env;
    if (m_curEnv) {
        m_curEnv->reload();
    }
    emit currentEnvChanged(m_curEnv);
}

LiteApi::IEnv *EnvManager::currentEnv() const
{
    return m_curEnv;
}

QProcessEnvironment EnvManager::currentEnvironment() const
{
    if (m_curEnv) {
        return m_curEnv->env();
    }
    return QProcessEnvironment::systemEnvironment();
}

void EnvManager::loadEnvFiles(const QString &path)
{
    QDir dir = path;
    m_liteApp->appendConsole("LiteEnv","LoadEnvFiles",path);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(QStringList("*.env"));
    foreach (QString fileName, dir.entryList()) {
        Env::loadEnv(this,QFileInfo(dir,fileName).absoluteFilePath());
    }
}

bool EnvManager::initWithApp(LiteApi::IApplication *app)
{
    if (!LiteApi::IEnvManager::initWithApp(app)) {
        return false;
    }
    loadEnvFiles(m_liteApp->resourcePath()+"/environment");

    m_toolBar = new QToolBar(tr("LiteEnv"));
    m_toolBar->setObjectName("LiteEnv");

    m_envCmb = new QComboBox;
    m_envCmb->setToolTip(tr("Environment"));

    m_toolBar->addWidget(new QLabel(tr("Env:")));
    m_toolBar->addWidget(m_envCmb);

    foreach (LiteApi::IEnv *env, m_envList) {
        m_envCmb->addItem(env->id());
    }

    m_liteApp->extension()->addObject("LiteApi.IEnvManager",this);

    QString id = m_liteApp->settings()->value("LiteEnv/current","system").toString();
    if (!id.isEmpty()) {
        this->setCurrentEnvId(id);
    }

    m_liteApp->actionManager()->addToolBar(m_toolBar);
    connect(m_envCmb,SIGNAL(activated(QString)),this,SLOT(envActivated(QString)));

    return true;
}

void EnvManager::setCurrentEnvId(const QString &id)
{
    LiteApi::IEnv *env = findEnv(id);
    if (!env) {
        return;
    }
    for (int i = 0; i < m_envCmb->count(); i++) {
        if (m_envCmb->itemText(i) == id) {
            m_envCmb->setCurrentIndex(i);
            break;
        }
    }
    setCurrentEnv(env);
}


void EnvManager::envActivated(QString id)
{
    LiteApi::IEnv *env = findEnv(id);
    setCurrentEnv(env);
}
