#include "gopathproject.h"
#include "gopathmodel.h"
#include "liteapi/litefindobj.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeView>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QDebug>

GopathProject::GopathProject(QWidget *widget) : m_widget(widget)
{
}

GopathProject::~GopathProject()
{
    m_widget->hide();
}

QWidget *GopathProject::widget()
{
    return m_widget;
}

QString GopathProject::name() const
{
    return m_path;
}


QString GopathProject::filePath() const
{
    return m_path;
}

QString GopathProject::mimeType() const
{
    return "text/x-gopath";
}

QStringList GopathProject::fileNameList() const
{
    QDir dir(m_path);
    QStringList list;
    foreach(QFileInfo info,dir.entryInfoList(QDir::Files)) {
        list.append(info.fileName());
    }
    return list;
}

QStringList GopathProject::filePathList() const
{
    QDir dir(m_path);
    return dir.entryList(QDir::Files);
}

QString GopathProject::fileNameToFullPath(const QString &filePath)
{
    return QFileInfo(m_path,filePath).filePath();
}

QString GopathProject::target() const
{
    return QFileInfo(m_path).fileName();
}

QString GopathProject::targetPath() const
{
    return QFileInfo(m_path,this->target()).filePath();
}

QString GopathProject::workPath() const
{
    return m_path;
}

void GopathProject::load()
{
    m_widget->show();
}
